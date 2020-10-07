#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> //access()
#include <assert.h>

#include "libdbgen.h"

/* @disclaimer this file will be completely remade using POSIX command, that's
    why I didn't rework formatting */

enum optype {
    Option=1, 
    Field=2, 
    Delim=4, 
    UniqueKey=8,
    NextArg=16
};

static int
dbgen_parse_op(char *argv[], t_colinfo* info, dbconfig* database)
{
    //lock makes sure that when consecutive linking, each link is
    //initialized before proceeding any to non-linkable columns
    static t_colinfo *lock = NULL;

    int (*fn)(char*[], t_colinfo*, dbconfig*);
    t_colinfo *info_ptr;
    char **argv_ptr = argv;

    switch ( **argv ){
        case '-': // Option op
            info_ptr = info;
            fn = &dbgen_def_method;
            *argv_ptr = (*argv)+1; //skips first char
            break;
        case '[': // Field op
            fn = &dbgen_def_field;
            info_ptr = info;
            *argv_ptr = (*argv)+1;
            break;
        case '/': // Delim op
            info_ptr = info;
            fn = &dbgen_def_delim;
            *argv_ptr = (*argv)+1;
            break;
        case '~': // UniqueKey op
            if ( !lock ){ //if lock is inactive
                lock = info-1; //points lock to linker's index
            } info_ptr = lock; //points to active linker

            fn = &dbgen_def_ukey;
            argv_ptr = argv+1; //skips current line
            break;
        default:
            _error(ERR_READ, *argv);
            exit(EXIT_FAILURE);
    }
    int optype = (fn)(argv_ptr, info_ptr, database);
    
    // resets lock if previous operation didn't involve linking
    if (( lock != info_ptr ) && ( lock )){
        lock = NULL;
    }

    return optype;
}

static void
dbgen_op_select(int argc, char *argv[], t_colinfo *info, dbconfig* database)
{
    short i = 0;
    while(argc--)
    {
        int optype = dbgen_parse_op(argv, info+i, database);

        if ( optype & UniqueKey ){
            ++argv;
            --argc;
        }
        else if ( optype & Field ){
            ++i;
        }
        else if ( optype & Option ){
            if ( optype & NextArg ){
                ++argv;
                --argc;
            }
        }
        else if ( optype & Delim ){
        } //not used for now
        else {
            _error(ERR_READ,"Invalid op");
            exit(EXIT_FAILURE);
        }
        ++argv;
    }
}

t_colinfo*
dbgen_parser(int argc, char *argv[], dbconfig* database)
{
    if (argc <= 1){
        _error(ERR_READ,"no argument given");
        exit(EXIT_FAILURE);
    }

    { // ignore first argument
        --argc;
        ++argv;
    }

    short amt_col = 0, amt_ukey = 0;
    dbgen_count_colinfo(argc, argv, &amt_col, &amt_ukey);

    t_colinfo *info = malloc(amt_col * sizeof(t_colinfo));

    dbgen_init_colinfo(info, amt_col);
    dbgen_op_select(argc, argv, info, database);

    database->amt_col = amt_col + amt_ukey;

    return info;
}

void
dbgen_count_colinfo(int argc, char *argv[], short *amt_col, short *amt_ukey)
{
    *amt_col = 0;
    *amt_ukey = 0;

    while ( argc-- ){
        if (strchr(*argv,'[') && strchr(*argv,']')){
            ++(*amt_col);
        }    
        else if (( **argv == '~' ) && ( strlen(*argv) == 1 )){
            --(*amt_col);
            ++(*amt_ukey);
        }
        ++argv;
    } 
}

