#include "data_fetch.h"
#include "manipulate_data.h"

#ifndef FILE_H
#define FILE_H
#include "directives.h"
#endif

//randomly pick data index and returns its address
char *pick_random(t_hblock *block){
    return block->data[ rand() % block->size ];
}

//fetch for DATA at chosen index
char *pick_index(t_hblock *block, size_t i)
{
    /* can fetch linearly only if DBSIZE 
    is smaller or equal to block size 
    AND index is smaller or equal to block size*/
    assert( (DBSIZE <= block->size) && (i <= block->size) );
    return block->data[i];
}

//Pretty straightforward, swaps two string pointers
void str_swap(char **str1_ptr, char **str2_ptr)
{
    char *temp_ptr;

    assert( str1_ptr && str2_ptr );

    temp_ptr = *str1_ptr;
    *str1_ptr = *str2_ptr;
    *str2_ptr = temp_ptr;
}

//Makes use of stringSwap(), to swap string pointers randomly
void shuffle_array(t_hblock *block)
{
    size_t i;

    for ( i = 0 ; i < block->size ; ++i ) //shuffle
        str_swap( block->data + i , (block->data + (rand() % block->size)) );
}

//Initialize empty tree from tag
void init_tree(t_tree *tree, char *new_tag)
{
    assert(new_tag);

    tree->tag = new_tag;
    tree->size = 0;
    tree->root = NULL;
}

t_node *init_node(t_node *new_node)
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
t_node *insert_node(t_tree *tree, t_node *node, char *str, ...)
{
    t_node *aux1, *aux2 = NULL;
    va_list ap;
    
    //fetch node data
    va_start(ap, str);
    node_vararg(node, str, ap);
    va_end(ap);

    aux1 = tree->root;
    while ( aux1 != NULL ){
        aux2 = aux1;
        if ( strcmp(node->data[0], aux1->data[0]) < 0 ){
            aux1 = aux1->l;
        } else aux1 = aux1->r;
    }

    node->p = aux2;
    if ( aux2 == NULL ){
        tree->root = node;
    } else if ( strcmp(node->data[0], aux2->data[0]) < 0 ){
        aux2->l = node;
    } else aux2->r = node;

    ++tree->size;

    return node;
}

void node_vararg(t_node *node, char *str, va_list ap){
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
char *xdata(t_tree *T, char *str)
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
char *find_xdata(t_tree *T, size_t amt, size_t length, char s_array[][length])
{
    size_t i, temp_i; 
    size_t incr, limit;
    char *ptr = NULL;
    short DONE = 0;

    i = rand() % amt;
    temp_i = i;

    incr = 1;
    limit = amt;
    do {
        while ( i != limit ){
            if ((ptr = xdata(T, s_array[i])) != NULL){
                return ptr;
            }
            i += incr;
        }
        i = temp_i;
        limit = 0;
        incr = -1;
    } while (++DONE < 2);
    
    return NULL;
}

void print_tree(t_node *node, FILE *stream){
    size_t i;
    char delim;

    if ( node ){
        print_tree(node->l, stream);
        print_tree(node->r, stream);
        
        i = 0;
        while( i < node->count ){
            if ( i+1 < node->count ){
               delim = ','; 
            } else delim = '\n';
           
            fputs(node->data[i], stream);
            fputc(delim, stream); 

            ++i;
        }
    }
}

void erase_tree(t_node *node, t_tree *T){
    if ( node ){
        erase_tree(node->l, T);
        erase_tree(node->r, T);

        while(node->count){
            free(node->data[--node->count]);
        } free(node->data);
        free(node);

        --T->size;
    }
}
