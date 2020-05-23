#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> //access()
#include "parser.h"

t_colinfo *parser(int argc, char *argv[], int *ret_amt_cols)
{
    if (argc <= 1){
        _error(ERR_READ,"no argument given");
        exit(EXIT_FAILURE);
    }

    // ignore first argument
    --argc;
    ++argv;
    
    int amt_cols = count_colinfo(argc, argv);
    t_colinfo *info = malloc(amt_cols * sizeof(t_colinfo));
    init_colinfo( info, amt_cols );

    short i = 0;
    while(argc--)
    {
        t_colinfo *info_ptr;
        void (*fn)(char*, t_colinfo*);

        switch ( **argv ){
            case '-':
                fn = &def_option;
                info_ptr = info+i;
                break;
            case '[':
                fn = &def_colinfo;
                info_ptr = info+i;
                break;
            case '/':
                fn = &def_delim;
                info_ptr = info+(i-1);
                break;
            default:
                _error(ERR_READ, *argv);
                exit(EXIT_FAILURE);
        }
        (fn)((*argv)+1, info_ptr);
        
        if ( info[i].lwall || info[i].file ){
            ++i;
        }
        
        ++argv;
    }

    if ( ret_amt_cols != NULL ){
        *ret_amt_cols = amt_cols;
    }
    
    return info;
}

int count_colinfo(int argc, char *argv[])
{
    int amt_cols = 0;
    while ( argc-- ){
        if ( strchr(*argv,'[') && strchr(*argv,']') ){
            ++amt_cols;
        }    
        ++argv;
    } 
    return amt_cols;
}

void print_colinfo(t_colinfo* info, int amt_cols)
{
    int i = 0;
    while ( i < amt_cols ){
        fprintf(stderr, "\noption: %s\nfile: %s\nrange: %s-%s\namount: %s\ndelim: %c\nlink: %p\n\n",(info+i)->option, (info+i)->file, (info+i)->lwall, (info+i)->rwall, (info+i)->amount, (info+i)->delim, (info+i)->link);

        if ((info+i)->link){
            fprintf(stderr, "\toption: %s\n\tfile: %s\n\trange: %s-%s\n\tamount: %s\n\tdelim: %c\n\tlink: %p\n\n",(info+i)->link->option, (info+i)->link->file, (info+i)->link->lwall, (info+i)->link->rwall, (info+i)->link->amount, (info+i)->link->delim, (info+i)->link->link);
        }
    ++i;
    }
};

void init_colinfo(t_colinfo* info, int amt_cols)
{
    int i = 0;
    while ( i < amt_cols ){
        (info+i)->option[0] = '\0';
        (info+i)->lwall = NULL;
        (info+i)->rwall = NULL;
        (info+i)->amount = NULL;
        (info+i)->file = NULL;
        (info+i)->delim = '\0';
        (info+i)->link = NULL;
        ++i;
    }
}

void clean_colinfo(t_colinfo* info, int amt_cols)
{
    int i = 0;
    while ( i < amt_cols ){
        if((info+i)->lwall){
            free((info+i)->lwall);
        }
        if((info+i)->rwall){
            free((info+i)->rwall);
        }
        if((info+i)->amount){
            free((info+i)->amount);
        }
        if((info+i)->file){
            free((info+i)->file);
        }
        if((info+i)->link){
            (info+i)->link->link = NULL;
            clean_colinfo((info+i)->link, 1);
        }
        ++i;
    }
    free(info);
}

void def_option(char str[], t_colinfo *info)
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

    strncat(info->option, str, MAX_OPTIONS);
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

void def_colinfo(char str[], t_colinfo *info)
{
    short i = 0;
    while ( str[i] != ']' ){
       switch ( str[i] ){
            case '-':
                info->lwall = continue_then_init(&isdigit, str, NULL);
                ++i; // skip hyphen
                info->rwall = continue_then_init(&isdigit, str, &i); 
                break;
            case ',':
                ++i; // skip comma
                info->amount = continue_then_init(&isdigit, str, &i); 
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

    if ( !info->lwall ){ //if no range found than must be a file
        info->file = strndup(str, i);
        if ( access( info->file, R_OK ) == -1 ){
            _error(ERR_FILE, info->file);
            exit(EXIT_FAILURE);
        }
        
    }

    if ((str[++i] == '~') && (str[++i] == '[')){ //check for linking
        if (info->link){
            _error(ERR_LINK, "\0");
            exit(EXIT_FAILURE);
        }
        info->link = malloc(sizeof(t_colinfo));
        init_colinfo(info->link, 1);
        info->link->link = info;

        def_colinfo(str+(++i), info->link);
    }

}

void def_delim(char str[], t_colinfo *info)
{
    if ( strlen(str) != 1 ){
        _error(ERR_READ, str);
        exit(EXIT_FAILURE);
    }

    switch (str[0]){
        case 'b': case '\"': case '\'':
        case '|' : case '/' : case ';' :
        case 't' :
            info->delim = str[0];
            break;
        default:
            _error(ERR_READ, str);
            exit(EXIT_FAILURE);
    }
}
