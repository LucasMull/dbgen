#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> //access()
#include "parser.h"

t_column *parser(int argc, char *argv[], int *ret_amt_col)
{
    if (argc <= 1){
        _error(ERR_READ,"no argument given");
        exit(EXIT_FAILURE);
    }

    // ignore first argument
    --argc;
    ++argv;
    
    int amt_col = count_column(argc, argv);
    t_column *col = malloc(amt_col * sizeof(t_column));
    init_column( col, amt_col );

    short i = 0;
    while(argc--)
    {
        t_column *col_ptr;
        void (*fn)(char*, t_column*);

        switch ( **argv ){
            case '-':
                fn = &def_option;
                col_ptr = col+i;
                break;
            case '[':
                fn = &def_column;
                col_ptr = col+i;
                break;
            case '/':
                fn = &def_delim;
                col_ptr = col+(i-1);
                break;
            default:
                _error(ERR_READ, *argv);
                exit(EXIT_FAILURE);
        }
        (fn)((*argv)+1, col_ptr);
        
        if ( col[i].lwall || col[i].file ){
            ++i;
        }
        
        ++argv;
    }

    if ( ret_amt_col != NULL ){
        *ret_amt_col = amt_col;
    }
    
    return col;
}

int count_column(int argc, char *argv[])
{
    int amt_col = 0;
    while ( argc-- ){
        if ( **(argv++) == '[' ){ 
            ++amt_col;
        }    
    } 
    return amt_col;
}

void print_column(t_column* col, int amt_col)
{
    int i = 0;
    while ( i < amt_col ){
        fprintf(stderr, "\noption: %s\nfile: %s\nrange: %s-%s\namount: %s\ndelim: %c\nlink: %p\n\n",(col+i)->option, (col+i)->file, (col+i)->lwall, (col+i)->rwall, (col+i)->amount, (col+i)->delim, (col+i)->link);

        if ((col+i)->link){
            fprintf(stderr, "\toption: %s\n\tfile: %s\n\trange: %s-%s\n\tamount: %s\n\tdelim: %c\n\tlink: %p\n\n",(col+i)->link->option, (col+i)->link->file, (col+i)->link->lwall, (col+i)->link->rwall, (col+i)->link->amount, (col+i)->link->delim, (col+i)->link->link);
        }
    ++i;
    }
};

void init_column(t_column* col, int amt_col)
{
    int i = 0;
    while ( i < amt_col ){
        (col+i)->option[0] = '\0';
        (col+i)->lwall = NULL;
        (col+i)->rwall = NULL;
        (col+i)->amount = NULL;
        (col+i)->file = NULL;
        (col+i)->delim = '\0';
        (col+i)->link = NULL;
        ++i;
    }
}

void clean_column(t_column* col, int amt_col)
{
    int i = 0;
    while ( i < amt_col ){
        if((col+i)->lwall){
            free((col+i)->lwall);
        }
        if((col+i)->rwall){
            free((col+i)->rwall);
        }
        if((col+i)->amount){
            free((col+i)->amount);
        }
        if((col+i)->file){
            free((col+i)->file);
        }
        if((col+i)->link){
            (col+i)->link->link = NULL;
            clean_column((col+i)->link, 1);
        }
    ++i;
    }
    free(col);
}

void def_option(char str[], t_column *col)
{
    if ( strlen(str) > MAX_OPTIONS ){
        _error(ERR_READ, "too many options");
        exit(EXIT_FAILURE);
    }

    short i = 0;
    do {
       switch ( str[i] ){
            case 'r': case 'u':
            case 'f': case 's':
                break;
            default:
                _error(ERR_READ, str+i);
                exit(EXIT_FAILURE);
       }
       ++i;
    } while ( str[i] );

    strncat(col->option, str, MAX_OPTIONS);
}

char *continue_then_init( int(*fn)(int), char *src, short *i)
{
    short j = 0;
    short *ptr_i = i;

    if ( ptr_i ){
        j = *i;
    } else {
        short sub_i = 0;
        ptr_i = &sub_i;
    }
    
    while ( (fn)(src[(*ptr_i)]) ){
        ++(*ptr_i);
    }
    
    return strndup(src+j, (*ptr_i)-j);
}

void def_column(char str[], t_column *col)
{
    short i = 0;
    while ( str[i] != ']' ){
       switch ( str[i] ){
            case '-':
                col->lwall = continue_then_init(&isdigit, str, NULL);
                ++i; // skip hyphen
                col->rwall = continue_then_init(&isdigit, str, &i); 
                break;
            case ',':
                ++i; // skip comma
                col->amount = continue_then_init(&isdigit, str, &i); 
                break;
            default:
                if ( ! isgraph(str[i]) ){
                    _error(ERR_READ,"missing ']' terminator");
                    exit(EXIT_FAILURE);
                }
                ++i;
                break;
       }
    }

    if ( !col->lwall ){ //if no range found than must be a file
        col->file = strndup(str, i);
        if ( access( col->file, R_OK ) == -1 ){
            _error(ERR_FILE, col->file);
            exit(EXIT_FAILURE);
        }
        
    }

    if ((str[++i] == '~') && (str[++i] == '[')){ //check for linking
        if (col->link){
            _error(ERR_LINK, "\0");
            exit(EXIT_FAILURE);
        }
        col->link = malloc(sizeof(t_column));
        init_column(col->link, 1);
        col->link->link = col;

        def_column(str+(++i), col->link);
    }

}

void def_delim(char str[], t_column *col)
{
    if ( strlen(str) != 1 ){
        _error(ERR_READ, str);
        exit(EXIT_FAILURE);
    }

    switch (str[0]){
        case 'b': case '\"': case '\'':
        case '|' : case '/' : case ';' :
        case 't' :
            col->delim = str[0];
            break;
        default:
            _error(ERR_READ, str);
            exit(EXIT_FAILURE);
    }
}
