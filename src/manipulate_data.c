#include "data_fetch.h"
#include "manipulate_data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

//Initialize empty tree from tag
void initTree(t_tree *T, char *tag)
{
    assert(tag);

    T->size = 0;
    T->data = tag;
    T->root = NULL;
}
//check potential child for it's uniqueness among already
//assigned children. If unique, return child's data address
//otherwise return NULL.

//I should implement a limiter on how many functions calls can be
//made before it gives up
char *uniqueChild(t_tree *T, char *child)
{
    t_node *aux;
    int cmp;
    aux = T->root;

    while (aux != NULL){
        cmp = strcmp(child, aux->data);
        if ( cmp == 0 ){
            return NULL;
        } else if ( cmp > 0 ){
            aux = aux->r;
        } else aux = aux->l;
    }
    return child;
}

//insert child into tree respecting the binary tree format
void insertChild(t_tree *T, char *child){
    t_node *new_node, *aux1, *aux2;
    int cmp; //where strcmp() return value will be stored;
    
    assert(child);

    new_node = (t_node*)malloc(sizeof(t_node));
    assert(new_node);

    new_node->data = child;
    aux1 = T->root;
    aux2 = NULL;
    while( aux1 != NULL ){
        aux2=aux1;
        cmp = strcmp(new_node->data, aux1->data);
        if ( cmp < 0 ){
            aux1 = aux1->l;
        } else aux1 = aux1->r;
    }

    new_node->p = aux2;
    if ( aux2 == NULL ){
        T->root = new_node;
    } else if (strcmp(new_node->data, aux2->data) < 0){
        aux2->l = new_node;
    } else aux2->r = new_node;
    ++T->size;
}

void printTree(t_node *node){
    if ( node ) {
        printTree(node->l);
        printTree(node->r);
        fprintf(stdout,"%s ",node->data);
    }
}

void eraseTree(t_node *node, t_tree *T){
    if ( node ) {
        eraseTree(node->l, T);
        eraseTree(node->r, T);
        free(node);
        --T->size;
    }
}
