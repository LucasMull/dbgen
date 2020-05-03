#include "data_fetch.h"
#include "manipulate_data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

//randomly pick data index and returns its address
char *pickRandom(t_BLOCK *BLOCK){
    return BLOCK->data[ rand() % BLOCK->size ];
}

//fetch for DATA at chosen index
char *pickIndex(t_BLOCK *BLOCK, size_t i)
{
    /* can fetch linearly only if DBSIZE 
    is smaller or equal to BLOCK size 
    AND index is smaller or equal to BLOCK size*/
    assert( (BLOCK->size >= DBSIZE) && (i <= BLOCK->size) );
    return BLOCK->data[i];
}

//Pretty straightforward, swaps two string pointers
void stringSwap(char **str1_ptr, char **str2_ptr)
{
    char *temp;

    assert( str1_ptr && str2_ptr );

    temp = *str1_ptr;
    *str1_ptr = *str2_ptr;
    *str2_ptr = temp;
}

//Makes use of stringSwap(), to swap string pointers randomly
void shuffleArray(t_BLOCK *BLOCK)
{
    size_t i;

    for ( i = 0 ; i < BLOCK->size ; ++i ) //shuffle
        stringSwap( BLOCK->data + i , BLOCK->data + ( rand() % BLOCK->size ) );
}

//Initialize empty tree from tag
void initTree(t_tree *T, char *tag)
{
    assert(T);
    assert(tag);

    T->data = tag;
    T->size = 0;
    T->root = NULL;
}

t_node *initNode(t_node *new_node)
{
    new_node = (t_node*)malloc(sizeof(t_node));
    assert(new_node);

    new_node->data = NULL;
    new_node->count = 0;

    new_node->p = NULL;
    new_node->l = NULL;
    new_node->r = NULL;

    return new_node;
}

//insert node into tree respecting the binary tree format
t_node *insertNode(t_tree *T, t_node *node, char *str, ...)
{
    t_node *aux1, *aux2 = NULL;
    va_list ap;
    
    //fetch node data
    va_start(ap, str);
    parseStrIntoNode(node, str, ap);
    va_end(ap);

    aux1 = T->root;
    while ( aux1 != NULL ){
        aux2 = aux1;
        if ( strcmp(node->data[0], aux1->data[0]) < 0 ){
            aux1 = aux1->l;
        } else aux1 = aux1->r;
    }

    node->p = aux2;
    if ( aux2 == NULL ){
        T->root = node;
    } else if ( strcmp(node->data[0], aux2->data[0]) < 0 ){
        aux2->l = node;
    } else aux2->r = node;

    ++T->size;

    return node;
}

void parseStrIntoNode(t_node *node, char *str, va_list ap){
    char *ptr;
    size_t i;

    assert(node->count == 0);

    while(*str){
        if ( *str++ == '%' ){
            switch (*str){
                case 's' :
                    ++node->count;
                    break;
                default:
                    assert(*str == 's');
                    break; //defensive break just in case
            }
        }
    }

    node->data = (char**)malloc(sizeof(char*) * node->count);
    assert(node->data);
    
    i = 0;
    while (i < node->count){
        ptr = va_arg(ap, char*);
        node->data[i++] = strndup(ptr, strlen(ptr));
    }
}

//check if data holds exclusivity amongst tree's nodes
char *xData(t_tree *T, char *str)
{
    t_node *aux;
    int cmp;

    aux = T->root;
    while (aux != NULL){
        cmp = strcmp(str, aux->data[0]);
        if ( cmp == 0 ){
            return NULL;
        } else if ( cmp > 0 ){
            aux = aux->r;
        } else aux = aux->l;
    }
    return str;
}
//Search and return an exclusive data in the array not present in any tree's nodes
char *findxData(t_tree *T, size_t amt, size_t length, char s_array[][length])
{
    size_t i, temp_i;
    char *ptr = NULL;

    i = rand() % amt;

    temp_i = i;
    while ( i < amt ){
        if ((ptr = xData(T, s_array[i])) != NULL){
            return ptr;
        }
        ++i;
    }

    i = temp_i;
    while ( i >= 0 ){
        if ((ptr = xData(T, s_array[i])) != NULL){
            return ptr;
        }
        --i;
    }
    
    return NULL;
}

void printTree(t_node *node, FILE *stream){
    size_t count;
    char delim;

    if ( node ){
        printTree(node->l, stream);
        printTree(node->r, stream);
        
        count = 0;
        while( count < node->count ){
            if ( count+1 < node->count ){
               delim = ','; 
            } else delim = '\n';

            fprintf(stream, "%s%c", node->data[count], delim);

            ++count;
        }
    }
}

void eraseTree(t_node *node, t_tree *T){
    if ( node ){
        eraseTree(node->l, T);
        eraseTree(node->r, T);

        while(node->count){
            free(node->data[--node->count]);
        }free(node->data);
        free(node);

        --T->size;
    }
}
