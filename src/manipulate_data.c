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

//a better option for this function would be if instead
//of concatenating two strings into a new one, simply have
//joint point to address of str1 and str2, like char *joint[2]
void joinStrings(char joint[], char str1[], char str2[]){
    if (str1 == joint) {
        strncat(joint, " ", STRLENGTH);
        strncat(joint, str2, STRLENGTH);
    } else if (str2 == joint) {
        strncat(joint, " ", STRLENGTH);
        strncat(joint, str1, STRLENGTH);
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

    parent->start->data = NULL;
    parent->end->data = NULL;
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
    t_node *aux;   
    unsigned comp_start=0;
    unsigned comp_end=1;
    
    if ( parent->start->next != parent->end ){ 
        comp_start = (unsigned)strcmp(children,parent->start->next->data);
        comp_end = (unsigned)strcmp(children,parent->end->prev->data);
    }

    if ( !parent->start || !parent->end ){
        fprintf(stderr, "Uninitialized t_assign variable, aborting\n");
        exit(1);
    }

    if ( comp_start < comp_end ){
        aux = parent->end->prev;
        while ( aux != parent->start ){
            if (!strcmp(children, aux->data)) return NULL;
            aux = aux->prev;
        }
    } else {
        aux = parent->start->next;
        while ( aux != parent->end ){
            if (!strcmp(children, aux->data)) return NULL;
            aux = aux->next;
        }

    }
    return children;
}

//assign children data to parent data, and stores it in linked list in crescent
//order, for easy access
void assignChild(t_assign *parent, char *children)
{
    t_node *node, *aux;

    if ( !children ){
        fprintf(stderr,"NULL children,aborting\n");
        exit(1);
    }
    node = (t_node*)malloc(sizeof(t_node));
    if ( !node ){
       fprintf(stderr,"Couldn't malloc, aborting\n");
        exit(1);
    }
   
    aux = parent->start->next; 
    while ((aux!=parent->end) && (strcmp(children, aux->data)>0))
        aux = aux->next;

    node->next = aux;
    node->prev = aux->prev;
    aux->prev->next = node;
    aux->prev = node;

    node->data = children;
    ++parent->size;
}

//erases family so that the parent variable can be reused
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
