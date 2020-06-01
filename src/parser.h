#include "../dbgen.h"
#define MAX_OPTIONS 4

typedef struct {
    size_t size;
    char delim;
    int buffer[BUFSIZ];
    short amt_cols;
    FILE *out_stream;
} dbconfig;

typedef struct colinfo {
    char option[MAX_OPTIONS];
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

void def_link(char*, t_colinfo*);
void def_option(char*, t_colinfo*);
void def_field(char*, t_colinfo*);
void def_delim(char*, t_colinfo*);

char *continue_then_init( int(*)(int), char*, short*);

void print_colinfo(t_colinfo*, short amt_info);
void init_colinfo(t_colinfo*, short amt_info);
void clean_colinfo(t_colinfo*, short amt_info);
