typedef struct entity {
    union {
        char *svalue;
        double dvalue;
    };
} t_entity;

typedef struct {
    t_entity tag; //if dependency is of gentype file tag will be svalue
                //otherwise will be dvalue

    union { //if ukey's gentype is file will store svalue,
            //otherwise will store dvalue
        char **store_svalue;
        double *store_dvalue;
    };
    unsigned int n_amt;
} linkstorage;

typedef struct ukey {
    linkstorage **storage;
    unsigned int n_amt;
} t_ukey;

typedef struct colgen {
    char method; // how entity will be generated
                 // random, unique, scalable (...)

    int gentype; //can be either _template, _list or _ukey gentype
    union {
        t_entity *_template;

        struct {
            t_entity **_list;
            t_entity *_curr_entity; // points to index of current output entity
        };                   // will point to NULL if column prints
    };                       // incremental values

    t_ukey *_ukey;
    struct colgen *_dependency; // holds addr of dependency

    void (*fn)(struct colgen*, dbconfig*);

    unsigned int amt_row; //aka max amount of elements in that column

    char delim; //will be set to newline on the last column
    char format_entity[10]; //format entity to be printed

    union { //can be either range of numbers or file's content
        struct { 
            double lwall; //start of range
            double rwall; //end of range
            char decimal_places;
        };
        char *file;
    };
} t_colgen;

t_entity *init_entity();

void start_arrlist(t_colgen*, dbconfig*);
void destroy_list(t_entity*, t_colgen*);
void destroy_arrlist(t_entity**, t_colgen*);
void print_arrlist(size_t, t_entity**, short);

void start_templ(t_colgen*, t_entity*, dbconfig*);
void destroy_templ(t_entity*);
void print_templ(t_entity*);

linkstorage *init_linkstorage();
linkstorage *start_linkstorage(t_colgen*, t_colgen*, linkstorage*);
void destroy_linkstorage(linkstorage*, const int);

t_ukey *init_ukey();
void start_ukey(t_colgen*, t_colgen*, t_ukey*, dbconfig*);
void destroy_ukey(t_ukey*, const int);

t_colgen *init_colgen(dbconfig*);
t_colgen *start_colgen(t_colinfo*, t_colgen*, t_colgen*, dbconfig*);
int start_ukey_colgen(t_colgen**, t_colgen*, t_colinfo*, dbconfig*);
t_colgen **start_arrcolgen(t_colinfo*, dbconfig*);

void destroy_colgen(t_colgen*);
void destroy_arrcolgen(t_colgen**, int);
void print_arrcolgen(t_colgen**, int);
