#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <sqlite3.h>

struct db_conn {
    int rc;
    sqlite3 *db;
};

int opendb(struct db_conn *conn, const char *db_name);
void closedb(struct db_conn *conn);
int add_link(struct db_conn *conn, const char *link);
int get_link(struct db_conn *conn, int id, char *out_link, int max_len);

#endif