void
dbgen_print_colinfo(t_colinfo* info, short amt_col)
{
    for ( short i = 0; i < amt_col; ++i ){
        fprintf(stderr, "\nmethod: %d\nfile: %s\nrange: %s-%s\namount: %s\ndelim: %c\ndecimal_places: %d\nukey: %p\n\n",(info+i)->method, (info+i)->file, (info+i)->lwall, (info+i)->rwall, (info+i)->amount, (info+i)->delim, (info+i)->decimal_places, (void*)(info+i)->ukey);

        t_colinfo *ptr_ukey = (info+i)->ukey;
        while ( ptr_ukey ){
            --amt_col;
            fprintf(stderr, "\tmethod: %d\n\tfile: %s\n\trange: %s-%s\n\tamount: %s\n\tdelim: %c\n\tdecimal_places: %d\n\tukey: %p\n\n",ptr_ukey->method, ptr_ukey->file, ptr_ukey->lwall, ptr_ukey->rwall, ptr_ukey->amount, ptr_ukey->delim, ptr_ukey->decimal_places, (void*)ptr_ukey->ukey);

            ptr_ukey = ptr_ukey->ukey;
        }
    }
}

void
dbgen_init_colinfo(t_colinfo* info, short amt_col)
{
    //every other member set to NULL value
    const t_colinfo default_colinfo = { 0 };

    for ( short i = 0; i < amt_col; ++i ){
        info[i] = default_colinfo;
    }
}

void
dbgen_clean_colinfo(t_colinfo* info)
{
    if (info->lwall)
        free(info->lwall);
    if (info->rwall)
        free(info->rwall);
    if (info->amount)
        free(info->amount);
    if (info->file)
        free(info->file);
}

void
dbgen_destroy_colinfo(t_colinfo* info, short amt_col)
{
    for ( short i = 0; i < amt_col; ++i ){
        dbgen_clean_colinfo(info+i);
        if ((info+i)->ukey){
            t_colinfo* prev_ukey = (info+i)->ukey;
            t_colinfo* next_ukey = prev_ukey->ukey;
            while ( prev_ukey ){
                --amt_col;
                dbgen_clean_colinfo(prev_ukey);
                free(prev_ukey);
                prev_ukey = next_ukey;
            }
        }
    }
    free(info);
}

int
dbgen_def_ukey(char *argv[], t_colinfo *info, dbconfig* database)
{
    if ( **argv != '[' ){
        _error(ERR_READ,"missing '[' field's delimiter");
        exit(EXIT_FAILURE);
    }

    t_colinfo *ptr_prev = info;
    t_colinfo *ptr_next = info->ukey;
    while (ptr_next){
        ptr_prev = ptr_prev->ukey;
        ptr_next = ptr_next->ukey;
    } ptr_next = malloc(sizeof(t_colinfo));
    ptr_prev->ukey = ptr_next;    

    dbgen_init_colinfo(ptr_next, 1);
    ptr_next->method |= DBGEN_LINKING;

    dbgen_parse_op(argv, ptr_next, database);

    return UniqueKey;
}

int
dbgen_def_method(char *argv[], t_colinfo *info, dbconfig* database)
{
    short i, j;
    short new_method = DBGEN_UNDEFINED;
    char *str_ptr;
    int optype = DBGEN_UNDEFINED;

    i = 0;
    do {
       switch ( (*argv)[i] ){
            case 'r':
                new_method |= DBGEN_RANDOM; 
                break;
            case 'u':
                new_method |= DBGEN_UNIQUE;
                break;
            case 's':
                new_method |= DBGEN_SCALEABLE;
                break;
            case 'o':
                str_ptr = *(argv+1);
                if ( access( str_ptr, W_OK ) == 0 ){
                    _error("FILE ALREADY EXISTS -- ", str_ptr);
                    exit(EXIT_FAILURE);
                }
                database->out_stream = fopen(str_ptr, "w"); 
                optype |= NextArg;
                break;
            case 'S':
                str_ptr = *(argv+1);
                j = 0;
                while ( str_ptr[j] ){
                    if ( !isdigit(str_ptr[j]) ){
                        _error("INVALID NUMBER FOR SIZE -- ", str_ptr);
                        exit(EXIT_FAILURE);
                    }
                    ++j;
                }
                database->amt_row = strtol(str_ptr,NULL,10);
                optype |= NextArg;
                break;
            default:
                _error(ERR_READ, *(argv+i));
                exit(EXIT_FAILURE);
       }
       ++i;
    } while ( (*argv)[i] );
    
    info->method |= new_method;
    
    return ( optype | Option );
}

