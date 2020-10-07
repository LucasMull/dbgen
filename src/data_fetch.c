#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libdbgen.h"

/* @todo turn into macro */
inline static double
dcmp(const double num1, const double num2){
  return num1 - num2;
}

int
dbgen_create_dstorage(linkstorage* storage, t_entity* list)
{
  ++storage->n_amt;
  storage->store_dvalue = realloc(storage->store_dvalue, storage->n_amt * sizeof *storage->store_dvalue);
  assert(NULL != storage->store_dvalue);

  storage->store_dvalue[storage->n_amt-1] = list->dvalue;

  double key = storage->store_dvalue[storage->n_amt-1];
  int i = storage->n_amt-1;
  while ((0 < i) && (0 > dcmp(key, storage->store_dvalue[i-1]))){
    storage->store_dvalue[i] = storage->store_dvalue[i-1];
    --i;
  }
  
  storage->store_dvalue[i] = key;

  return i;
}

int 
dbgen_create_sstorage(linkstorage* storage, t_entity* list)
{
  ++storage->n_amt;
  storage->store_svalue = realloc(storage->store_svalue, storage->n_amt * sizeof *storage->store_svalue);
  assert(NULL != storage->store_svalue);

  storage->store_svalue[storage->n_amt-1] = list->svalue;

  char* key = storage->store_svalue[storage->n_amt-1];
  int i = storage->n_amt-1;
  while ((0 < i) && (0 > strcmp(key, storage->store_svalue[i-1]))){
    storage->store_svalue[i] = storage->store_svalue[i-1];
    --i;
  }
  
  storage->store_svalue[i] = key;

  return i;
}

int
dbgen_search_or_create_dstorage(linkstorage* storage, t_entity* list)
{
  int top = storage->n_amt-1;
  int low = 0;
  int mid;
  while (low <= top){
    mid = ((unsigned int)low + (unsigned int)top) >> 1;
    double cmp = dcmp(list->dvalue, storage->store_dvalue[mid]);

    //can't ukey that value if it already exists
    if (0 == cmp) return -1;

    if (0 > cmp){
      top = mid-1;
    } else {
      low = mid+1;
    }
  }

  return dbgen_create_dstorage(storage, list);
}

int
dbgen_search_or_create_sstorage(linkstorage* storage, t_entity* list)
{
  int top = storage->n_amt-1;
  int low = 0;
  int mid;
  while (low <= top){
    mid = ((unsigned int)low + (unsigned int)top) >> 1;
    double cmp = strcmp(list->svalue, storage->store_svalue[mid]);

    //can't ukey that value if it already exists
    if (cmp == 0) return -1;

    if (0 > cmp){
      top = mid-1;
    } else {
      low = mid+1;
    }
  }

  return dbgen_create_sstorage(storage, list);
}

//create exclusive list based value
int
dbgen_create_xlvalue(t_colgen* colgen, linkstorage* storage, int (*storage_fn)(linkstorage*, t_entity*))
{
  assert(NULL != storage);

  int i = rand() % colgen->amt_row;
  int temp_i = i;
  
  int ceiling = colgen->amt_row;
  short direction = 1;
  do {
    int cmp;
    while (ceiling != i){
      cmp = (storage_fn)(storage, colgen->_list[i]);
      if (-1 != cmp)
          return cmp;
      i += direction;
    }
    i = temp_i;
    ceiling = -1;
    direction *= -1;
  } while (0 < direction); //exits at second loop

  return -1; //can't assign new values
}

//create exclusive template based value
int
dbgen_create_xtvalue( t_colgen* colgen, linkstorage* storage, int (*storage_fn)(linkstorage*, t_entity*) )
{
  assert(NULL != storage);
  
  size_t rnd = rand() % (size_t)(colgen->rwall - colgen->lwall);
  colgen->_template->dvalue = colgen->lwall + rnd; 
  double temp_dvalue = colgen->_template->dvalue;
  
  double ceiling = colgen->rwall;
  short direction = 1;
  do {
    int cmp;
    while (ceiling != colgen->_template->dvalue){
      cmp = (storage_fn)(storage, colgen->_template);

      if (-1 != cmp) return cmp;

      colgen->_template->dvalue += direction;
    }
    colgen->_template->dvalue = temp_dvalue;
    ceiling = -1;
    direction *= -1;
  } while (0 < direction); //exits at second loop

  return -1; //can't assign new values
}

