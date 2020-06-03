typedef struct node {
        char *svalue;

        struct node *next;
        struct node *prev;
} t_node;

typedef struct list {
    char *svalue;
    double dvalue;

    t_node **root;
    size_t tree_size;
} t_list;

typedef struct templ {
    char *svalue;
    double dvalue;
} t_templ;

typedef struct colgen {
    char method;
    int gentype; //can be either _templ or _list generated
    union {
        t_templ *_template;
        t_list **_list;
    };

    unsigned int amt_row;
    char delim;

    void (*fn)(struct colgen*, dbconfig*);
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
    char format_data[10]; //format data to be printed
} t_colgen;

t_list *init_list();
void start_arrlist(t_colgen*, dbconfig*);
void destroy_list(t_list*, short);
void destroy_arrlist(short, t_list**, int);
void print_list(size_t, t_list**, short);

t_templ *init_templ();
void start_templ(t_colgen*, t_templ*, dbconfig*);
void destroy_templ(t_templ*);
void print_templ(t_templ*);

void def_typeof(t_colinfo*, t_colgen*);

t_colgen *init_colgen(dbconfig*);
t_colgen *start_colgen(t_colinfo*, t_colgen*, dbconfig*);
t_colgen **start_arrcolgen(t_colinfo*, dbconfig*);

void destroy_colgen(t_colgen*);
void destroy_arrcolgen(t_colgen**, int);
void print_arrcolgen(t_colgen**, int);
