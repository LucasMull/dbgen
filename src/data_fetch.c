#include "data_fetch.h"

#ifndef FILE_H
#define FILE_H
#include "directives.h"
#endif

//Initialize block in a heap, and assign it's address to an index in the heap,
//the maximum amount of blockS that can be created is defined in TOTAL_blockS
void init_h(t_hblock *block, t_heap *heap)
{
    static size_t i = 0;
    size_t temp_i = 0;

    assert (i < TOTAL_BLOCKS);
    while(temp_i < i){ //check if block has already been initialized
        assert( heap->addr[temp_i++] != block );
    }

    block->size = 0;
    block->data = NULL;
    
    heap->size = i;
    heap->addr[i++] = block;
}

//Free pointers allocated to the particular block
//EX: first_i = j , means all pointers from index j up to block->size will be free'd
void free_h(t_hblock *block, size_t first_i)
{
    size_t i, temp_i;

    temp_i = block->size;
    for ( i = first_i ; i < block->size ; ++i ){
        free(block->data[i]);
        --temp_i;
    } block->size = temp_i;
    
    if ( block->size == 0 ){
        free(block->data);
    }
}

//Read file and store each line as a string value of index i in the array
void file_to_h(FILE* f_read, t_hblock* block, size_t ln_total)
{
    size_t i;
    char ptr_str[STRLEN];

    if (!ln_total){
        ln_total = INT_MAX;
    }

    i = 0;
    while ( (fgets(ptr_str, STRLEN-1, f_read)) && (i < ln_total) ){
        block->data = (char**)realloc(block->data, sizeof(char*) * (i + 1));
        assert(block->data);

        block->data[i] = strndup(ptr_str, strlen(ptr_str)-1);
        assert(block->data[i]);

        ++i;
    } block->size = i;
}

/*
Stores numbers proportionally to the scope (last - first) provided, by making use of a 'padding' tool
ex: 1000 to 2000; amount of elements wanted = 5

pad = scope / amount , which translates to 1000/5 = 200
pad = 200 , means it will decrement from 2000 to 1000, 
    lowering the value of the last element by 200 each run (last -= pad), until last <= first

rand() used in each padding, from a range of last to last+pad makes sure each random number 
    is unique, but also lower than the previous one
*/
void nums_to_h(t_hblock *block, long int first, long int last, size_t amount, size_t length)
{
    unsigned int pad;
    
    assert( last >= first );
    assert( (amount > 0) && (amount <= last-first) );

    pad = (last - first) / amount;

    block->data = (char**)malloc(sizeof(char*) * amount);
    assert(block->data);
    block->size = amount;

    last -= pad;
    while ( amount-- > 0 ){
        block->data[amount] = (char*)malloc(sizeof(char) * length);
        assert(block->data[amount]);
        snprintf( block->data[amount] , length-1 , "%ld" , last+(rand()%pad) );

        last -= pad;
    }
}

<<<<<<< HEAD
void nums_to_s(long int first, long int last, size_t amount, size_t length, t_sblock block[][length])
=======
void nums_to_s(long int first, long int last, size_t amount, size_t digits, t_sblock block[][digits])
>>>>>>> 9b07480291894254e98e87299de0b47ad8af5037
{
    unsigned int pad;
    
    assert( last >= first );
    assert( (amount > 0) && (amount <= last - first) );

    pad = (last - first) / amount;

    last -= pad;
    while ( amount-- > 0 ){
<<<<<<< HEAD
        snprintf( block[amount] , length-1 , "%ld" , last+(rand()%pad) );
=======
        snprintf( block[amount] , digits-1 , "%ld" , last+(rand()%pad) );
>>>>>>> 9b07480291894254e98e87299de0b47ad8af5037
        last -= pad;
    }
}