int
dbgen_create_dtag(t_colgen* colgen, t_ukey* ukey, t_colgen* dependency)
{
  ++ukey->n_amt;
  ukey->storage = realloc(ukey->storage, ukey->n_amt * sizeof *ukey->storage);
  ukey->storage[ukey->n_amt-1] = dbgen_build_linkstorage(dependency, colgen, dbgen_init_linkstorage()); 


  linkstorage* key = ukey->storage[ukey->n_amt-1];
  int i = ukey->n_amt-1;
  while ((0 < i) && (0 > dcmp(key->tag.dvalue, ukey->storage[i-1]->tag.dvalue))){
    ukey->storage[i] = ukey->storage[i-1];
    --i;
  }
  
  ukey->storage[i] = key;
  
  return i;
}

int
dbgen_create_stag(t_colgen* colgen, t_ukey* ukey, t_colgen* dependency)
{
  ++ukey->n_amt;
  ukey->storage = realloc(ukey->storage, ukey->n_amt * sizeof *ukey->storage);
  ukey->storage[ukey->n_amt-1] = dbgen_build_linkstorage(dependency, colgen, dbgen_init_linkstorage()); 


  linkstorage* key = ukey->storage[ukey->n_amt-1];
  int i = ukey->n_amt-1;
  while ((0 < i) && (0 > strcmp(key->tag.svalue, ukey->storage[i-1]->tag.svalue))){
    ukey->storage[i] = ukey->storage[i-1];
    --i;
  }
  
  ukey->storage[i] = key;
  
  return i;
}

int
dbgen_search_or_create_dtag(
  t_colgen* colgen, 
  t_ukey* ukey, 
  t_colgen* dependency, 
  double key_dvalue )
{
  //check and return tag if already exists
  int top = ukey->n_amt-1;
  int low = 0;
  int mid;
  while (low <= top){
    mid = ((unsigned int)low + (unsigned int)top) >> 1;
    double cmp = dcmp(key_dvalue, ukey->storage[mid]->tag.dvalue);

    if (0 == cmp) return mid;

    if (0 > cmp){
      top = mid-1;
    } else {
      low = mid+1;
    }
  }

  //otherwise create new tag and return it 
  return dbgen_create_dtag(colgen, ukey, dependency);
}

int
dbgen_search_or_create_stag(t_colgen* colgen, t_ukey* ukey, t_colgen* dependency, char* key_svalue)
{
  //check and return tag if already exists
  int top = ukey->n_amt-1;
  int low = 0;
  int mid;
  while (low <= top){
    mid = ((unsigned int)low + (unsigned int)top) >> 1;
    double cmp = strcmp(key_svalue, ukey->storage[mid]->tag.svalue);

    if (0 == cmp) return mid;

    if (0 > cmp){
      top = mid-1;
    } else {
      low = mid+1;
    }
  }

  //otherwise create new tag and return it 
  return dbgen_create_stag(colgen, ukey, dependency);
}

void
dbgen_tukey_tdependency(t_colgen* colgen, dbconfig* database)
{
  assert(NULL != colgen);

  t_ukey *ukey = colgen->_ukey;
  t_colgen *dependency = colgen->_dependency;

  char str[50] = ""; //holds entity for printing

  int i = dbgen_search_or_create_dtag(
            colgen,
            ukey, 
            dependency, 
            dependency->_template->dvalue); //key_dvalue

  int j = dbgen_create_xtvalue(
              colgen, 
              ukey->storage[i],
              &dbgen_search_or_create_dstorage);

  if (-1 == j){
    strcpy(str, "(NULL)");
  } else {
    sprintf(str, colgen->format_entity, ukey->storage[i]->store_dvalue[j], colgen->decimal_places);
  }

  fprintf(database->out_stream, "%s%c", str, colgen->delim);
}

void
dbgen_tukey_dldependency(t_colgen* colgen, dbconfig* database)
{
  assert(NULL != colgen);

  t_ukey *ukey = colgen->_ukey;
  t_colgen *dependency = colgen->_dependency;

  char str[50] = ""; //holds entity for printing

  int i = dbgen_search_or_create_dtag(
            colgen,
            ukey, 
            dependency, 
            dependency->_curr_entity->dvalue); //key_dvalue

  int j = dbgen_create_xtvalue(
              colgen, 
              ukey->storage[i],
              &dbgen_search_or_create_dstorage);

  if (-1 == j){
    strcpy(str, "(NULL)");
  } else {
    sprintf(str, colgen->format_entity, ukey->storage[i]->store_dvalue[j], colgen->decimal_places);
  }

  fprintf(database->out_stream, "%s%c", str, colgen->delim);
}

