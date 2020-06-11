#include "../dbgen.h"

typedef struct {
    unsigned int amt_rows; //aka database size
    unsigned short amt_cols;
    char delim;
    int buffer[BUFSIZ];
    FILE *out_stream;
} dbconfig;

typedef struct colinfo {
    short option;
    char *lwall;
    char *rwall;
    char *amount;
    char *file;
    char delim;
    char decimals;

    struct colinfo *link;
} t_colinfo;

t_colinfo *parser(int argc, char *argv[], dbconfig *database);

short count_colinfo(int, char *[]);

int def_link(char*[], t_colinfo*, dbconfig*);
int def_option(char*[], t_colinfo*, dbconfig*);
int def_field(char*[], t_colinfo*, dbconfig*);
int def_delim(char*[], t_colinfo*, dbconfig*);

char *continue_then_init( int(*)(int), char*, short*);

void print_colinfo(t_colinfo*, short amt_info);
void init_colinfo(t_colinfo*, short amt_info);
void clean_colinfo(t_colinfo*, short amt_info);

dbconfig *init_dbconfig();
void destroy_dbconfig(dbconfig* database);
