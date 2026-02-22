#include <stdio.h>
#include <stdlib.h>
#include "database/datasource.h"

// Forward declaration of the server start function
// server.c will implement: void start_server(struct db_conn *db);
void start_server(struct db_conn *db);

int main() {
    struct db_conn db;

    // Initialize database
    if (!opendb(&db, "database/mydatabase.db")) {
        fprintf(stderr, "Failed to open database. Exiting.\n");
        return 1;
    }

    printf("Database initialized successfully.\n");

    // Start the server, passing the database connection
    start_server(&db);

    // Clean up database connection
    closedb(&db);

    return 0;
}

