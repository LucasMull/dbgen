#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libdbgen.h"

t_entity*
dbgen_init_entity()
{
  t_entity *new_entity = calloc(1, sizeof *new_entity);
  assert(NULL != new_entity);

  return new_entity;
}

/* @todo return new_arrlist */
void
dbgen_build_arrlist(t_colgen *colgen, dbconfig *database)
{
  assert(!colgen->_list);

  t_entity **new_arrlist = malloc(colgen->amt_row * sizeof *new_arrlist);
  assert(NULL != new_arrlist);

  for (int i = 0; i < colgen->amt_row; ++i){
    new_arrlist[i] = dbgen_init_entity();
  }

  colgen->_list = new_arrlist;
}

void
dbgen_destroy_list(t_entity* list, t_colgen* colgen)
{
  assert(NULL != list);

  dbgen_gentype_et gentype = colgen->gentype;

  if ((gentype & DBGEN_FILE) && (list->svalue)){
    free(list->svalue);
  }
  
  free(list);
}

void
dbgen_destroy_arrlist(t_entity** arrlist, t_colgen* colgen)
{
  assert(NULL != arrlist);

  for (int i = 0; i < colgen->amt_row; ++i){
    dbgen_destroy_list(colgen->_list[i], colgen);
  }

  free(arrlist);
}

/* @todo return new_templ */
void
dbgen_build_templ(t_colgen* colgen, t_entity* new_templ, dbconfig* database)
{
  assert(NULL != new_templ);
  assert(NULL == colgen->_template);

  new_templ->dvalue = colgen->lwall;
  
  colgen->_template = new_templ;
}

void
dbgen_destroy_templ(t_entity *templ)
{
  assert(NULL != templ);

  free(templ);
}

linkstorage*
dbgen_init_linkstorage()
{
  linkstorage *new_storage = calloc(1, sizeof *new_storage);
  assert(NULL != new_storage);

  return new_storage;
}

linkstorage*
dbgen_build_linkstorage(t_colgen* dependency, t_colgen* colgen, linkstorage* new_storage)
{
  assert(NULL != dependency);

  if ((dependency->gentype & DBGEN_LIST) && (dependency->_curr_entity)){
    if (dependency->gentype & DBGEN_FILE){
      new_storage->tag.svalue = dependency->_curr_entity->svalue;
    } else {
      new_storage->tag.dvalue = dependency->_curr_entity->dvalue;
    }
  } else { //then it's a template
    new_storage->tag.dvalue = dependency->_template->dvalue;
  }

  return new_storage;
}

void
destroy_linkstorage(linkstorage* storage, const int gentype)
{
  assert(NULL != storage);

  if (gentype & DBGEN_FILE){
    free(storage->store_svalue);
  } else {
    free(storage->store_dvalue);
  }
  free(storage);
}

t_ukey*
init_ukey()
{
  t_ukey *new_ukey = calloc(1, sizeof *new_ukey);
  assert(NULL != new_ukey);

  return new_ukey;
}

void
start_ukey(t_colgen *colgen, t_colgen *dependency, t_ukey *new_ukey, dbconfig *database)
{
  assert(NULL != new_ukey);

  new_ukey->storage = malloc(sizeof *new_ukey->storage);
  assert(NULL != new_ukey->storage);

  colgen->_dependency = dependency;
  colgen->_ukey = new_ukey;
}

void
dbgen_destroy_ukey(t_ukey *ukey, const int gentype)
{
  assert(NULL != ukey);

  if (ukey->storage){
    for (unsigned int i = 0; i < ukey->n_amt ; ++i){
      destroy_linkstorage(ukey->storage[i], gentype);
    }
    free(ukey->storage);
  }
  free(ukey);
}

static void
def_typeof(t_colinfo *info, t_colgen *colgen)
{
  assert(NULL != info);
  assert(NULL != colgen);

  short new_method = info->method;
  short new_gentype = DBGEN_UNDEFINED;
 
  /*
   * if amt of elements given is fixed (will be double checked later) it's a list
   * else if new_method is random and also unique or scalable it's a list
   * otherwise it's a template
   */
  if (NULL != info->file){
    new_gentype = ( DBGEN_FILE | DBGEN_LIST );
  } else if (new_method & DBGEN_FIXEDSIZE) {
    new_gentype = DBGEN_LIST;
  } else if ((new_method & DBGEN_RANDOM) && (new_method & (DBGEN_UNIQUE | DBGEN_SCALEABLE))) {
    new_gentype = DBGEN_LIST;
  } else {
    new_gentype = DBGEN_TEMPLATE; 
  }

  if ( new_method & DBGEN_LINKING ){
    new_gentype |= DBGEN_UNIQUE_KEY;
  }

  colgen->method = new_method;
  colgen->gentype = new_gentype;
}

