#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> //access()
#include "parser.h"

enum ops {Option, Field, Delim, Link};

short parse_op(char *argv[], t_colinfo *info)
{
    //lock makes sure that when consecutive linking, each link is
    //initialized before proceeding any to non-linkable columns
    static t_colinfo *lock = NULL;

    void (*fn)(char*, t_colinfo*);
    t_colinfo *info_ptr;
    char *arg_ptr;
    short op_type;

    switch ( **argv ){
        case '-': // Option op
            info_ptr = info;
            fn = &def_option;
            arg_ptr = (*argv)+1; //skip first char
            op_type = Option;
            break;
        case '[': // Field op
            fn = &def_colinfo;
            info_ptr = info;
            arg_ptr = (*argv)+1;
            op_type = Field;
            break;
        case '/': // Delim op
            info_ptr = info;
            fn = &def_delim;
            arg_ptr = (*argv)+1;
            op_type = Delim;
            break;
        case '~': // Link op
            if ( !lock ){
                lock = info-1;
            } info_ptr = lock;

            fn = &def_link;
            arg_ptr = *(argv+1); //skip ~
            op_type = Link;
            break;
        default:
            _error(ERR_READ, *argv);
            exit(EXIT_FAILURE);
    }
    (fn)(arg_ptr, info_ptr);
    
    // resets lock if previous operation didn't involve linking
    if (( lock ) && ( lock != info_ptr )){
        lock = NULL;
    }

    return op_type;
}

void op_select(int argc, char *argv[], t_colinfo *info)
{
    short i = 0;
    while(argc--)
    {
        short op = parse_op(argv, info+i);

        switch ( op ){
            case Link:
                ++argv;
                --argc;
                break;    
            case Field:
                ++i;
                break;
            case Delim: case Option:
                break;
            default:
                _error(ERR_READ,"Invalid op");
                exit(EXIT_FAILURE);
        }
        ++argv;
    }
}

t_colinfo *parser(int argc, char *argv[], short *ret_amt_cols)
{
    if (argc <= 1){
        _error(ERR_READ,"no argument given");
        exit(EXIT_FAILURE);
    }

    { // ignore first argument
        --argc;
        ++argv;
    }

    short amt_cols = count_colinfo(argc, argv);

    t_colinfo *info = malloc(amt_cols * sizeof(t_colinfo));
    init_colinfo( info, amt_cols );
    op_select(argc, argv, info);

    if ( ret_amt_cols ){
        *ret_amt_cols = amt_cols;
    }

    return info;
}

short count_colinfo(int argc, char *argv[])
{
    short amt_cols = 0;
    while ( argc-- ){
        if ( strchr(*argv,'[') && strchr(*argv,']') ){
            ++amt_cols;
        }    
        if (( **argv == '~' ) && ( strlen(*argv) == 1 )){
            --amt_cols;
        }
        ++argv;
    } 
    return amt_cols;
}

void print_colinfo(t_colinfo* info, short amt_cols)
{
    short i = 0;
    while ( i < amt_cols ){
        fprintf(stderr, "\noption: %s\nfile: %s\nrange: %s-%s\namount: %s\ndelim: %c\nlink: %p\n\n",(info+i)->option, (info+i)->file, (info+i)->lwall, (info+i)->rwall, (info+i)->amount, (info+i)->delim, (info+i)->link);

        t_colinfo *ptr_link = (info+i)->link;
        while ( ptr_link ){
            fprintf(stderr, "\toption: %s\n\tfile: %s\n\trange: %s-%s\n\tamount: %s\n\tdelim: %c\n\tlink: %p\n\n",ptr_link->option, ptr_link->file, ptr_link->lwall, ptr_link->rwall, ptr_link->amount, ptr_link->delim, ptr_link->link);

            ptr_link = ptr_link->link;
        }
    ++i;
    }
};

void init_colinfo(t_colinfo* info, short amt_cols)
{
    short i = 0;
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

void clean_colinfo(t_colinfo* info, short amt_cols)
{
    short i = 0;
    while ( i < amt_cols ){
        if((info+i)->lwall)
            free((info+i)->lwall);
        if((info+i)->rwall)
            free((info+i)->rwall);
        if((info+i)->amount)
            free((info+i)->amount);
        if((info+i)->file)
            free((info+i)->file);
        if((info+i)->link)
            clean_colinfo((info+i)->link, 1);

        ++i;
    }
    free(info);
}

void def_link(char str[], t_colinfo *info)
{
    if ( *str != '[' ){
        _error(ERR_READ,"can't specify linked column options");
        exit(EXIT_FAILURE);
    }

    t_colinfo *ptr_prev = info;
    t_colinfo *ptr_next = info->link;
    while (ptr_next){
        ptr_prev = ptr_prev->link;
        ptr_next = ptr_next->link;
    } ptr_next = malloc(sizeof(t_colinfo));
    ptr_prev->link = ptr_next;    

    init_colinfo(ptr_next, 1);

    char **arg = malloc(sizeof(char*));
    *arg = str;

    parse_op(arg, ptr_next);

    free(arg);
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

    //if no range found then must be a file
    if ( !info->lwall || !info->rwall){
        //trim string if a comma is found
        short trim = i;

        if ( info->amount ){
            trim -= strlen(info->amount)+1;
        }

        info->file = strndup(str, trim);
        if ( access( info->file, R_OK ) == -1 ){
            _error(ERR_FILE, info->file);
            exit(EXIT_FAILURE);
        }
        
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