void 
dbgen_tukey_sldependency(t_colgen* colgen, dbconfig* database)
{
  assert(NULL != colgen);

  t_ukey *ukey = colgen->_ukey;
  t_colgen *dependency = colgen->_dependency;

  char str[50] = ""; //holds entity for printing

  int i = dbgen_search_or_create_stag(
            colgen,
            ukey, 
            dependency, 
            dependency->_curr_entity->svalue); //key_svalue

  int j = dbgen_create_xtvalue(
              colgen, 
              ukey->storage[i],
              &dbgen_search_or_create_dstorage);

  if (-1 == j){
    strcpy(str, "(NULL)");
  } else {
    sprintf(str, colgen->format_entity, ukey->storage[i]->store_dvalue[j], colgen->decimal_places);
  }

  fprintf(database->out_stream, "%s%c", str, colgen->delim);
}

void 
dbgen_dlukey_tdependency(t_colgen* colgen, dbconfig* database)
{
  assert(NULL != colgen);

  t_ukey *ukey = colgen->_ukey;
  t_colgen *dependency = colgen->_dependency;

  char str[50] = ""; //holds entity for printing

  int i = dbgen_search_or_create_dtag(
            colgen,
            ukey, 
            dependency, 
            dependency->_template->dvalue); //key_dvalue

  int j = dbgen_create_xlvalue(
              colgen, 
              ukey->storage[i],
              &dbgen_search_or_create_dstorage);

  if (-1 == j){
    strcpy(str, "(NULL)");
  } else {
    sprintf(str, colgen->format_entity, ukey->storage[i]->store_dvalue[j], colgen->decimal_places);
  }

  fprintf(database->out_stream, "%s%c", str, colgen->delim);
}

void
dbgen_slukey_tdependency(t_colgen* colgen, dbconfig* database)
{
  assert(NULL != colgen);

  t_ukey *ukey = colgen->_ukey;
  t_colgen *dependency = colgen->_dependency;

  char str[50] = ""; //holds entity for printing

  int i = dbgen_search_or_create_dtag(
            colgen,
            ukey, 
            dependency, 
            dependency->_template->dvalue); //key_svalue

  int j = dbgen_create_xlvalue(
              colgen, 
              ukey->storage[i],
              &dbgen_search_or_create_sstorage);

  if (-1 == j){
    strcpy(str, "(NULL)");
  } else {
    strcpy(str, ukey->storage[i]->store_svalue[j]);
  }

  fprintf(database->out_stream, colgen->format_entity, str, colgen->delim);
}

void
dbgen_dlukey_dldependency(t_colgen* colgen, dbconfig* database)
{
  assert(NULL != colgen);

  t_ukey *ukey = colgen->_ukey;
  t_colgen *dependency = colgen->_dependency;

  char str[50] = ""; //holds entity for printing

  int i = dbgen_search_or_create_dtag(
            colgen,
            ukey, 
            dependency, 
            dependency->_curr_entity->dvalue); //key_dvalue

  int j = dbgen_create_xlvalue(
              colgen, 
              ukey->storage[i],
              &dbgen_search_or_create_dstorage);

  if (-1 == j){
    strcpy(str, "(NULL)");
  } else {
    sprintf(str, colgen->format_entity, ukey->storage[i]->store_dvalue[j], colgen->decimal_places);
  }

  fprintf(database->out_stream, "%s%c", str, colgen->delim);
}

void
dbgen_dlukey_sldependency(t_colgen* colgen, dbconfig* database)
{
  assert(NULL != colgen);

  t_ukey *ukey = colgen->_ukey;
  t_colgen *dependency = colgen->_dependency;

  char str[50] = ""; //holds entity for printing

  int i = dbgen_search_or_create_stag(
            colgen,
            ukey, 
            dependency, 
            dependency->_curr_entity->svalue); //key_svalue

  int j = dbgen_create_xlvalue(
              colgen, 
              ukey->storage[i],
              &dbgen_search_or_create_dstorage);

  if (-1 == j){
    strcpy(str, "(NULL)");
  } else {
    sprintf(str, colgen->format_entity, ukey->storage[i]->store_dvalue[j], colgen->decimal_places);
  }

  fprintf(database->out_stream, "%s%c", str, colgen->delim);
}

