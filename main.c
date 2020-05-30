#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "src/data_fetch.h"
#include "src/parser.h"

#define DEFAULT_DBSIZE 10

enum gentype {
    Template=1, 
    List=2,
    File=4,
    TotalGens,

    Error=-1
};

enum methodtype { 
    Rnd=1, 
    Unq=2, 
    Scl=4, 
    TotalMethods,

    Undef=-1
};

t_list *init_list();
void start_arrlist(t_colgen*);
void destroy_list(t_list*, short);
void destroy_arrlist(short, t_list**, int);
void print_list(size_t, t_list**, short);

t_templ *init_templ();
void start_templ(t_colgen*, t_templ*);
void destroy_templ(t_templ*);
void print_templ(t_templ*);

void gen_type(t_colinfo*, t_colgen*);

t_colgen *init_colgen();
t_colgen *start_colgen(t_colinfo*, t_colgen*);
t_colgen **start_arrcolgen(t_colinfo*, int);
void destroy_colgen(t_colgen*);
void destroy_arrcolgen(t_colgen**, int);
void print_arrcolgen(t_colgen**, int);


int main(int argc, char *argv[])
{
    short amt_cols;

    t_colinfo *info = parser(argc, argv, &amt_cols);

    int amt_link = 0;
    for ( int i = 0; i < amt_cols; ++i ){
        if ( (info+i)->link ){
            t_colinfo *ptr_link = (info+i)->link;
            while ( ptr_link ){
                ptr_link = ptr_link->link;
                ++amt_link; 
            }
        }
    }

    t_colgen **colgen = start_arrcolgen(info, amt_cols+amt_link);

    print_colinfo(info,amt_cols);
    clean_colinfo(info,amt_cols);
    
    print_arrcolgen(colgen, amt_cols+amt_link);
    destroy_arrcolgen(colgen, amt_cols+amt_link);    
    return 0;
}

t_list *init_list()
{
    t_list *new_list = malloc(sizeof(t_list));
    assert(new_list);

    new_list->svalue = NULL;

    new_list->root = NULL;

    new_list->tree_size = 0;

    return new_list;
}

void destroy_arrlist(short gentype, t_list** list, int amt_row)
{
    assert(list);

        for ( int i = 0; i < amt_row; ++i ){
            destroy_list(list[i], gentype);
        } free(list);
}

void destroy_list(t_list* list, short gentype)
{
    assert(list);

    if (( gentype == ( File | List ) ) && ( list->svalue ))
        free(list->svalue);
    //implement free traversing tree
    if ( list->root )
        free(list->root);
     free(list);
}


t_templ *init_templ()
{
    t_templ *new_templ = malloc(sizeof(t_templ));
    assert(new_templ);

    new_templ->dvalue = 0;
    new_templ->fn = NULL;

    return new_templ;
}

void start_templ(t_colgen* colgen, t_templ* templ)
{
    assert(colgen);
    assert(templ);
    assert(!colgen->_template);

    templ->dvalue = colgen->rwall - colgen->lwall;
    
    enum methodtype methods = Rnd;
    for ( short i = 0; i < TotalMethods; ++i ){
        if ( colgen->method & ( methods << i ) ){
            //templ->fn = templ_fns[i];
            break;
        }
    }
    
    colgen->_template = templ;
}

void destroy_templ(t_templ *templ)
{
    assert(templ);
    free(templ);
}

void gen_type(t_colinfo *info, t_colgen *colgen)
{
    assert(info);
    assert(colgen);

    char letter[3] = {'r','u','s'}; //this needs to be made global

    short new_method = 0;
    enum methodtype methods = Rnd;
    for ( short i = 0; i < TotalMethods; ++i ){
        if ( strchr(info->option, letter[i]) ){
            new_method |= ( methods << i ) ;
        }
    }
    
    colgen->gentype = Template;
    if ( new_method & Rnd ){ //if cmmd is random
        if ( new_method & ( Unq | Scl ) ){ //and also unique or scalable
            colgen->gentype = List; //then it must be a list
        }
    } colgen->method = new_method;

    if ( info->file ){
        if ( colgen->gentype == List ){
            colgen->gentype = File | List;
        } 
        else if ( colgen->gentype == Template){
            colgen->gentype = File | Template;
        }
    }
}

t_colgen *init_colgen()
{
    t_colgen *new_colgen = malloc(sizeof(t_colgen));

    new_colgen->method = Undef; //-1
    new_colgen->gentype = Error; //-1
    new_colgen->_list = NULL;
    new_colgen->amt_row = DEFAULT_DBSIZE; 
    new_colgen->lwall = 0.0;
    new_colgen->rwall = DEFAULT_DBSIZE;
    new_colgen->delim = ',';
    new_colgen->decimals = 0;
    new_colgen->_linker = NULL;

    return new_colgen;
}

