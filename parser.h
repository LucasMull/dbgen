#define MAX_OPTIONS 4
#define _error(A,B) fprintf(stderr, "dbgen: %s%s\n", A, B)

#define ERR_READ "READ ERROR -- "
#define ERR_FILE "CANT READ FILE -- "
#define ERR_LINK "CANT LINK MORE THAN TWO FILES"

typedef struct colinfo {
    char option[MAX_OPTIONS];
    char *lwall;
    char *rwall;
    char *amount;
    char *file;
    char delim;

    struct colinfo *link;
} t_colinfo;

t_colinfo *parser(int argc, char *argv[], int *ret_amt_info);

int count_colinfo(int, char *[]);

void def_option(char*, t_colinfo*);
void def_colinfo(char*, t_colinfo*);
void def_delim(char*, t_colinfo*);

char *continue_then_init( int(*)(int), char*, short*);

void print_colinfo(t_colinfo*, int amt_info);
void init_colinfo(t_colinfo*, int amt_info);
void clean_colinfo(t_colinfo*, int amt_info);
