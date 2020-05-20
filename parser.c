#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> //access()

#define MAX 4

#define _error(A,B) fprintf(stderr, "dbgen: %s%s\n", A, B)
#define TRUE 0
#define FALSE 1

#define NIL "\0"
#define ERR_READ "READ ERROR -- "
#define ERR_FILE "CANT READ FILE -- "
#define ERR_LINK "CANT LINK MORE THAN TWO FILES"

typedef struct info {
    char *method;
    char *lwall;
    char *rwall;
    char *amount;
    char *file;
    char delim;

    struct info *link;
} t_info;

void def_method(char*, t_info*);
void def_column(char*, t_info*);
void def_delim(char*, t_info*);

char *continue_then_init( int(*fn)(int), char *src, short *i);

void print_info(t_info*);
void init_info(t_info*);

int main(int argc, char *argv[])
{
    // ignore first argument
    --argc;
    ++argv;

    t_info info[argc];
    for(short i = 0; i < argc; ++i){
        init_info( info+i );
    }

    short i = 0;
    t_info *info_ptr;
    void (*fn)(char*, t_info*);
    while(argc--)
    {
        switch ( **argv ){
            case '-':
                fn = &def_method;
                info_ptr = info+i;
                break;
            case '[':
                fn = &def_column;
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

    for (short j = 0; j < i; ++j){
        print_info(info+j);
    }
}

void print_info(t_info* info)
{
    fprintf(stderr, "\nmethod: %s\nfile: %s\nrange: %s-%s\namount: %s\ndelim: %c\nlink: %p\n\n",info->method, info->file, info->lwall, info->rwall, info->amount, info->delim, info->link);

    if (info->link){
        fprintf(stderr, "\nmethod: %s\nfile: %s\nrange: %s-%s\namount: %s\ndelim: %c\nlink: %p\n\n",info->link->method, info->link->file, info->link->lwall, info->link->rwall, info->link->amount, info->link->delim, info->link->link);
    }
};

void init_info(t_info* info)
{
    info->lwall = NULL;
    info->rwall = NULL;
    info->amount = NULL;
    info->file = NULL;
    info->delim = '\0';

    info->link = NULL;
}

void def_method(char str[], t_info *info)
{
    short i = 0;
    while ( str[i] ){
       switch ( str[i++] ){
            case 'r': case 'u':
            case 'f': case 's':
                break;
            default:
                _error(ERR_READ, str+i);
                exit(EXIT_FAILURE);
       }
       info->method = strndup(str, i);
    }
}

char *continue_then_init( int(*fn)(int), char *src, short *i)
{
    short j = (*i)+1;
    while ( (fn)(src[++(*i)]) )
        continue;
    
    return strndup(src+j, (*i)-j);
}

void def_column(char str[], t_info *info)
{
    short i = 0;
    while ((str[i]) && (str[i] != ']')){
       switch ( str[i] ){
            case '-':
                info->lwall = strndup(str, i);
                info->rwall = continue_then_init(&isdigit, str, &i); 
                break;
            case ',':
                info->amount = continue_then_init(&isdigit, str, &i); 
                break;
            default:
                ++i;
       }
    }
    if ( info->lwall == NULL ){
        info->file = strndup(str, i);
        if ( access( info->file, R_OK ) == -1 ){
            _error(ERR_FILE, info->file);
            exit(EXIT_FAILURE);
        }
        
    }
    if ((str[i] == ']') && (str[++i] == '~') && (str[++i] == '[')){
        if (info->link){
            _error(ERR_LINK, NIL);
            exit(EXIT_FAILURE);
        }
        info->link = malloc(sizeof(t_info));
        init_info(info->link);
        info->link->link = info;

        def_column(str+(++i), info->link);
    }

}

void def_delim(char str[], t_info *info)
{
    if ( strlen(str) != 1 ){
        _error(ERR_READ, str);
        exit(EXIT_FAILURE);
    }

    switch (str[0]){
        case 'b': case '\"': case '\'':
        case '|' : case '/' : case ';' :
            info->delim = str[0];
            break;
        default:
            _error(ERR_READ, str);
            exit(EXIT_FAILURE);
    }
}
