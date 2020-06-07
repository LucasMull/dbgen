typedef struct data {
    union {
        char *svalue;
        double dvalue;
    };
} t_data;

typedef struct {
    t_data tag; //if linker is gentype file tag will be svalue
                //otherwise will be dvalue

    union { //if link's gentype is file will store svalue,
            //otherwise will store dvalue
        char **store_svalue;
        double *store_dvalue;
    };
    unsigned int n_amt;
} linkstorage;

typedef struct link {
    linkstorage **storage;
    unsigned int n_amt;
} t_link;

typedef struct colgen {
    char method; // how data will be generated
                 // random, unique, scalable (...)

    int gentype; //can be either _template, _list or _link gentype
    union {
        t_data *_template;

        struct {
            t_data **_list;
            t_data *_lindex; // points to index of last output list
        };                   // will point to NULL if column prints
    };                       // incremental values

    t_link *_link;
    struct colgen *_linker; //a reference to linker

    void (*fn)(struct colgen*, dbconfig*);

    unsigned int amt_row; //aka max amount of elements in that column

    char delim; //will be set to newline on the last column
    char format_data[10]; //format data to be printed

    union { //can be either range of numbers or file's content
        struct { 
            double lwall; //start of range
            double rwall; //end of range
            char decimals;
        };
        char *file;
    };
} t_colgen;

t_data *init_data();

void start_arrlist(t_colgen*, dbconfig*);
void destroy_list(t_data*, t_colgen*);
void destroy_arrlist(t_data**, t_colgen*);
void print_arrlist(size_t, t_data**, short);

void start_templ(t_colgen*, t_data*, dbconfig*);
void destroy_templ(t_data*);
void print_templ(t_data*);

linkstorage *init_linkstorage();
linkstorage *start_linkstorage(t_colgen*, t_colgen*, linkstorage*);
void destroy_linkstorage(linkstorage*, const int);

t_link *init_link();
void start_link(t_colgen*, t_colgen*, t_link*, dbconfig*);
void destroy_link(t_link*, const int);

t_colgen *init_colgen(dbconfig*);
t_colgen *start_colgen(t_colinfo*, t_colgen*, t_colgen*, dbconfig*);
t_colgen **start_arrcolgen(t_colinfo*, dbconfig*);

void destroy_colgen(t_colgen*);
void destroy_arrcolgen(t_colgen**, int);
void print_arrcolgen(t_colgen**, int);
