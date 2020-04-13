#include <stdio.h>

typedef struct node {
        struct node *l;
        struct node *r;
        struct node *p;

        char *data; //should be *void
} t_node;

typedef struct assign {
        t_node *start;
        t_node *end;

        char *data;
        int size;
} t_assign;

typedef struct tree {
        t_node *root;
        char *data; //should be void*
        size_t size;
} t_tree;

char *pickRandom(t_BLOCK *BLOCK);

char *fetchLinear(t_BLOCK *BLOCK, size_t i);

void stringSwap(char **str1_ptr, char **str2_ptr);

void shuffleArray(t_BLOCK *BLOCK);
/*
These grouped functions make use of binary tree to assign
children data in a orderly fashion, *uniqueChild()
can be used to verify any occurence of a given children
inside the tree, returning NULL if there is an
occurence, and the child address if there isn't.

Should probably turn this into its own library.
*/
void initTree(t_tree *T, char *tag);
char *uniqueChild(t_tree *T, char *child);
void insertChild(t_tree *T, char *child);
void printTree(t_node *node);
void eraseTree(t_node *node, t_tree *T);