t_colgen *start_colgen(t_colinfo* info, t_colgen* colgen)
{
    if ( info->lwall ){
        colgen->lwall = strtod(info->lwall, NULL);
    }

    if ( info->rwall ){
        colgen->rwall = strtod(info->rwall, NULL);
    }

    if (( info->amount ) && ( atoi(info->amount) < DEFAULT_DBSIZE )){ 
        colgen->amt_row = atoi(info->amount);
    } 
    else if (( colgen->rwall - colgen->lwall ) < DEFAULT_DBSIZE ){
        colgen->amt_row = colgen->rwall - colgen->lwall;
    }

    if ( info->delim != '\0' ){ // if specified delim in info
        colgen->delim = info->delim; // col will input such delim
    }

    if ( info->decimals > 0){
        colgen->decimals = info->decimals;
    }

    gen_type(info, colgen);
    short gentype = colgen->gentype;
    if ( gentype & List ){
        start_arrlist(colgen); 
    }
    else if ( gentype & Template ){
        start_templ(colgen, init_templ());
    }
    
    return colgen;
}

void start_arrlist(t_colgen *colgen)
{
    assert(!colgen->_list);
    assert(colgen->amt_row);

    t_list **new_arrlist = malloc(colgen->amt_row * sizeof(t_list*));
    assert(new_arrlist);

    for ( int i = 0; i < colgen->amt_row; ++i ){
        new_arrlist[i] = init_list();
    }
    if ( colgen->gentype == List ){
        nums_to_arrlist(new_arrlist, colgen);
    }
    else if ( colgen->gentype == ( File | List ) ){
        FILE *f_read = fopen("content/nomes.txt", "r");
        file_to_arrlist(f_read, new_arrlist, colgen);
        fclose(f_read);
    }

    colgen->_list = new_arrlist;
}

t_colgen **start_arrcolgen(t_colinfo *info, int amt_cols)
{
    assert(info);

    t_colgen **new_arrcolgen = malloc(amt_cols * sizeof(t_colgen*));
    assert(new_arrcolgen);

    int j = 0; //iterate through colgen index
    int i = 0; //iterate through colinfo index
    while ( j < (amt_cols) ){
        new_arrcolgen[j] = start_colgen(info+i, init_colgen()); 

        if ( (info+i)->link ){
            int linker_index = j; //hold onto the linker index
            t_colinfo *ptr_link = (info+i)->link;
            while ( ptr_link ){
                ++j; 
                new_arrcolgen[j] = start_colgen(ptr_link, init_colgen());
                new_arrcolgen[j]->_linker = new_arrcolgen[linker_index];

                ptr_link = ptr_link->link;
            }
        } else {
            new_arrcolgen[j]->_linker = NULL;
        }
        
        if ( i < amt_cols ){
            ++i;        
        }
        ++j;
    }

    return new_arrcolgen;
}

void print_list(size_t amt_rows, t_list **list, short gentype)
{
    assert(list);
    
    for ( int i = 0; i < amt_rows; ++i ){
        if (( gentype == ( File | List )) && ( list[i]->svalue )){
            fprintf(stderr, "\t# %s\n", list[i]->svalue);
        }
        else if ( gentype == List ){
            fprintf(stderr, "\t# %f\n", list[i]->dvalue);
        }

        if ( list[i]->tree_size ){
            fprintf(stderr, "\t#%d tree_size: %ld", i+1, list[i]->tree_size);
        }
    } fputc('\n', stderr);
}

void print_templ(t_templ *templ)
{
    assert(templ);

    fprintf(stderr, "\tvalue: %f\n", templ->dvalue);
}

void print_arrcolgen(t_colgen **colgen, int amt_cols)
{
    assert(colgen);

    for ( int i = 0; i < amt_cols; ++i ){
        fprintf(stderr, "n#: %d\nmethod: %d\namt_row: %ld\nrange: %f-%f\ndelim: %c\ndecimals: %d\nlinker: %p\n", i+1, colgen[i]->method, colgen[i]->amt_row, colgen[i]->lwall, colgen[i]->rwall, colgen[i]->delim, colgen[i]->decimals, (void*)colgen[i]->_linker);
        
        if ( colgen[i]->gentype == Template){
            fprintf(stderr,"gentype: template\n");
            print_templ(colgen[i]->_template);
        } 
        else if (colgen[i]->gentype == ( File | Template )){
            fprintf(stderr,"gentype: file_template\n");
            print_templ(colgen[i]->_template);
        }
        else if ( colgen[i]->gentype == List ){
            fprintf(stderr, "gentype: list\n");
            print_list( colgen[i]->amt_row, colgen[i]->_list, colgen[i]->gentype );
        }
        else if ( colgen[i]->gentype == ( File | List )){
            fprintf(stderr, "gentype: file_list\n");
            print_list( colgen[i]->amt_row, colgen[i]->_list, colgen[i]->gentype );
        } else {
            _error(ERR_READ, "Nil gentype");
            exit(EXIT_FAILURE);
        }
    }
}

void destroy_colgen(t_colgen *colgen)
{
    short gentype = colgen->gentype;
    if ( gentype == Template || gentype == ( File | Template ) ){
        destroy_templ(colgen->_template);
    }
    else if ( gentype == List || gentype == ( File | List ) ){
        destroy_arrlist(colgen->gentype, colgen->_list, colgen->amt_row);
    }
    
    free(colgen);
}

void destroy_arrcolgen(t_colgen **arrcolgen, int amt_cols)
{
    assert(arrcolgen);

    for ( int i = 0; i < amt_cols; ++i ){
        destroy_colgen(arrcolgen[i]);
    } free (arrcolgen);
}
