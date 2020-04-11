#include "data_fetch.h"
#include "manipulate_data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//randomly pick data index and returns its address
char *pickRandom(t_BLOCK *BLOCK){
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
void joinRandom(char joint[], const t_BLOCK *BLOCK1, const t_BLOCK *BLOCK2){
    snprintf(joint,STRLENGTH-1,"%s %s",BLOCK1->data[rand()%BLOCK1->size],
                                       BLOCK2->data[rand()%BLOCK2->size]);
}

//fetch for DATA linearly under the condition that its elements count is greater than or equal to DBSIZE
void fetchLinear(char dest[], t_BLOCK *BLOCK, size_t i)
{
    if ( BLOCK->size < DBSIZE ){
        fprintf(stderr,"Couldn't fetch linearly, BLOCK elements is lesser than DBSIZE");
        exit(1);
    }
    snprintf(dest,BLOCK->digits-1,"%s",BLOCK->data[i]);
}

//fetch for DATA with random index access
void fetchRandom(char dest[], t_BLOCK *BLOCK){
    snprintf(dest,BLOCK->digits-1,"%s",BLOCK->data[rand()%BLOCK->size]);
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

//Initialize parent data, to be assigned children to
void initAssign(t_assign *parent, char *str)
{
    t_node *start, *end;

    if ( parent->start || parent->end ){
        fprintf(stderr, "t_assign can't be initialized, aborting\n");
        exit(1);
    }

    start = (t_node*)malloc(sizeof(t_node));
    end = (t_node*)malloc(sizeof(t_node));

    if ( !start || !end ){
        fprintf(stderr, "Couldn't malloc, aborting\n");
        exit(1);
    }
    parent->start = start;
    start->next = end;
    start->prev = NULL;
    parent->end = end;
    end->prev = start;
    end->next = NULL;

    parent->data = str;
    parent->size = 0;
}
//check potential children for it's uniqueness among already
//assigned parent's children. If unique, return children's data
//address, otherwise return NULL.

//I should implement a limiter on how many functions calls can be
//made before it gives up
char *uniqueChild(t_assign *parent, char *children)
{
    t_node *aux = parent->start->next;    

    if ( !parent->start || !parent->end ){
        fprintf(stderr, "Uninitialized t_assign, aborting\n");
        exit(1);
    }

    while ( aux != parent->end ){
        if (!strcmp(aux->data, children)) return NULL;
        aux = aux->next;
    }
    return children;
}

//assign children data to parent data
void assignChild(t_assign *parent, char *children)
{
    t_node *node;

    if ( !children ){
        fprintf(stderr,"NULL children,aborting\n");
        exit(1);
    }
    node = (t_node*)malloc(sizeof(t_node));
    if ( !node ){
       fprintf(stderr,"Couldn't malloc, aborting\n");
        exit(1);
    }
    node->prev = parent->start;
    node->next = parent->start->next;
    parent->start->next->prev = node;
    parent->start->next = node;
    
    node->data = children;
    ++parent->size;
}

//erases parent so that it can be reused
void eraseFamily(t_assign *parent)
{
    t_node *aux;

    aux = parent->start->next;    
    while (aux != NULL){
        free(aux->prev);
        aux = aux->next;
    }
    free(parent->end);

    parent->size = 0;
    parent->data = NULL;
}
