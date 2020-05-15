#ifndef FILE_H
#define FILE_H
#include "directives.h"
#endif

typedef struct node {
        struct node *l;
        struct node *r;
        struct node *p;

        char **data;
        size_t count;
} t_node;

typedef struct tree {
        t_node *root;
        char *tag;
        size_t size;
} t_tree;

char *pick_random(t_hblock *block);

char *pick_index(t_hblock *block, size_t i);

void str_swap(char **str1_ptr, char **str2_ptr);

void shuffle_array(t_hblock *block);

/*
 * These functions make use of a binary tree to organize
 * common data together, by turning them into nodes. 
 *
 * e.g.
 * char *xdata()
 * can be used to verify any occurence of a given node
 * inside that particular tree, returning NULL if there is an
 * occurence, and the node address if there isn't.
*/
void init_tree(t_tree *tree, char *tag);
t_node *init_node(t_node *node);
t_node *insert_node(t_tree *tree, t_node *node, char *str, ...);
void vnode_data(t_node *node, char *str, va_list ap);
char *xdata(t_tree *tree, char *str);
char *find_xdata(t_tree *tree, size_t amt, size_t length, t_sblock block[][length]);
void print_tree(t_node *node, FILE* stream);
void erase_tree(t_node *node, t_tree *tree);
