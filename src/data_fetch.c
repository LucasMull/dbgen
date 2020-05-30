#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include "data_fetch.h"

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


/*
 * Read file and store each line as a string value of index i in the array
 */
void file_to_arrlist(FILE* f_read, t_list** arrlist, t_colgen* colgen)
{
    size_t ln_total = colgen->amt_row; 
    const short LEN = 25;

    char ptr_str[LEN];
    size_t i = 0;
    while ( (fgets(ptr_str, LEN-1, f_read)) && (i < ln_total) ){
        arrlist[i]->svalue = strndup(ptr_str, strlen(ptr_str)-1);
        assert(arrlist[i]->svalue);

        ++i;
    }
}

void nums_to_arrlist(t_list** arrlist, t_colgen* colgen)
{
    if ( colgen->method != Undef ) {
        if ( colgen->method & Rnd ){
            gen_scl(arrlist,colgen);
        }
        if ( colgen->method & Unq ){
            gen_scl(arrlist,colgen);
        }
        if ( colgen->method & Scl ){
            gen_scl(arrlist,colgen);
        }
    } else { 
            gen_scl(arrlist,colgen);
    }
}


/*
 * Stores numbers proportionally to the scope (last - first) provided, by making use of a 'padding' tool
 * ex: 1000 to 2000; amount of elements wanted = 5
 *
 * pad = scope / amount , which translates to 1000/5 = 200
 * pad = 200 , means it will decrement from 2000 to 1000, 
 * lowering the value of the last element by 200 each run (last -= pad), until last <= first
 *
 * rand() used in each padding, from a range of last to last+pad makes sure each random number 
 * is unique, but also lower than the previous one
 */
void gen_scl ( t_list** arrlist, t_colgen* colgen )
{
    double last = colgen->rwall;
    double first = colgen->lwall;
    size_t amount = colgen->amt_row;

    unsigned int pad = (last - first) / amount;

    last -= pad;
    while ( amount-- ){
        arrlist[amount]->dvalue = last + (rand() % pad);
        last -= pad;
    }
}