t_colgen*
init_colgen(dbconfig* database)
{
  t_colgen *new_colgen = malloc(sizeof *new_colgen);
  assert(NULL != new_colgen);

  //everything else is set to NULL value
  const t_colgen default_colgen = { 
      .rwall = database->amt_row,
      .delim = database->delim,
      .amt_row = database->amt_row
  };

  *new_colgen = default_colgen;

  return new_colgen;
}

static size_t
count_flines(char *file)
{
  FILE *f_count = fopen(file, "r");
  assert(NULL != f_count);

  const short LEN = 50;

  char ptr_str[LEN];
  size_t ln = 0;
  while ( fgets(ptr_str, LEN-1, f_count) ){
    ++ln;
  };

  fclose(f_count);
  
  return ln;
}

static void
def_fn(t_colgen* colgen, dbconfig* database)
{
  if (colgen->gentype & DBGEN_LIST){
    if (colgen->gentype & DBGEN_FILE){
      dbgen_filesetter_arrlist(colgen->_list, colgen);
    } else {
      dbgen_numsetter_arrlist(colgen->_list, colgen);
    }
  } else { // colgen->gentype & DBGEN_TEMPLATE
    dbgen_numsetter_templ(colgen, database);
  }
}

t_colgen*
start_colgen(t_colinfo* info, t_colgen* colgen, t_colgen *dependency, dbconfig* database)
{
  if (NULL != info->file){
    colgen->file = strdup(info->file);
  } else {
    if (NULL != info->lwall ){
      colgen->lwall = strtod(info->lwall, NULL);
    }
    if (NULL != info->rwall ){
      colgen->rwall = strtod(info->rwall, NULL);
    }
  } // can be generate either file or range of numbers

  if ('\0' != info->delim){
    colgen->delim = info->delim;
  }

  if (0 != info->decimal_places){
    colgen->decimal_places = info->decimal_places;
  }

  def_typeof(info, colgen);


  if ((colgen->method & DBGEN_FIXEDSIZE) && (atoi(info->amount) >= database->amt_row)){
    colgen->method ^= DBGEN_FIXEDSIZE; //can't have fixed amt be >= dbsize
    colgen->gentype = DBGEN_TEMPLATE; //and shouldn't be a list
  }

  if (colgen->gentype & DBGEN_FILE){
    colgen->amt_row = count_flines(colgen->file);
  } else if ((colgen->method & DBGEN_FIXEDSIZE) && (atoi(info->amount) < database->amt_row)) {
    colgen->amt_row = atoi(info->amount);
  } else if ((colgen->rwall - colgen->lwall) < database->amt_row ) {
    colgen->amt_row = colgen->rwall - colgen->lwall;
  }
  /*
   * if is of file type and random method then it is necessary to
   * fetch the entire file into memory
   * (otherwise random elements scope would be limited to default db size)
   * else if designated amount to be generated is lower than default db size
   * else if range is lower than default dbsize, assign rante as amt
   */


  if (colgen->gentype & DBGEN_FILE){
    sprintf(colgen->format_entity,"%%s%%c");
  } else {
    sprintf(colgen->format_entity,"%%.%df%%c",colgen->decimal_places);
  }


  if (colgen->gentype & DBGEN_LIST){
    dbgen_build_arrlist(colgen, database);
  } else if (colgen->gentype & DBGEN_TEMPLATE) {
    dbgen_build_templ(colgen, dbgen_init_entity(), database);
  }

  if (colgen->gentype & DBGEN_UNIQUE_KEY){
    start_ukey(colgen, dependency, init_ukey(), database);
  }

  def_fn(colgen, database);

  return colgen;
}

static int
_dbgen_build_ukey(t_colgen** new_arrcolgen, t_colgen* dependency, t_colinfo* ptr_ukey, dbconfig* database)
{
  int j=0;

  while (NULL != ptr_ukey){
    ++j;

    new_arrcolgen[j] = start_colgen(ptr_ukey, init_colgen(database), dependency, database);

    ptr_ukey = ptr_ukey->ukey;
  }

  return j;
}

