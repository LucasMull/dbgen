#include <stdio.h>

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
        double dvalue;
    };

    t_node **root;
    size_t tree_size;
} t_list;

typedef struct templ {
    double dvalue;
    //function that will update value according to method
    void (*fn)(struct templ*);
} t_templ;

typedef struct colgen {
    char method;

    int gentype; //can be either _templ or _list generated
    union {
        t_templ *_template;
        t_list **_list;
    };

    size_t amt_row;

    double lwall;
    double rwall;

    char delim;

    char decimals;

    struct colgen *_linker;
} t_colgen;

void gen_scalable(t_list**, t_colgen*);
void gen_incremental(t_list**, t_colgen*);
//void gen_random(t_list**, t_colgen*);

void nums_to_arrlist(t_list** arrlist, t_colgen* colgen);
void file_to_arrlist(FILE* f_read, t_list** arrlist, t_colgen* colgen);