char*
dbgen_continue_then_init( int(*fn)(int), char *src, short *i)
{
    short j = 0;
    short *ptr_i = i;

    if ( ptr_i ){
        j = *i;
    } else {
        short sub_i = 0;
        ptr_i = &sub_i;
    }
    
    short hold_index = *ptr_i;
    while ( (fn)(src[(*ptr_i)]) ){
        ++(*ptr_i);
    }

    if ( *ptr_i != hold_index ){
        char *dest = strndup( src+j, (*ptr_i)-j );
        return dest;
    }

    return NULL;
}

static void
dbgen_add_decimal_places(char *argv[], t_colinfo *info, short *i)
{
    if ( (*argv)[*i] == '.' ){
        ++(*i); //skip dot

        char *leftover = dbgen_continue_then_init(&isdigit, *argv, i);
        info->decimal_places = strlen( leftover );
        info->lwall = realloc(info->lwall, info->decimal_places+(*i));

        if ( leftover ){
            strcat( info->lwall, "." );
            strcat( info->lwall, leftover );
            free(leftover);
        }
    }
}

int
dbgen_def_field(char *argv[], t_colinfo *info, dbconfig* database)
{
    short i = 0;
    short j;
    while ( (*argv)[i] != ']' ){
       switch ( (*argv)[i] ){
            case '-':
                j = 0;
                info->lwall = dbgen_continue_then_init(&isdigit, *argv, &j);
                dbgen_add_decimal_places(argv, info, &j);

                ++i; // skip hyphen

                info->rwall = dbgen_continue_then_init(&isdigit, *argv, &i); 
                if ( (*argv)[i] == '.' ){
                    ++i; //skip dot

                    char *leftover = dbgen_continue_then_init(&isdigit, *argv, &i);
                    if ( info->decimal_places < strlen(leftover) ){
                        info->decimal_places = strlen(leftover);
                    }
                    info->rwall = realloc(info->rwall, info->decimal_places+i);

                    if ( leftover ){
                        strcat( info->rwall, "." );
                        strcat( info->rwall, leftover );
                        free(leftover);
                    }
                }
                break;
            case ',':
                ++i; // skip comma

                info->amount = dbgen_continue_then_init(&isdigit, *argv, &i); 
                info->method |= DBGEN_FIXEDSIZE;
                break;
            default:
                if ( ! isgraph((*argv)[i]) ){
                    _error(ERR_READ,"missing ']' terminator");
                    exit(EXIT_FAILURE);
                }
                ++i;
                break;
       }
    }

    //if no range found then must be a file
    if ( !info->lwall || !info->rwall ){
        info->decimal_places = 0; //resets to default value
        short trim = i; //trim string if a comma is found

        if ( info->amount ){
            trim -= strlen(info->amount)+1;
        }

        info->file = strndup(*argv, trim);
        if ( access( info->file, R_OK ) == -1 ){
            _error(ERR_FILE, info->file);
            exit(EXIT_FAILURE);
        }
    }

    return Field;
}

int
dbgen_def_delim(char *argv[], t_colinfo *info, dbconfig* database)
{
    if ( strlen(*argv) != 1 ){
        _error(ERR_READ, *argv);
        exit(EXIT_FAILURE);
    }

    switch (**argv){
        case 's': 
            info->delim = ' ';
            break;
        case 't' :
            info->delim = '\t';
            break;
        case '\"': case '\'': case '|' : 
        case '/' : case ';' :
            info->delim = **argv;
            break;
        default:
            _error(ERR_READ, *argv);
            exit(EXIT_FAILURE);
    }

    return Delim;
}

dbconfig*
dbgen_init_dbconfig()
{
    const dbconfig default_database = {
        .amt_row = 10, 
        .delim = ',', 
        .amt_col = 0,
        .out_stream = stdout
    };
    
    dbconfig *new_database = malloc(sizeof(dbconfig));
    *new_database = default_database;

    return new_database;
}

void
dbgen_destroy_dbconfig(dbconfig* database)
{
    assert(database);
    if (database->out_stream != stdout)
        fclose(database->out_stream);
    free(database);
}

