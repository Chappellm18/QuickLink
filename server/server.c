#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../database/datasource.h"

#define PORT 8080

void handle_client(SSL *ssl, struct db_conn *db) {
    char buf[4096] = {0};
    SSL_read(ssl, buf, sizeof(buf));

    printf("Request:\n%s\n", buf);

    // Example: use the database
    add_link(db, "https://example.com");

    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 12\r\n"
        "\r\n"
        "Hello World!";

    SSL_write(ssl, response, strlen(response));
}

void start_server(struct db_conn *db) {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    // Load cert and key
    if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { perror("socket failed"); exit(1); }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind failed"); exit(1); }
    if (listen(sockfd, 10) < 0) { perror("listen failed"); exit(1); }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        int client_fd = accept(sockfd, NULL, NULL);
        if (client_fd < 0) { perror("accept failed"); continue; }

        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_fd);

        if (SSL_accept(ssl) <= 0) { ERR_print_errors_fp(stderr); }
        else { handle_client(ssl, db); }

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client_fd);
    }

    close(sockfd);
    SSL_CTX_free(ctx);
    EVP_cleanup();
}
