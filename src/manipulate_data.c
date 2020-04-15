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

//fetch for DATA linearly under the condition that its elements count is greater than or equal to DBSIZE
char *fetchLinear(t_BLOCK *BLOCK, size_t i)
{
    if ( BLOCK->size < DBSIZE ){
        fprintf(stderr,"Couldn't fetch linearly, BLOCK element count is lesser than DBSIZE");
        exit(1);
    }
    return BLOCK->data[i];
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

    T->data = tag;
    T->size = 0;
    T->root = NULL;
}

t_node *initNode(t_node *new_node)
{
    new_node = (t_node*)malloc(sizeof(t_node));
    if (badAlloc(new_node)) return NULL;

    new_node->data = NULL;
    new_node->p = NULL;
    new_node->l = NULL;
    new_node->r = NULL;

    return new_node;
}

//insert node into tree respecting the binary tree format
t_node *insertNode(t_tree *T, t_node *node, char *str)
{
    t_node *aux1, *aux2 = NULL;
    int cmp; //where strcmp() return value will be stored;

    node->data = str;
    aux1 = T->root;
    while( aux1 != NULL ){
        aux2=aux1;
        cmp = strcmp(node->data, aux1->data);
        if ( cmp < 0 ){
            aux1 = aux1->l;
        } else aux1 = aux1->r;
    }

    node->p = aux2;
    if ( aux2 == NULL ){
        T->root = node;
    } else if (strcmp(node->data, aux2->data) < 0){
        aux2->l = node;
    } else aux2->r = node;
    ++T->size;

    return node;
}

//check potential child for it's uniqueness among already
//assigned children. If unique, return child's data address
//otherwise return NULL.
char *uniqueNodeData(t_tree *T, char *str)
{
    t_node *aux;
    int cmp;
    aux = T->root;

    while (aux != NULL){
        cmp = strcmp(str, aux->data);
        if ( cmp == 0 ){
            return NULL;
        } else if ( cmp > 0 ){
            aux = aux->r;
        } else aux = aux->l;
    }
    return str;
}

void printTree(t_node *node, FILE *stream){
    if ( node ){
        printTree(node->l, stream);
        printTree(node->r, stream);
        fprintf(stream,"%s\n",node->data);
    }
}

void eraseTree(t_node *node, t_tree *T){
    if ( node ){
        eraseTree(node->l, T);
        eraseTree(node->r, T);
        free(node);
        --T->size;
    }
}
