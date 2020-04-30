#include <stdio.h>

typedef struct node {
        struct node *l;
        struct node *r;
        struct node *p;

        char *data1; //should be *void
        char *data2; //should be *void
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

char *pickIndex(t_BLOCK *BLOCK, size_t i);

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
t_node *initNode(t_node *node);
t_node *insertNode(t_tree *T, t_node *new_node, char *str1, char *str2);
char *xData(t_tree *T, char *str);
char *find_xData(t_tree *T, size_t amt, size_t length, char s_array[][length]);
void printTree(t_node *node, FILE* stream);
void eraseTree(t_node *node, t_tree *T);
