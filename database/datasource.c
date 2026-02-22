#include "datasource.h"
#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>

// Open database connection
int opendb(struct db_conn *conn, const char *db_name) {
    conn->rc = sqlite3_open(db_name, &conn->db);

    if (conn->rc != SQLITE_OK) {
        fprintf(stderr, "Error opening database: %s\n",
                sqlite3_errmsg(conn->db));
        return 0;  // failure
    }

    printf("Database opened successfully\n");
    return 1;  // success
}

// Close database connection
void closedb(struct db_conn *conn) {
    sqlite3_close(conn->db);
}

// Insert a link into the LINKS table safely using a prepared statement
int add_link(struct db_conn *conn, const char *link) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO LINKS (LINK) VALUES (?);";

    int rc = sqlite3_prepare_v2(conn->db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(conn->db));
        return 1;
    }

    sqlite3_bind_text(stmt, 1, link, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(conn->db));
    } else {
        printf("Link inserted safely\n");
    }

    sqlite3_finalize(stmt);
    return 0;
}

// Get a link by ID
int get_link(struct db_conn *conn, int id, char *out_link, int max_len) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT LINK FROM LINKS WHERE ID = ?;";

    int rc = sqlite3_prepare_v2(conn->db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(conn->db));
        return 1;
    }

    sqlite3_bind_int(stmt, 1, id);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const unsigned char *text = sqlite3_column_text(stmt, 0);
        strncpy(out_link, (const char*)text, max_len - 1);
        out_link[max_len - 1] = '\0'; // ensure null-termination
        sqlite3_finalize(stmt);
        return 0; // success
    } else if (rc == SQLITE_DONE) {
        fprintf(stderr, "No link found with ID %d\n", id);
        sqlite3_finalize(stmt);
        return 2; // not found
    } else {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(conn->db));
        sqlite3_finalize(stmt);
        return 3; // error
    }
}
