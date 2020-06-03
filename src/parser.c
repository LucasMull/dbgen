#include <ctype.h>
#include <unistd.h> //access()

#include "../config.h"
#include "../dbgen.h"

enum optype {
    Option=1, 
    Field=2, 
    Delim=4, 
    Link=8
};

static short parse_op(char *argv[], t_colinfo *info)
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
            arg_ptr = (*argv)+1; //skips first char
            op_type = Option;
            break;
        case '[': // Field op
            fn = &def_field;
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
            if ( !lock ){ //if lock is inactive
                lock = info-1; //points lock to linker's index
            } info_ptr = lock; //points to active linker

            fn = &def_link;
            arg_ptr = *(argv+1); //skips current line
            op_type = Link;
            break;
        default:
            _error(ERR_READ, *argv);
            exit(EXIT_FAILURE);
    }
    (fn)(arg_ptr, info_ptr);
    
    // resets lock if previous operation didn't involve linking
    if (( lock != info_ptr ) && ( lock )){
        lock = NULL;
    }

    return op_type;
}

static void op_select(int argc, char *argv[], t_colinfo *info)
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

t_colinfo *parser(int argc, char *argv[], dbconfig* database)
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

    init_colinfo(info, amt_cols);
    op_select(argc, argv, info);

    if ( database )
        database->amt_cols = amt_cols;

    return info;
}

short count_colinfo(int argc, char *argv[])
{
    short amt_cols = 0;
    while ( argc-- ){
        if (strchr(*argv,'[') && strchr(*argv,']')){
            ++amt_cols;
        }    
        else if (( **argv == '~' ) && ( strlen(*argv) == 1 )){
            --amt_cols;
        }
        ++argv;
    } 
    return amt_cols;
}

void print_colinfo(t_colinfo* info, short amt_cols)
{
    for ( short i = 0; i < amt_cols; ++i ){
        fprintf(stderr, "\noption: %s\nfile: %s\nrange: %s-%s\namount: %s\ndelim: %c\ndecimals: %d\nlink: %p\n\n",(info+i)->option, (info+i)->file, (info+i)->lwall, (info+i)->rwall, (info+i)->amount, (info+i)->delim, (info+i)->decimals, (void*)(info+i)->link);

        t_colinfo *ptr_link = (info+i)->link;
        while ( ptr_link ){
            fprintf(stderr, "\toption: %s\n\tfile: %s\n\trange: %s-%s\n\tamount: %s\n\tdelim: %c\n\tdecimals: %d\n\tlink: %p\n\n",ptr_link->option, ptr_link->file, ptr_link->lwall, ptr_link->rwall, ptr_link->amount, ptr_link->delim, ptr_link->decimals, (void*)ptr_link->link);

            ptr_link = ptr_link->link;
        }
    }
}

void init_colinfo(t_colinfo* info, short amt_cols)
{
    //every other member set to NULL value
    const t_colinfo default_colinfo = { 
        .option = {'\0'} 
    };

    for ( short i = 0; i < amt_cols; ++i ){
        info[i] = default_colinfo;
    }
}

void clean_colinfo(t_colinfo* info, short amt_cols)
{
    for ( short i = 0; i < amt_cols; ++i ){
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
            case 's':
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

static void add_decimals(char str[], t_colinfo *info, short *i)
{
    if ( str[*i] == '.' ){
        ++(*i); //skip dot

        char *leftover = continue_then_init(&isdigit, str, i);
        info->decimals = strlen( leftover );
        info->lwall = realloc(info->lwall, info->decimals+(*i));

        if ( leftover ){
            strcat( info->lwall, "." );
            strcat( info->lwall, leftover );
            free(leftover);
        }
    }
}

void def_field(char str[], t_colinfo *info)
{
    short i = 0;
    short j;
    while ( str[i] != ']' ){
       switch ( str[i] ){
            case '-':
                j = 0;
                info->lwall = continue_then_init(&isdigit, str, &j);
                add_decimals(str, info, &j);

                ++i; // skip hyphen

                info->rwall = continue_then_init(&isdigit, str, &i); 
                if ( str[i] == '.' ){
                    ++i; //skip dot

                    char *leftover = continue_then_init(&isdigit, str, &i);
                    if ( info->decimals < strlen(leftover) ){
                        info->decimals = strlen(leftover);
                    }
                    info->rwall = realloc(info->rwall, info->decimals+i);

                    if ( leftover ){
                        strcat( info->rwall, "." );
                        strcat( info->rwall, leftover );
                        free(leftover);
                    }
                }
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
    if ( !info->lwall || !info->rwall ){
        info->decimals = 0; //resets to default value
        short trim = i; //trim string if a comma is found

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
        _error(ERR_READ, "invalid delim");
        exit(EXIT_FAILURE);
    }

    switch (str[0]){
        case 's': 
            info->delim = ' ';
            break;
        case 't' :
            info->delim = '\t';
            break;
        case '\"': case '\'': case '|' : 
        case '/' : case ';' :
            info->delim = str[0];
            break;
        default:
            _error(ERR_READ, str);
            exit(EXIT_FAILURE);
    }
}
