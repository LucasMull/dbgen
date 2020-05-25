#include <stdio.h>
#include "directive.h"

typedef struct templ {
    double value;
} t_templ;

typedef struct list {
    char **data;
    size_t size;
} t_list;

typedef struct colgen {
    char *method;
    int gentype; //can be either _templ or _list generated
    union {
        t_templ *_template;
        t_list *_list;
    };

    size_t amount;
    double lwall;
    double rwall;
    char delim;
    //t_tree *link; //if a column has values to be linked
} t_colgen;
/*
void init_list(void *list);

int free_list(t_list *list, size_t first_i);

void init_templ(void *templ);

void file_to_list(FILE* f_read, t_list* list, size_t ln_total);

void nums_to_list(t_list *list, long int first, long int last, size_t amount, size_t length);
*/
