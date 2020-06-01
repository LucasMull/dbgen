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
    union {
        double dvalue;
        char *svalue;
    };
} t_templ;

typedef struct colgen {
    char method;
    int gentype; //can be either _templ or _list generated
    union {
        t_templ *_template;
        t_list **_list;
    };

    size_t amt_row;
    char delim;

    void (*fn)(struct colgen*);
    struct colgen *_linker;

   //union because if gentype is File there is no reason to use range,
   //and if gentype is not File then file won't be used
    union {
        struct {
            double lwall; //start of range
            double rwall; //end of range
            char decimals;
        };
        char *file;
    };
} t_colgen;

t_list *init_list();
void start_arrlist(t_colgen*);
void destroy_list(t_list*, short);
void destroy_arrlist(short, t_list**, int);
void print_list(size_t, t_list**, short);

t_templ *init_templ();
void start_templ(t_colgen*, t_templ*);
void destroy_templ(t_templ*);
void print_templ(t_templ*);

void def_typeof(t_colinfo*, t_colgen*);

t_colgen *init_colgen(dbconfig*);
t_colgen *start_colgen(t_colinfo*, t_colgen*, dbconfig*);
t_colgen **start_arrcolgen(t_colinfo*, dbconfig*);

void destroy_colgen(t_colgen*);
void destroy_arrcolgen(t_colgen**, int);
void print_arrcolgen(t_colgen**, int);
