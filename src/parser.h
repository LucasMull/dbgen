#include "../dbgen.h"

typedef struct {
    unsigned int amt_row; //aka database size
    unsigned short amt_col;
    char delim;
    FILE *out_stream;
} dbconfig;

typedef struct colinfo {
    short method;
    char *lwall;
    char *rwall;
    char *amount;
    char *file;
    char delim;
    char decimal_places;

    struct colinfo *ukey; // unique key,
} t_colinfo;

t_colinfo *parser(int argc, char *argv[], dbconfig *database);

void count_colinfo(int, char *[], short*, short*);

int def_ukey(char*[], t_colinfo*, dbconfig*);
int def_method(char*[], t_colinfo*, dbconfig*);
int def_field(char*[], t_colinfo*, dbconfig*);
int def_delim(char*[], t_colinfo*, dbconfig*);

char *continue_then_init( int(*)(int), char*, short*);

void print_colinfo(t_colinfo*, short amt_info);
void init_colinfo(t_colinfo*, short amt_info);
void clean_colinfo(t_colinfo*);
void destroy_colinfo(t_colinfo*, short amt_info);

dbconfig *init_dbconfig();
void destroy_dbconfig(dbconfig* database);
