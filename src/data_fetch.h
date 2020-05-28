#include <stdio.h>
#include "directive.h"

typedef struct node {
        union {
            char *svalue;
            double *dvalue;
        };
        struct node *next;
        struct node *prev;
} t_node;

typedef struct list {
    union {
        char *svalue;
        double *dvalue;
    };

    t_node **root;
    size_t tree_size;
} t_list;

typedef struct templ {
    double value;
    //function that will update value according to method
    void (*fn)(struct templ*);
} t_templ;

typedef struct colgen {
    char *method;

    int gentype; //can be either _templ or _list generated
    union {
        t_templ *_template;
        t_list **_list;
    };

    size_t amt_row;

    struct colgen *_linker;

    double lwall;
    double rwall;

    char delim;

    char decimals;
} t_colgen;
