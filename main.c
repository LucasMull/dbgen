#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "src/data_fetch.h"
#include "src/parser.h"

#define DEFAULT_DBSIZE 10

enum type {Template, List};


t_list *start_list(t_colinfo*);
t_list **start_arrlist(t_colinfo*, int);
void destroy_list(t_list*);
void destroy_arrlist(t_list**, int);
void print_list(size_t, t_list**);

t_templ *start_templ(t_colinfo*);
void destroy_templ(t_templ*);
void print_templ(t_templ*);

int gen_type(t_colinfo*);

t_colgen *start_colgen(t_colinfo*);
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

t_list *start_list(t_colinfo* info)
{
    t_list *new_list = malloc(sizeof(t_list));
    assert(new_list);

    new_list->svalue = NULL;

    new_list->root = NULL;

    new_list->tree_size = 0;

    return new_list;
}

void destroy_arrlist(t_list** list, int amt_row)
{
    assert(list);

        for ( int i = 0; i < amt_row; ++i ){
            destroy_list(list[i]);
        } free(list);
}

void destroy_list(t_list* list)
{
    assert(list);

    if ( list->svalue )
        free(list->svalue);
    //implement free traversing tree
    if ( list->root )
        free(list->root);
     free(list);
}


t_templ *start_templ(t_colinfo* info)
{
    t_templ *new_templ = malloc(sizeof(t_templ));
    assert(new_templ);

    if ( (info) && (info->lwall && info->rwall) ){
        new_templ->value = strtod(info->rwall,NULL) - strtod(info->lwall, NULL);
    } else {
        new_templ->value = 0;
    }
    new_templ->fn = NULL;

    return new_templ;
}

void destroy_templ(t_templ *templ)
{
    assert(templ);
    free(templ);
}

int gen_type(t_colinfo *info)
{
    assert(info);

    if ( info->file ){
        return List;
    } else if ( info->link ){
        return Template;
    }
    
    int cmmd = 0; //common denominator
    char letter[] = {"rus"};
    int prime[3] = {2, 3, 5};

    short i = 0;
    while ( letter[i] ){
        if ( strchr(info->option, letter[i]) ){
            cmmd += prime[i];
        }
        ++i;
    }
    
    if ( !(cmmd % 2) ){ //if cmmd is random
        if ( !(cmmd % 3) || !(cmmd % 5) ){ //and also unique or scalable
            return List; //then it must be a list
        }
    }

    return Template; //otherwise a template will do
}

t_colgen *start_colgen(t_colinfo *info)
{
    t_colgen *new_colgen = malloc(sizeof(t_colgen));

    if ( info->lwall ){
        new_colgen->lwall = strtod(info->lwall, NULL);
    } else {
        new_colgen->lwall = 0.0;
    }
    if ( info->rwall ){
        new_colgen->rwall = strtod(info->rwall, NULL);
    } else {
        new_colgen->rwall = DEFAULT_DBSIZE;
    }
  
    new_colgen->amt_row = DEFAULT_DBSIZE; 
    if (( info->amount ) && ( atoi(info->amount) < DEFAULT_DBSIZE )){ 
        new_colgen->amt_row = atoi(info->amount);
    } else if (( new_colgen->rwall - new_colgen->lwall ) < DEFAULT_DBSIZE ){
        new_colgen->amt_row = new_colgen->rwall - new_colgen->lwall;
    }

    if ( gen_type(info) == List ){
        new_colgen->gentype = List;
        new_colgen->_list = start_arrlist(info, new_colgen->amt_row); 
    } else {
        new_colgen->gentype = Template;
        new_colgen->_template = start_templ(info);
    }

    if ( info->delim ){ // if specified delim in info
        new_colgen->delim = info->delim; // col will input such delim
    } else { // otherwise will pick default delimiter
        new_colgen->delim = ',';
    }

    if ( info->decimals ){
        new_colgen->decimals = info->decimals;
    } else {
        new_colgen->decimals = 0;
    }

    if ( info->file ){ // if info contains file
        new_colgen->method = strdup("f"); // must follow file method
    } else if ( strlen(info->option) > 0 ){ // otherwise get from info
        new_colgen->method = strdup(info->option);
    } else { // if the empty is above then no strict method to follow
        new_colgen->method = strdup("!");
    }
    new_colgen->_linker = NULL;

    return new_colgen;
}

t_list **start_arrlist(t_colinfo *info, int amt_row)
{
    t_list **new_list = malloc(amt_row * sizeof(t_list*));

    for ( int i = 0; i < amt_row; ++i ){
        new_list[i] = start_list(info);
    }

    return new_list;
}

t_colgen **start_arrcolgen(t_colinfo *info, int amt_cols)
{
    assert(info);

    t_colgen **new_colgen = malloc(amt_cols * sizeof(t_colgen*));
    assert(new_colgen);

    int j = 0; //iterate through colgen index
    int i = 0; //iterate through colinfo index
    while ( j < (amt_cols) ){
        new_colgen[j] = start_colgen(info+i); 

        if ( (info+i)->link ){
            int linker_index = j; //hold onto the linker index
            t_colinfo *ptr_link = (info+i)->link;
            while ( ptr_link ){
                ++j; 
                new_colgen[j] = start_colgen(ptr_link);
                new_colgen[j]->_linker = new_colgen[linker_index];

                ptr_link = ptr_link->link;
            }
        } else {
            new_colgen[j]->_linker = NULL;
        }
        
        if ( i < amt_cols ){
            ++i;        
        }
        ++j;
    }

    return new_colgen;
}

void print_list(size_t amt_rows, t_list **list)
{
    assert(list);
    
    for ( int i = 0; i < amt_rows; ++i ){
        if ( list[i]->tree_size ){
            fprintf(stderr, "\t#%d tree_size: %ld", i+1, list[i]->tree_size);
        }
    } fputc('\n', stderr);
}

void print_templ(t_templ *templ)
{
    assert(templ);

    fprintf(stderr, "\tvalue: %f\n", templ->value);
}

void print_arrcolgen(t_colgen **colgen, int amt_cols)
{
    assert(colgen);

    for ( int i = 0; i < amt_cols; ++i ){
        fprintf(stderr, "n#: %d\nmethod: %s\namt_row: %ld\nrange: %f-%f\ndelim: %c\ndecimals: %d\nlinker: %p\n", i+1, colgen[i]->method, colgen[i]->amt_row, colgen[i]->lwall, colgen[i]->rwall, colgen[i]->delim, colgen[i]->decimals, (void*)colgen[i]->_linker);

        if ( colgen[i]->gentype == Template ){
            fprintf(stderr,"gentype: template\n");
            print_templ(colgen[i]->_template);
        } else {
            fprintf(stderr, "gentype: list\n");
            print_list( colgen[i]->amt_row, colgen[i]->_list );
        }
    }
}

void destroy_colgen(t_colgen *colgen)
{
    if ( colgen->method ){
        free(colgen->method);
    }

    if ( colgen->gentype == Template ){
        destroy_templ(colgen->_template);
    } else if ( colgen->gentype == List ){
        destroy_arrlist(colgen->_list, colgen->amt_row);
    }

    free(colgen);
}

void destroy_arrcolgen(t_colgen **colgen, int amt_cols)
{
    assert(colgen);

    for ( int i = 0; i < amt_cols; ++i ){
        destroy_colgen(colgen[i]);
    } free (colgen);
}
