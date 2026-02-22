#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int main() {
	int sockfd, clientfd;
	struct sockaddr_in addr;

	// Initialize OpenSSL
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
	SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
	if (!ctx) {
	ERR_print_errors_fp(stderr);
		return 1;
	    }

	    // Load certificate and private key
	    if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0 ||
		SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		return 1;
	    }

	    // Create socket
	    sockfd = socket(AF_INET, SOCK_STREAM, 0);

	    addr.sin_family = AF_INET;
	    addr.sin_port = htons(8080);
	    addr.sin_addr.s_addr = INADDR_ANY;

	    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	    listen(sockfd, 10);

	    printf("Waiting for connection on port 8080...\n");

	    clientfd = accept(sockfd, NULL, NULL);

	    SSL *ssl = SSL_new(ctx);
	    SSL_set_fd(ssl, clientfd);

	    if (SSL_accept(ssl) <= 0) {
		    ERR_print_errors_fp(stderr);
	} else {

	    char request[4096] = {0};
	    SSL_read(ssl, request, sizeof(request));
	    printf("Request:\n%s\n", request);

	    FILE *file = fopen("../ui/index.html", "rb");
	    if (!file) {
		const char *not_found =
		    "HTTP/1.1 404 Not Found\r\n"
		    "Content-Type: text/plain\r\n"
		    "Content-Length: 13\r\n"
		    "\r\n"
		    "404 Not Found";

		SSL_write(ssl, not_found, strlen(not_found));
	    } else {
		fseek(file, 0, SEEK_END);
		long file_size = ftell(file);
		rewind(file);

		char *file_buffer = malloc(file_size);
		fread(file_buffer, 1, file_size, file);
		fclose(file);

		char header[512];
		snprintf(header, sizeof(header),
			 "HTTP/1.1 200 OK\r\n"
			 "Content-Type: text/html\r\n"
			 "Content-Length: %ld\r\n"
			 "\r\n",
                	 file_size);

        	SSL_write(ssl, header, strlen(header));
        	SSL_write(ssl, file_buffer, file_size);

        	free(file_buffer);
    	}
	}

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(clientfd);
    close(sockfd);
    SSL_CTX_free(ctx);

    return 0;
}