void
dbgen_slukey_sldependency(t_colgen* colgen, dbconfig* database)
{
  assert(NULL != colgen);

  t_ukey *ukey = colgen->_ukey;
  t_colgen *dependency = colgen->_dependency;

  char str[50] = ""; //holds entity for printing

  int i = dbgen_search_or_create_stag(
            colgen,
            ukey, 
            dependency, 
            dependency->_curr_entity->svalue); //key_svalue

  int j = dbgen_create_xlvalue(
              colgen, 
              ukey->storage[i],
              &dbgen_search_or_create_sstorage);

  if (-1 == j){
    strcpy(str, "(NULL)");
  } else {
    strcpy(str, ukey->storage[i]->store_svalue[j]);
  }

  fprintf(database->out_stream, colgen->format_entity, str, colgen->delim);
}

/*
* Stores numbers proportionally to the range (last - first) provided, by making use of a 'padding' tool
* ex: 1000 to 2000; amount of elements wanted = 5
*
* pad = range / amount , which translates to 1000/5 = 200
* pad = 200 , means it will decrement from 2000 to 1000, 
* lowering the value of the last element by 200 each run (last -= pad), until last <= first
*
* rand() used in each padding, from a range of last to last+pad makes sure each random number 
* is unique, but also lower than the previous one
*/
static void
dbgen_gen_scalable(t_entity** arrlist, t_colgen* colgen)
{
  double last = colgen->rwall;
  double first = colgen->lwall;
  size_t amount = colgen->amt_row;

  unsigned int pad = (last - first) / amount;

  last -= pad;
  while (amount--){
    arrlist[amount]->dvalue = last + (rand() % pad);
    last -= pad;
  }
}

static void
dbgen_gen_incremental(t_entity** arrlist, t_colgen* colgen)
{
  double first = colgen->lwall;
  size_t amount = colgen->amt_row;

  for (size_t i = 1; i <= amount; ++i){
    arrlist[amount-i]->dvalue = first + (i-1);
  }
}

static void
dbgen_list_swap(t_entity** ptr1, t_entity** ptr2)
{
  assert(ptr1 && ptr1);

  t_entity *temp_ptr = *ptr1;
  *ptr1 = *ptr2;
  *ptr2 = temp_ptr;
}

static void
dbgen_shuffle_arrlist(t_entity** arrlist, t_colgen* colgen)
{
  for ( size_t i = 0 ; i < colgen->amt_row ; ++i ){ //shuffle
    dbgen_list_swap( arrlist + i, arrlist + rand()%colgen->amt_row );
  }
}

//will print from random indexes
void
dbgen_dlist_random(t_colgen* colgen, dbconfig* database)
{
  int i_rand = rand() % colgen->amt_row;

  fprintf(database->out_stream, colgen->format_entity,
      colgen->_list[i_rand]->dvalue,
      colgen->delim);

  colgen->_curr_entity = colgen->_list[i_rand];
}

void
dbgen_slist_random(t_colgen* colgen, dbconfig* database)
{
  int i_rand = rand() % colgen->amt_row;

  fprintf(database->out_stream, colgen->format_entity,
      colgen->_list[i_rand]->svalue,
      colgen->delim);

  colgen->_curr_entity = colgen->_list[i_rand];
}

//will print in order
void
dbgen_dlist_incremental(t_colgen* colgen, dbconfig* database)
{
  //frees current list(one use only) index
  if (0 != colgen->amt_row){
    fprintf(database->out_stream, colgen->format_entity,
        colgen->_list[colgen->amt_row-1]->dvalue,
        colgen->delim);

    free(colgen->_list[colgen->amt_row-1]);
    --colgen->amt_row;
  } else {
    fprintf(database->out_stream, "(NULL)%c",
        colgen->delim
    );
  }
}


void
dbgen_filesetter_arrlist(t_entity** arrlist, t_colgen* colgen)
{
  FILE *f_read = fopen(colgen->file, "r");
  assert(NULL != f_read);

  const int len = 50;

  char ptr_str[len];
  
  size_t i = 1;
  while ((fgets(ptr_str, len-1, f_read)) && (i <= colgen->amt_row)){
    arrlist[colgen->amt_row-i]->svalue = strndup(ptr_str, strlen(ptr_str)-1);
    assert(NULL != arrlist[colgen->amt_row-i]->svalue);

    ++i;
  }

  if (!(colgen->gentype & DBGEN_UNIQUE_KEY)){ // colgen is not ukeyed
    colgen->fn = &dbgen_slist_random;
  } else { // colgen is ukeyed to another colgen
    if (NULL == colgen->_dependency->_curr_entity){ //dependency is incremental
      colgen->fn = &dbgen_slist_random;
    } else if (colgen->_dependency->gentype & DBGEN_FILE) { //dependency is sl (string list )
      colgen->fn = &dbgen_slukey_sldependency;
    } else { // dependency is template
      colgen->fn = &dbgen_slukey_tdependency;
    }
  }

  // if dependency->_curr_entity is null then it's values are incremental,
  // and each value shall only appear once,
  // rendering ukeying unecessary
  colgen->_curr_entity = arrlist[colgen->amt_row-1];

  fclose(f_read);
}

