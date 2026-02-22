#ifndef _datasource_h
# define _datasource_h

struct db_conn opendb(char db_name[]);

int add_link(struct db_conn, const char *link);

char *get_link(struct db_conn db, int id);

int main();

#endif
