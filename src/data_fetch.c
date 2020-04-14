#include "data_fetch.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <time.h>

//Initialize BLOCK in a HEAP, and assign it's address to an index in the HEAP,
//the maximum amount of BLOCKS that can be created is defined in TOTAL_BLOCKS
void initBLOCK(t_BLOCK *BLOCK, t_HEAP *HEAP)
{
    static size_t i = 0;

    if (i >= TOTAL_BLOCKS) {
        fprintf(stderr, "Reached maximum capacity (%d) for BLOCK's creation\n", TOTAL_BLOCKS);
        exit(1);
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
    size_t i, temp;

    temp = BLOCK->size;
    for ( i=first_i; i < BLOCK->size; ++i ){
        free(BLOCK->data[i]);
        --temp;
    }
    BLOCK->size = temp;
    
    if ( BLOCK->size == 0 )
        free(BLOCK->data);
}

//Read file and store each line as a string value of index i in the array
void fileToBLOCK(FILE* f_read, t_BLOCK* BLOCK)
{
    size_t i;
    char temp[STRLEN];

    i = 0;
    while( fgets(temp, STRLEN-1, f_read) ){
        BLOCK->data =(char**)realloc(BLOCK->data, sizeof(char*)*(i+1));
        if (badAlloc(BLOCK->data)) exit(1);
        BLOCK->data[i] = strndup(temp,strlen(temp)-1);
        if (badAlloc(BLOCK->data[i])) exit(1);
        ++i;
    }
    BLOCK->size = i;
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
void numsToHEAP(t_BLOCK *BLOCK, int first, int last, size_t amount, size_t digits)
{
    unsigned int pad;
    
    if ( last < first ){
        fprintf(stderr, "Last index is lesser than first\n");
        exit(1);
    } else if ( amount > (last-first) ) {
        amount = (last-first);
        pad = 1;
    } else if ( amount > 0 ) {
        pad = (last-first)/amount;
    } else {
        fprintf(stderr, "Invalid elements amount\n");
        exit(1);
    }

    BLOCK->data = (char**)malloc(sizeof(char*)*amount);
    if (badAlloc(BLOCK->data)) exit(1);

    BLOCK->size = amount;

    last -= pad;
    while ( amount-- > 0 ){
        BLOCK->data[amount] = (char*)malloc(sizeof(char)*digits);
        if (badAlloc(BLOCK->data[amount])) exit(1);

        snprintf(BLOCK->data[amount],digits-1,"%d",last+rand()%pad);
        last -= pad;
    }
}

void numsToSTACK(int first, int last, size_t amount, size_t digits, char STACK[][digits])
{
    unsigned int pad;
    
    if ( last < first ){
        fprintf(stderr, "Last index is lesser than first\n");
        exit(1);
    } else if ( amount > (last-first) ) {
        amount = (last-first);
        pad = 1;
    } else if ( amount > 0 ) {
        pad = (last-first)/amount;
    } else {
        fprintf(stderr, "Invalid elements amount\n");
        exit(1);
    }

    last -= pad;
    while ( amount-- > 0 ){
        snprintf(STACK[amount],digits-1,"%d",last+rand()%pad);
        last -= pad;
    }
}

int badAlloc(void *ptr)
{
    if (ptr == NULL){
        fprintf(stderr,"Couldn't malloc\n");
        return 1;
    }
    return 0;
}
