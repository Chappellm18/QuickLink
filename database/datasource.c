#include <stdio.h>
#include <sqlite3.h>
#include <string.h>

struct db_conn {
	int rc;  
	sqlite3 *db;
}

struct db_conn opendb(char db_name[]) {
	struct db_conn db1;
	db1.rc = sqlite3_open(db_name, &db1.db);

	if(db1.rc != SQLITE_OK) {
		fprintf(stderr, "Error opening database: %s\n",sqlite3_errmsg(db1.db));
	} else {
		fprintf(stderr, "Database opened successfully\n");
	}
	
	return db1;
}

int add_link(struct db_conn db, const char *link) {

	sqlite3_stmt *stmt;
    	const char *sql = "INSERT INTO LINKS (LINK) VALUES (?);";

    	int rc = sqlite3_prepare_v2(db.db, sql, -1, &stmt, NULL);
    	if (rc != SQLITE_OK) {
        	fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db.db));
        	return 1;
    	}

    	sqlite3_bind_text(stmt, 1, link, -1, SQLITE_STATIC);

    	rc = sqlite3_step(stmt);
    	if (rc != SQLITE_DONE) {
        	fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db.db));
    	} else {
        	printf("Link inserted safely\n");
    	}

    	sqlite3_finalize(stmt);
	return 0;
}

char *get_link(struct db_conn db, int id) {
	sqlite3_stmt *stmt;
	const char *sql = "SELECT LINK FROM LINKS WHERE ID = ?;";
	char *result = NULL;

	int rc = sqlite3_prepare_v2(db.db, sql, -1, &stmt, NULL);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "Prepare failed : %s\n", sqlite3_errmsg(db.db));
        	return NULL;
    	}

    	sqlite3_bind_int(stmt, 1, id);

    	rc = sqlite3_step(stmt);

    	if (rc == SQLITE_ROW) {
        	const unsigned char *text = sqlite3_column_text(stmt, 0);

        	if (text != NULL) {
            		result = malloc(strlen((const char *)text) + 1);
		
			if (result != NULL) {
                		strcpy(result, (const char *)text);
            		}
        	}
    	} else if (rc == SQLITE_DONE) {
        	printf("No record found.\n");
    	} else {
        	fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db.db));
    	}

    	sqlite3_finalize(stmt);
    	return result;
}

int main() {
        struct db_conn db = opendb();

        return 0;
}
