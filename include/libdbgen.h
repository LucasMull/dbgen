#ifndef DBGEN_H_
#define DBGEN_H_

#include <stdio.h>
#include "config.h"

typedef enum {
  DBGEN_TEMPLATE        = 1 << 0,
  DBGEN_LIST            = 1 << 1,
  DBGEN_FILE            = 1 << 2,
  DBGEN_UNIQUE_KEY      = 1 << 3,
} dbgen_gentype_et;

typedef enum {
  DBGEN_UNDEFINED       = 0,
  DBGEN_RANDOM          = 1 << 0,
  DBGEN_UNIQUE          = 1 << 1,
  DBGEN_SCALEABLE       = 1 << 2,
  DBGEN_FIXEDSIZE       = 1 << 3,
  DBGEN_LINKING         = 1 << 4,
} dbgen_method_et;

typedef struct {
  unsigned int amt_row; //aka database size
  unsigned short amt_col;
  char delim;
  FILE *out_stream;
} dbconfig;

typedef struct colinfo {
  short method;
  char *lwall;
  char *rwall;
  char *amount;
  char *file;
  char delim;
  char decimal_places;

  struct colinfo *ukey; // unique key,
} t_colinfo;

t_colinfo *dbgen_parser(int argc, char *argv[], dbconfig *database);

void dbgen_count_colinfo(int, char *[], short*, short*);

int dbgen_def_ukey(char*[], t_colinfo*, dbconfig*);
int dbgen_def_method(char*[], t_colinfo*, dbconfig*);
int dbgen_def_field(char*[], t_colinfo*, dbconfig*);
int dbgen_def_delim(char*[], t_colinfo*, dbconfig*);

char *dbgen_continue_then_init( int(*)(int), char*, short*);

void dbgen_print_colinfo(t_colinfo*, short amt_info);
void dbgen_init_colinfo(t_colinfo*, short amt_info);
void dbgen_clean_colinfo(t_colinfo*);
void dbgen_destroy_colinfo(t_colinfo*, short amt_info);

dbconfig *dbgen_init_dbconfig();
void dbgen_destroy_dbconfig(dbconfig* database);

typedef struct entity {
  union {
    char *svalue;
    double dvalue;
  };
} t_entity;

typedef struct {
  t_entity tag; //if dependency is of gentype file tag will be svalue
              //otherwise will be dvalue

  union { //if ukey's gentype is file will store svalue,
        //otherwise will store dvalue
    char **store_svalue;
    double *store_dvalue;
  };
  unsigned int n_amt;
} linkstorage;

typedef struct ukey {
  linkstorage **storage;
  unsigned int n_amt;
} t_ukey;

/* @todo this is crazy big, organize*/
typedef struct colgen {
  char method; // how entity will be generated
               // random, unique, scalable (...)

  int gentype; //can be either _template, _list or _ukey gentype
  union {
    t_entity *_template;

    struct {
      t_entity **_list;
      t_entity *_curr_entity; // points to index of current output entity
    };                   // will point to NULL if column prints
  };                       // incremental values

  t_ukey *_ukey;
  struct colgen *_dependency; // holds addr of dependency

  /* @todo typedef this */
  void (*fn)(struct colgen*, dbconfig*);

  unsigned int amt_row; //aka max amount of elements in that column

  char delim; //will be set to newline on the last column
  char format_entity[10]; //format entity to be printed

  union { //can be either range of numbers or file's content
    struct { 
      double lwall; //start of range
      double rwall; //end of range
      char decimal_places;
    };
    char *file;
  };
} t_colgen;

/* DATA_FETCH FUNCTIONS */
void dbgen_numsetter_templ(t_colgen*, dbconfig*);
void dbgen_numsetter_arrlist(t_entity** arrlist, t_colgen* colgen);
void dbgen_filesetter_arrlist(t_entity** arrlist, t_colgen* colgen);
void dbgen_generate_database(t_colgen** arrcolgen, dbconfig* database);

/* START_COL FUNCTIONS */
t_entity *dbgen_init_entity();

void dbgen_build_arrlist(t_colgen*, dbconfig*);
void dbgen_destroy_list(t_entity*, t_colgen*);
void dbgen_destroy_arrlist(t_entity**, t_colgen*);
void dbgen_print_arrlist(size_t, t_entity**, short);

void dbgen_build_templ(t_colgen*, t_entity*, dbconfig*);
void dbgen_destroy_templ(t_entity*);
void dbgen_print_templ(t_entity*);

linkstorage *dbgen_init_linkstorage();
linkstorage *dbgen_build_linkstorage(t_colgen*, t_colgen*, linkstorage*);
void dbgen_destroy_linkstorage(linkstorage*, const int);

t_ukey *dbgen_init_ukey();
void dbgen_build_ukey(t_colgen*, t_colgen*, t_ukey*, dbconfig*);
void dbgen_destroy_ukey(t_ukey*, const int);

t_colgen *dbgen_init_colgen(dbconfig*);
t_colgen *dbgen_build_colgen(t_colinfo*, t_colgen*, t_colgen*, dbconfig*);
int dbgen_build_ukey_colgen(t_colgen**, t_colgen*, t_colinfo*, dbconfig*);
t_colgen **dbgen_build_arrcolgen(t_colinfo*, dbconfig*);

void dbgen_destroy_colgen(t_colgen*);
void dbgen_destroy_arrcolgen(t_colgen**, int);
void dbgen_print_arrcolgen(t_colgen**, int);

#endif
