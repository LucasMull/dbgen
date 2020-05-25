#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "src/data_fetch.h"
#include "parser.h"

#define TEMPLATE 0
#define LIST 1

t_list *start_list(t_colinfo*);
void destroy_list(t_list*);

t_templ *start_templ(t_colinfo*);
void destroy_templ(t_templ*);

int gen_type(t_colinfo*);
t_colgen *start_colgen(t_colinfo*, int);
void destroy_colgen(t_colgen*, int);

void print_list(t_list*);
void print_templ(t_templ*);
void print_colgen(t_colgen*, int);

int main(int argc, char *argv[])
{
    short amt_cols;

    t_colinfo *info = parser(argc, argv, &amt_cols);
    t_colgen *colgen = start_colgen(info, amt_cols);

    print_colinfo(info,amt_cols);
    clean_colinfo(info,amt_cols);

    print_colgen(colgen, amt_cols);
    destroy_colgen(colgen, amt_cols);    
    return 0;
}

t_list *start_list(t_colinfo* info)
{
    t_list *new_list = malloc(sizeof(t_list));
    assert(new_list);

    new_list->data = NULL;
    new_list->size = 0;

    return new_list;
}

void destroy_list(t_list* list)
{
    assert(list);

    if (list->data){
        free(list->data);
    } free(list);
}

t_templ *start_templ(t_colinfo* info)
{
    t_templ *new_templ = malloc(sizeof(t_templ));
    assert(new_templ);

    if (info->lwall && info->rwall){
        new_templ->value = strtod(info->rwall,NULL) - strtod(info->lwall, NULL);
    } else {
        new_templ->value = 0;
    }

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

    if ( info->link ){
        return LIST;
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
            return LIST; //then it must be a list
        }
    }

    return TEMPLATE; //otherwise a template will do
}

t_colgen *start_colgen(t_colinfo *info, int amt_cols)
{
    assert(info);

    t_colgen *new_colgen = malloc( amt_cols * sizeof(t_colgen) );

    int i = 0;
    while ( i < amt_cols ){
        if ( gen_type(info+i) == TEMPLATE ){
            (new_colgen+i)->_template = start_templ(info+i);
            (new_colgen+i)->gentype = TEMPLATE;
        } else /* if is LIST */{
            (new_colgen+i)->_list = start_list(info+i);
            (new_colgen+i)->gentype = LIST;
        }

        if( (info+i)->lwall && (info+i)->rwall ){
            (new_colgen+i)->lwall = strtod((info+i)->lwall, NULL);
            (new_colgen+i)->rwall = strtod((info+i)->rwall, NULL);
        } else {
            (new_colgen+i)->lwall = 0.0;
            (new_colgen+i)->rwall = 0.0;
        }
       
        if ((info+i)->amount){ 
            (new_colgen+i)->amount = atoi((info+i)->amount);
        } else {
            (new_colgen+i)->amount = 0;
        }

        if ((info+i)->delim){
            (new_colgen+i)->delim = (info+i)->delim;
        } else {
            (new_colgen+i)->delim = ',';
        }
        (new_colgen+i)->method = strdup((info+i)->option);
        ++i;        
    }

    return new_colgen;
}

void print_list(t_list *list)
{
    assert(list);

    fprintf(stderr, "\tsize: %ld\n", list->size);
}

void print_templ(t_templ *templ)
{
    assert(templ);

    fprintf(stderr, "\tvalue: %f\n", templ->value);
}

void print_colgen(t_colgen *colgen, int amt_cols)
{
    assert(colgen);

    int i = 0;
    while ( i < amt_cols ){
        fprintf(stderr, "n#: %d\nmethod: %s\namount: %ld\nrange: %f-%f\ndelim: %c\n", i+1, (colgen+i)->method, (colgen+i)->amount, (colgen+i)->lwall, (colgen+i)->rwall, (colgen+i)->delim);
        if ((colgen+i)->gentype == TEMPLATE){
            fprintf(stderr,"gentype: template\n");
            print_templ((colgen+i)->_template);
        } else {
            fprintf(stderr, "gentype: list\n");
            print_list((colgen+i)->_list);
        }
        ++i;
    }
}

void destroy_colgen(t_colgen *colgen, int amt_cols)
{
    assert(colgen);

    int i = 0;
    while ( i < amt_cols ){
        if ( (colgen+i)->method ){
            free((colgen+i)->method);
        }
        if ((colgen+i)->gentype == TEMPLATE){
            destroy_templ((colgen+i)->_template);
        } else {
            destroy_list((colgen+i)->_list);
        }
        ++i;
    } free (colgen);
}
