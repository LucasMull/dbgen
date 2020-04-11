#include <stdio.h>

typedef struct node {
        struct node *next;
        struct node *prev;

        char *data;
} t_node;

typedef struct assign {
        t_node *start;
        t_node *end;

        char *data;
        int size;
} t_assign;

char *pickRandom(t_BLOCK *BLOCK);

void joinStrings(char joint[], char str1[], char str2[]);

void joinRandom(char joint[], const t_BLOCK *BLOCK1, const t_BLOCK *BLOCK2);

void fetchLinear(char dest[], t_BLOCK *BLOCK, size_t i);

void fetchRandom(char dest[], t_BLOCK *BLOCK);

void stringSwap(char **str1_ptr, char **str2_ptr);

void shuffleArray(t_BLOCK *BLOCK);
/*
These grouped functions make use of linked list to assign
n amount of children data to a parent data, uniqueChild()
can be used to verify any occurence of a given children
under the parent data, returning NULL if there is an
occurence, and the children address if there is not,
should probably be turned into a new library.
*/
void initAssign(t_assign *parent, char *str);
char *uniqueChild(t_assign *parent, char *children);
void assignChild(t_assign *parent, char *children);
void eraseFamily(t_assign *parent);
