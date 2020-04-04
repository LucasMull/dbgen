#include "generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <time.h>

/*Initialize BLOCK in a HEAP, and assign it's address to an index in the HEAP,
the maximum amount of BLOCKS that can be created is defined in TOTAL_BLOCKS*/
void initBLOCK(t_BLOCK *BLOCK, t_HEAP *HEAP, int digits)
{
    static size_t i = 0;

    if (HEAP->size >= TOTAL_BLOCKS) {
        fprintf(stderr, "Reached maximum capacity (%d) for memory creation\n", TOTAL_BLOCKS);
        exit(1);
    }
    BLOCK->size = 0;
    BLOCK->digits = digits;
    
    HEAP->size = i;
    HEAP->addr[i++] = BLOCK;
}
/*Free pointers allocated to the particular block
  first_i = n - means all pointers from index n up to BLOCK->size will be free'd,*/
void freeBLOCK(t_BLOCK *BLOCK, size_t first_i)
{
    size_t i, temp;

    if ( BLOCK->data == NULL ){
        fprintf(stderr,"Block already emptied\n");
        return;
    }

    temp = BLOCK->size; 
    for ( i=first_i; i < BLOCK->size; ++i ){
        free(BLOCK->data[i]);
        --temp;
    }
    BLOCK->size = temp;

    if ( BLOCK->size == 0 )
        free(BLOCK->data);
}

/*Read file and store each line as an element to the array of strings*/
void fileToBLOCK(FILE* f_read, t_BLOCK* BLOCK)
{
    while( !feof(f_read) ){
        BLOCK->data = (char**)realloc(BLOCK->data, sizeof(char*)*(BLOCK->size+1));
        if ( BLOCK->data == NULL ) {
            fprintf(stderr,"Couldn't malloc\n");
            exit(1);
        }
        BLOCK->data[BLOCK->size] = (char*)malloc(sizeof(char)*STRLENGTH);
        if ( BLOCK->data[BLOCK->size] == NULL ) {
            fprintf(stderr,"Couldn't malloc\n");
            exit(1);
        }
        fgets(BLOCK->data[BLOCK->size], STRLENGTH-1, f_read);
        BLOCK->data[BLOCK->size][strlen(BLOCK->data[BLOCK->size])-1] = '\0';
        ++BLOCK->size;
    }
}

/*Store unique random values from a given scope, at a specified amount of elements*/
void randNumToBLOCK(t_BLOCK *BLOCK, int start, int scope, size_t amount)
{
    size_t i;
    scope -= start; 
    

    BLOCK->data = (char**)malloc(sizeof(char*)*(1+scope));
    for ( i=0; i<=scope; ++i ){
        BLOCK->data[i] = (char*)malloc(sizeof(char)*BLOCK->digits);
        snprintf(BLOCK->data[i],BLOCK->digits-1,"%d",start++);
        ++BLOCK->size;
    }


    if ( amount > scope ){
        fprintf(stderr,"Amount is bigger than scope\n");
        exit(1);
    } else if ( amount == 0 ) {
        amount = scope;
    }

    for ( i=0; i<=amount; ++i ) //shuffle
        stringSwap(BLOCK->data+i, BLOCK->data+rand()%scope);

    if ( amount != scope )
        freeBLOCK(BLOCK, amount);
}
