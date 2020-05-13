#ifndef FILE_H
#define FILE_H
#include "directives.h"
#endif

typedef struct hblock {
    char **data;
    size_t size;
} t_hblock;

typedef struct heap {
    t_hblock *addr[TOTAL_BLOCKS];
    size_t size;
} t_heap;

typedef char t_sblock;

void init_h(t_hblock *block, t_heap *heap);

void free_h(t_hblock *block, size_t first_i);

void file_to_h(FILE* f_read, t_hblock* block, size_t ln_total);

void nums_to_h(t_hblock *block, long int first, long int last, size_t amount, size_t length);

<<<<<<< HEAD
void nums_to_s(long int first, long int last, size_t amount, size_t length, t_sblock block[][length]);
=======
void nums_to_s(long int first, long int last, size_t amount, size_t digits, t_sblock block[][digits]);
>>>>>>> 9b07480291894254e98e87299de0b47ad8af5037
