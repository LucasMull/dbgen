#include "generator.h"
#include "columns.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//randomly pick data index and returns its address
char *pickRandomDATA(t_BLOCK *BLOCK){
    return BLOCK->data[rand()%BLOCK->size];
}

//concatenate two selected strings into a single one
void joinStrings(char joint[], char str1[], char str2[]){
    if (str1 == joint) {
        strcat(joint, " ");
        strcat(joint, str2);
    } else if (str2 == joint) {
        strcat(joint, " ");
        strcat(joint, str1);
    } else {
        snprintf(joint,STRLENGTH-1,"%s %s",str1,str2);
    }
}

//randomly pick string elements and concatenate into a single one (don't override existing ones)
void joinRandomDATA(char joint[], const t_BLOCK *BLOCK1, const t_BLOCK *BLOCK2){
    snprintf(joint,STRLENGTH-1,"%s %s",BLOCK1->data[rand()%BLOCK1->size],
                                       BLOCK2->data[rand()%BLOCK2->size]);
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
void fetchRandomDATA(char dest[], t_BLOCK *BLOCK){
    snprintf(dest,BLOCK->digits-1,"%s",BLOCK->data[rand()%BLOCK->size]);
}

//Need update, not modularized enough, basically compiles every information and print output into file
void getUsers(FILE* f_out, t_BLOCK *addr[], t_DB *database)
{
    size_t i;
    char *str1, *str2;
    
    shuffleArray(addr[1]);
    shuffleArray(addr[2]);
    for ( i=0; i<DBSIZE; ++i ){
        str1 = pickRandomDATA(addr[0]);
        str2 = pickRandomDATA(addr[1]);
        joinStrings(database->user[i].fullname,str1,str2);
        if (strlen(str2) <= 3){
            str2 = pickRandomDATA(addr[1]);
            joinStrings(database->user[i].fullname,database->user[i].fullname,str2);
        }
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
