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

    TotalGens=3,
};

enum methodtype { 
    Rnd=1, 
    Unq=2, 
    Scl=4, 

    TotalMethods=3,
    Undef=0
};


/*
 * Read file and store each line as a string value of index i in the array
 * Needs rework.
 */
void file_to_arrlist(FILE* f_read, t_list** arrlist, t_colgen* colgen)
{
    size_t ln_total = colgen->amt_row; 
    const int LEN = 50;

    char ptr_str[LEN];
    size_t i = 0;
    while ( (fgets(ptr_str, LEN-1, f_read)) && (i < ln_total) ){
        arrlist[i]->svalue = strndup(ptr_str, strlen(ptr_str)-1);
        assert(arrlist[i]->svalue);

        ++i;
    }
}

void list_swap(t_list** ptr1, t_list** ptr2)
{
    assert( ptr1 && ptr1 );

    t_list *temp_ptr = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = temp_ptr;
}

//Makes use of str_swap(), to swap string pointers randomly
void shuffle_arrlist(t_list **arrlist, t_colgen* colgen)
{
    for ( size_t i = 0 ; i < colgen->amt_row ; ++i ){ //shuffle
        int rnd = rand() % colgen->amt_row;
        list_swap( arrlist + i, arrlist + rnd );
    }
}

void set_scalable(void* this)
{
    t_colgen* colgen = (t_colgen*)this;

    double first = colgen->lwall;
    double last = colgen->rwall;
    size_t amount = colgen->amt_row;

    if ( colgen->_template->dvalue < last ){
        unsigned int pad = (last - first) / amount;
        colgen->_template->dvalue +=  rand() % pad;
    } else {
        colgen->_template->dvalue = last;
    }
}

void set_incremental(void* this)
{
    t_colgen* colgen = (t_colgen*)this;
    double last = colgen->rwall;

    if ( colgen->_template->dvalue < last ){
        ++colgen->_template->dvalue;
    }
}

void set_random(void* this)
{
    t_colgen* colgen = (t_colgen*)this;
    double first = colgen->lwall;
    double last = colgen->rwall;
    
    size_t rnd = rand() % (size_t)(last - first);
    colgen->_template->dvalue = first + rnd; 
}

void numsetter_templ(t_templ* templ, t_colgen* colgen)
{
    if ( colgen->method & Rnd )
        templ->fn = &set_random;
    else if ( colgen->method & Scl )
        templ->fn = &set_scalable;
    else
        templ->fn = &set_incremental;
}
/*
void filesetter_templ(t_templ* templ, t_colgen* colgen)
{

}
*/

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
void gen_scalable(t_list** arrlist, t_colgen* colgen)
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

void gen_incremental(t_list** arrlist, t_colgen* colgen)
{
    double first = colgen->lwall;
    size_t amount = colgen->amt_row;

    for ( size_t i = 0; i < amount; ++i )
        arrlist[i]->dvalue = first + i;
}

void nums_to_arrlist(t_list** arrlist, t_colgen* colgen)
{
        if ( colgen->method & Scl ){
            gen_scalable(arrlist,colgen);
        } else {
            gen_incremental(arrlist,colgen);
        }

        if ( colgen->method & Rnd )
            shuffle_arrlist(arrlist, colgen);
}
