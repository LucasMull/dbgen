#include "generator.h"
#include "columns.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//randomly join string elements into a single one
void joinRandomDATA(char joint[], const t_BLOCK *BLOCK1, const t_BLOCK *BLOCK2){
    char *temp_surnam = BLOCK2->data[rand()%BLOCK1->size];

    if ( strlen(temp_surnam) > 3 ){
        snprintf(joint, STRLENGTH-1, "%s %s", BLOCK1->data[rand()%BLOCK1->size],
                                                 temp_surnam);
    } else {
        snprintf(joint, STRLENGTH-1, "%s %s %s", BLOCK1->data[rand()%BLOCK1->size],
                                                    temp_surnam,
                                                    BLOCK2->data[rand()%BLOCK1->size]);
    }
}

//fetch for DATA linearly under the condition that its elements count is greater than or equal to DBSIZE
void fetchLinearDATA(char dest[], t_BLOCK *BLOCK, size_t i)
{
    if ( BLOCK->size < DBSIZE ){
        fprintf(stderr,"Couldn't fetch linearly, BLOCK elements is lesser than DBSIZE");
        exit(1);
    }
    snprintf(dest,BLOCK->digits-1,"%s",BLOCK->data[i]);
}

//fetch for DATA with random index access
void fetchRandomDATA(char dest[], t_BLOCK *BLOCK)
{
    snprintf(dest,BLOCK->digits-1,"%s",BLOCK->data[rand()%BLOCK->size]);
}

//Need update, not modularized enough, basically compiles every information and print output into file
void getUsers(FILE* f_out, t_BLOCK *addr[], t_DB *database)
{
    size_t i;
    
    shuffleArray(addr[1]);
    shuffleArray(addr[2]);
    for ( i=0; i<DBSIZE; ++i ){
        joinRandomDATA(database->user[i].fullname,addr[0],addr[1]);
        fetchLinearDATA(database->user[i].ID, addr[2], i);
        fetchRandomDATA(database->user[i].agency, addr[3]);

        fprintf(f_out,"%s,%s,%s\n", database->user[i].ID, 
                                    database->user[i].agency,
                                    database->user[i].fullname); 
    }
}

//Pretty straightforward, swaps two string pointers
void stringSwap(char **str1_ptr, char **str2_ptr)
{
    char *temp = *str1_ptr;
    *str1_ptr = *str2_ptr;
    *str2_ptr = temp;
}

//Makes use of stringSwap(), to swap string pointers randomly
void shuffleArray(t_BLOCK *BLOCK)
{
    size_t i;

    for ( i=0; i<BLOCK->size; ++i ) //shuffle
        stringSwap(BLOCK->data+i, BLOCK->data+rand()%BLOCK->size);
}
