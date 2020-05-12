#ifndef FILE_H
#define FILE_H
#include "directives.h"
#endif

typedef struct node {
        struct node *l;
        struct node *r;
        struct node *p;

        char **data; //should be *void
        size_t count;
} t_node;

typedef struct tree {
        t_node *root;
        char *data; //should be void*
        size_t size;
} t_tree;

char *pickRandom(t_hblock *block);

char *pickAtIndex(t_hblock *block, size_t i);

void stringSwap(char **str1_ptr, char **str2_ptr);

void shuffleArray(t_hblock *block);
/*
These grouped functions make use of binary tree to assign
children data in a orderly fashion, *uniqueChild()
can be used to verify any occurence of a given children
inside the tree, returning NULL if there is an
occurence, and the child address if there isn't.

Should probably turn this into its own library.
*/
void initTree(t_tree *T, char *tag);
t_node *initNode(t_node *node);
t_node *insertNode(t_tree *T, t_node *node, char *str, ...);
void parseStrIntoNode(t_node *node, char *str, va_list ap);
char *xData(t_tree *T, char *str);
char *findxData(t_tree *T, size_t amt, size_t length, char s_array[][length]);
void printTree(t_node *node, FILE* stream);
void eraseTree(t_node *node, t_tree *T);
