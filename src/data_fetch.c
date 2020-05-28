#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include "data_fetch.h"

/*
//Initialize block in a heap, and assign it's address to an index in the heap,
//the maximum amount of blockS that can be created is defined in TOTAL_BLOCKS
void init_list(void *ptr)
{
    t_list *list = (t_list*)ptr;
    list->amount = 0;
    list->size = 0;
    list->data = NULL;
}

void init_templ(void *ptr)
{
    t_templ *templ = (t_templ*)ptr;
    templ->lwall = 0;
    templ->rwall = DBSIZE;
    templ->value = 0;
}

//Free pointers allocated to the particular block
//EX: first_i = j , means all pointers from index j up to block->size will be free'd
int free_list(t_list *list, size_t first_i)
{
    size_t temp_i = list->size;
    for ( size_t i = first_i ; i < list->size ; ++i ){
        free(list->data[i]);
        --temp_i;
    } list->size = temp_i;
    
    if ( list->size == 0 ){
        free(list->data);
        return 0;
    }
    return list->size;
}

//Read file and store each line as a string value of index i in the array
void file_to_list(FILE* f_read, t_list* list, size_t ln_total)
{
    if (!ln_total){
        ln_total = INT_MAX;
    }

    char ptr_str[STRLEN];
    size_t i = 0;
    while ( (fgets(ptr_str, STRLEN-1, f_read)) && (i < ln_total) ){

        list->data = (char**)realloc(list->data, sizeof(char*) * (i + 1));
        assert(list->data);

        list->data[i] = strndup(ptr_str, strlen(ptr_str)-1);
        assert(list->data[i]);

        ++i;
    } list->size = i;
}
*/
/*
Stores numbers proportionally to the scope (last - first) provided, by making use of a 'padding' tool
ex: 1000 to 2000; amount of elements wanted = 5

pad = scope / amount , which translates to 1000/5 = 200
pad = 200 , means it will decrement from 2000 to 1000, 
    lowering the value of the last element by 200 each run (last -= pad), until last <= first

rand() used in each padding, from a range of last to last+pad makes sure each random number 
    is unique, but also lower than the previous one
*/
/*
void nums_to_list(t_list *list, long int first, long int last, size_t amount, size_t length)
{
    assert( last >= first );
    assert( (amount > 0) && (amount <= last-first) );

    list->data = (char**)malloc(sizeof(char*) * amount);
    assert(list->data);
    list->size = amount;

    unsigned int pad = (last - first) / amount;
    last -= pad;
    while ( amount-- > 0 ){
        list->data[amount] = (char*)malloc(sizeof(char) * length);
        assert(list->data[amount]);
        snprintf( list->data[amount] , length-1 , "%ld" , last+(rand()%pad) );

        last -= pad;
    }
}
*/