void
dbgen_numsetter_arrlist(t_entity** arrlist, t_colgen* colgen)
{
  if (colgen->method & DBGEN_SCALEABLE){
    dbgen_gen_scalable(arrlist,colgen);
  } else {
    dbgen_gen_incremental(arrlist,colgen);
  }

  if (colgen->method & DBGEN_RANDOM){
    dbgen_shuffle_arrlist(arrlist, colgen);
  }
 
  colgen->_curr_entity = arrlist[colgen->amt_row-1];
  
  if (!(colgen->gentype & DBGEN_UNIQUE_KEY)){ //colgen is not linked
    if (colgen->method & DBGEN_FIXEDSIZE){
      colgen->fn = &dbgen_dlist_random;
    } else {
      colgen->fn = &dbgen_dlist_incremental;
      colgen->_curr_entity = NULL; //is null because each value is free'd after print
    }
  } else { //colgen is linked
    if (colgen->_dependency->gentype & DBGEN_LIST){ //dependency is slist
      if (NULL == colgen->_dependency->_curr_entity){ //dependency is incremental
        colgen->fn = &dbgen_dlist_random;
      } else if (colgen->_dependency->gentype & DBGEN_FILE) {
        colgen->fn = &dbgen_dlukey_sldependency;
      } else {
        colgen->fn = &dbgen_dlukey_dldependency;
      }
    } else { //dependency is template
      colgen->fn = &dbgen_dlukey_tdependency;
    }
  }
}

void
dbgen_templ_scalable(t_colgen* colgen, dbconfig* database)
{
  double first = colgen->lwall;
  double last = colgen->rwall;
  size_t amount = database->amt_row;
  
  if (colgen->_template->dvalue < last){
    unsigned int pad = 2; 
    if (amount < (last-first)) {
      pad = (last - first) / amount;
    }

    colgen->_template->dvalue +=  (rand() % pad) + 1;

    if (colgen->_template->dvalue > last){
      colgen->_template->dvalue = last;
    }
  }

  fprintf(database->out_stream, colgen->format_entity,
      colgen->_template->dvalue,
      colgen->delim);
}

void
dbgen_templ_incremental(t_colgen* colgen, dbconfig* database)
{
  double last = colgen->rwall;

  if (colgen->_template->dvalue < last){
      ++colgen->_template->dvalue;
  }

  fprintf(database->out_stream, colgen->format_entity,
      colgen->_template->dvalue,
      colgen->delim);
}

void
dbgen_templ_random(t_colgen* colgen, dbconfig* database)
{
  double first = colgen->lwall;
  double last = colgen->rwall;
  
  size_t rnd = rand() % (size_t)(last - first);
  colgen->_template->dvalue = first + rnd; 

  fprintf(database->out_stream, colgen->format_entity,
      colgen->_template->dvalue,
      colgen->delim);
}

void
dbgen_numsetter_templ(t_colgen* colgen, dbconfig* database)
{
  if (!(colgen->gentype & DBGEN_UNIQUE_KEY)){
    if (colgen->method & DBGEN_RANDOM){
      colgen->fn = &dbgen_templ_random;
    } else if (colgen->method & DBGEN_SCALEABLE) {
      colgen->fn = &dbgen_templ_scalable;
    } else {
      colgen->fn = &dbgen_templ_incremental;
    }
  } else {
    if (colgen->_dependency->gentype & DBGEN_LIST){ //dependency is slist
      if (NULL == colgen->_dependency->_curr_entity){ //dependency is incremental
        colgen->fn = &dbgen_templ_random;
      } else if (colgen->_dependency->gentype & DBGEN_FILE) {
        colgen->fn = &dbgen_tukey_sldependency;
      } else {
        colgen->fn = &dbgen_tukey_dldependency;
      }
    } else { //dependency is template
      colgen->fn = &dbgen_tukey_tdependency;
    }
  }
}

void
dbgen_generate_database(t_colgen** arrcolgen, dbconfig* database)
{
  for (unsigned int i = 0; i < database->amt_row; ++i){
    for (unsigned short j = 0; j < database->amt_col; ++j){
      if (NULL != arrcolgen[j]->fn){
          (arrcolgen[j]->fn)(arrcolgen[j], database);
      }
    }
  }
}