t_colgen**
dbgen_build_arrcolgen(t_colinfo *info, dbconfig* database)
{
  assert(NULL != info);

  t_colgen **new_arrcolgen = malloc(database->amt_col * sizeof *new_arrcolgen);
  assert(NULL != new_arrcolgen);

  int j = 0; //iterate through, and add to, colgen index
  int i = 0; //iterate through colinfo index
  while (j < database->amt_col){ //j will be greater than i if there are ukeys
    new_arrcolgen[j] = start_colgen(info+i, init_colgen(database), NULL, database); 

    if (NULL != (info+i)->ukey){ //adds amount of ukeys to j
      j += _dbgen_build_ukey(new_arrcolgen+j, new_arrcolgen[j], (info+i)->ukey, database);
    }
    
    if (i < database->amt_col){
      ++i;        
    }
    
    ++j;
  }

  // makes sure last col will not have delim and will jump line
  new_arrcolgen[database->amt_col-1]->delim = '\n';

  return new_arrcolgen;
}

void
dbgen_print_arrlist(size_t amt_row, t_entity **arrlist, short gentype)
{
  assert(NULL != arrlist);
  
  for (int i = 0; i < 5; ++i){
    if (gentype & DBGEN_FILE){
      fprintf(stderr, "\t# %s\n", arrlist[i]->svalue);
    } else {
      fprintf(stderr, "\t# %f\n", arrlist[i]->dvalue);
    }
  }
  fputc('\n', stderr);

  fprintf(stderr, "\tand %d more ...\n",abs(amt_row-5));
}

void
dbgen_print_templ(t_entity *templ)
{
  assert(NULL != templ);

  fprintf(stderr, "\tvalue: %f\n", templ->dvalue);
}

void
dbgen_print_arrcolgen(t_colgen **colgen, int amt_col)
{
  assert(NULL != colgen);

  for (int i=0; i < amt_col; ++i){
    fprintf(stderr, "n#: %d\nmethod: %d\namt_row: %d\ndelim: %c\ndecimal_places: %d\n", i+1, colgen[i]->method, colgen[i]->amt_row, colgen[i]->delim, colgen[i]->decimal_places);
    
    char gentype[50] = "";
    if (colgen[i]->gentype & DBGEN_UNIQUE_KEY){
      fprintf(stderr,"dependency: %p\n", (void*)colgen[i]->_dependency);
      strcpy(gentype,"DBGEN_UNIQUE_KEY");
    }

    if (colgen[i]->gentype & DBGEN_FILE){
      fprintf(stderr,"file: %s\n", colgen[i]->file);
      strcat(gentype,"DBGEN_FILE");
    } else {
      fprintf(stderr,"range: %f-%f\n", colgen[i]->lwall, colgen[i]->rwall);
    }

    if (colgen[i]->gentype & DBGEN_TEMPLATE){
      strcat(gentype, "DBGEN_TEMPLATE");
      fprintf(stderr, "gentype: %s\n", gentype);
      dbgen_print_templ(colgen[i]->_template);
    } else if ( colgen[i]->gentype & DBGEN_LIST){
      strcat(gentype,"DBGEN_LIST");
      fprintf(stderr, "gentype: %s\n", gentype);
      dbgen_print_arrlist( colgen[i]->amt_row, colgen[i]->_list, colgen[i]->gentype );
    }
  }
}

void
dbgen_destroy_colgen(t_colgen *colgen)
{
  assert(NULL != colgen);

  if (colgen->gentype & DBGEN_LIST){
    if (colgen->gentype & DBGEN_FILE){
      free(colgen->file); 
    }
    
    dbgen_destroy_arrlist(colgen->_list, colgen);
  } else {
    dbgen_destroy_templ(colgen->_template);
  }

  if (colgen->gentype & DBGEN_UNIQUE_KEY){
    dbgen_destroy_ukey(colgen->_ukey, colgen->gentype);
  }
  
  free(colgen);
}

void
dbgen_destroy_arrcolgen(t_colgen **arrcolgen, int amt_col)
{
  assert(NULL != arrcolgen);

  for (int i = 0; i < amt_col; ++i){
    dbgen_destroy_colgen(arrcolgen[i]);
  }
  
  free (arrcolgen);
}
