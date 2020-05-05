#include "data_fetch.h"

#ifndef FILE_H
#define FILE_H
#include "directives.h"
#endif

//Initialize BLOCK in a HEAP, and assign it's address to an index in the HEAP,
//the maximum amount of BLOCKS that can be created is defined in TOTAL_BLOCKS
void initBLOCK(t_BLOCK *BLOCK, t_HEAP *HEAP)
{
    static size_t i = 0;
    size_t temp_i = 0;

    assert (i < TOTAL_BLOCKS);
    while(temp_i < i){ //check if BLOCK has already been initialized
        assert( HEAP->addr[temp_i++] != BLOCK );
    }

    BLOCK->size = 0;
    BLOCK->data = NULL;
    
    HEAP->size = i;
    HEAP->addr[i++] = BLOCK;
}

//Free pointers allocated to the particular block
//EX: first_i = j , means all pointers from index j up to BLOCK->size will be free'd
void freeBLOCK(t_BLOCK *BLOCK, size_t first_i)
{
    size_t i, temp_i;

    temp_i = BLOCK->size;
    for ( i = first_i ; i < BLOCK->size ; ++i ){
        free(BLOCK->data[i]);
        --temp_i;
    } BLOCK->size = temp_i;
    
    if ( BLOCK->size == 0 ){
        free(BLOCK->data);
    }
}

//Read file and store each line as a string value of index i in the array
void fileToBLOCK(FILE* f_read, t_BLOCK* BLOCK, size_t lntotal)
{
    size_t i;
    char temp[STRLEN];

    if (!lntotal){
        lntotal = INT_MAX;
    }

    i = 0;
    while ( (fgets(temp, STRLEN-1, f_read)) && (i < lntotal) ){
        BLOCK->data = (char**)realloc(BLOCK->data, sizeof(char*) * (i + 1));
        assert(BLOCK->data);

        BLOCK->data[i] = strndup(temp, strlen(temp)-1);
        assert(BLOCK->data[i]);

        ++i;
    } BLOCK->size = i;
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
void numsToHEAP(t_BLOCK *BLOCK, long int first, long int last, size_t amount, size_t digits)
{
    unsigned int pad;
    
    assert( last >= first );
    assert( (amount > 0) && (amount <= last-first) );

    pad = (last - first) / amount;

    BLOCK->data = (char**)malloc(sizeof(char*) * amount);
    assert(BLOCK->data);
    BLOCK->size = amount;

    last -= pad;
    while ( amount-- > 0 ){
        BLOCK->data[amount] = (char*)malloc(sizeof(char) * digits);
        assert(BLOCK->data[amount]);
        snprintf( BLOCK->data[amount] , digits-1 , "%ld" , last+(rand()%pad) );

        last -= pad;
    }
}

void numsToSTACK(long int first, long int last, size_t amount, size_t digits, char STACK[][digits])
{
    unsigned int pad;
    
    assert( last >= first );
    assert( (amount > 0) && (amount <= last - first) );

    pad = (last - first) / amount;

    last -= pad;
    while ( amount-- > 0 ){
        snprintf( STACK[amount] , digits-1 , "%ld" , last+(rand()%pad) );
        last -= pad;
    }
}
