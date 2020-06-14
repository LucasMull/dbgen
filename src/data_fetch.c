#include "../config.h"
#include "../dbgen.h"

enum gentype {
    Template=1, 
    List=2,
    File=4,
    UniqueKey=8
};

enum methodtype { 
    Rnd=1, 
    Unq=2, 
    Scl=4, 
    Fix=8
};

double dcmp(const double num1, const double num2)
{
    return num1 - num2;
}

int create_dstorage(linkstorage* storage, t_entity* list)
{
    ++storage->n_amt;
    storage->store_dvalue = realloc(storage->store_dvalue, storage->n_amt * sizeof(char*));

    storage->store_dvalue[storage->n_amt-1] = list->dvalue;

    double key = storage->store_dvalue[storage->n_amt-1];
    int i = storage->n_amt-1;
    while (( i > 0 ) && ( dcmp(key, storage->store_dvalue[i-1]) < 0 )){
        storage->store_dvalue[i] = storage->store_dvalue[i-1];
        --i;
    } storage->store_dvalue[i] = key;

    return i;
}

int create_sstorage(linkstorage* storage, t_entity* list)
{
    ++storage->n_amt;
    storage->store_svalue = realloc(storage->store_svalue, storage->n_amt * sizeof(char*));

    storage->store_svalue[storage->n_amt-1] = list->svalue;

    char* key = storage->store_svalue[storage->n_amt-1];
    int i = storage->n_amt-1;
    while (( i > 0 ) && ( strcmp(key, storage->store_svalue[i-1]) < 0 )){
        storage->store_svalue[i] = storage->store_svalue[i-1];
        --i;
    } storage->store_svalue[i] = key;

    return i;
}

int search_or_create_dstorage(linkstorage* storage, t_entity* list)
{
    int top = storage->n_amt-1;
    int low = 0;
    int mid;
    while ( low <= top ){
        mid = ((unsigned int)low + (unsigned int)top) >> 1;
        double cmp = dcmp(list->dvalue, storage->store_dvalue[mid]);
        if ( cmp == 0 )
            return -1; //can't ukey that value if it already exists
        if ( cmp < 0 )
            top = mid-1;
        else
            low = mid+1;
    }

    return create_dstorage(storage, list);
}

int search_or_create_sstorage(linkstorage* storage, t_entity* list)
{
    int top = storage->n_amt-1;
    int low = 0;
    int mid;
    while ( low <= top ){
        mid = ((unsigned int)low + (unsigned int)top) >> 1;
        double cmp = strcmp(list->svalue, storage->store_svalue[mid]);
        if ( cmp == 0 )
            return -1; //can't ukey that value if it already exists
        if ( cmp < 0 )
            top = mid-1;
        else
            low = mid+1;
    }

    return create_sstorage(storage, list);
}
//create exclusive list based value
int create_xlvalue(
    t_colgen* colgen, 
    linkstorage* storage,
    int (*storage_fn)(linkstorage*, t_entity*) )
{
    assert(storage);

    int i = rand() % colgen->amt_row;
    int temp_i = i;
    
    int ceiling = colgen->amt_row;
    short direction = 1;
    do {
        int cmp;
        while ( i != ceiling ){
            cmp = (storage_fn)(storage, colgen->_list[i]);
            if ( cmp != -1 )
                return cmp;
            i += direction;
        }
        i = temp_i;
        ceiling = -1;
        direction *= -1;
    } while ( direction > 0 ); //exits at second loop

    return -1; //can't assign new values
}
//create exclusive template based value
int create_xtvalue(
    t_colgen* colgen, 
    linkstorage* storage,
    int (*storage_fn)(linkstorage*, t_entity*) )
{
    assert(storage);
    
    size_t rnd = rand() % (size_t)(colgen->rwall - colgen->lwall);
    colgen->_template->dvalue = colgen->lwall + rnd; 
    double temp_dvalue = colgen->_template->dvalue;
    
    double ceiling = colgen->rwall;
    short direction = 1;
    do {
        int cmp;
        while ( colgen->_template->dvalue != ceiling ){
            cmp = (storage_fn)(storage, colgen->_template);
            if ( cmp != -1 )
                return cmp;
            colgen->_template->dvalue += direction;
        }
        colgen->_template->dvalue = temp_dvalue;
        ceiling = -1;
        direction *= -1;
    } while ( direction > 0 ); //exits at second loop

    return -1; //can't assign new values
}

int create_dtag(t_colgen* colgen, t_ukey* ukey, t_colgen* dependency)
{
    ++ukey->n_amt;
    ukey->storage = realloc(ukey->storage, ukey->n_amt * sizeof(linkstorage*));
    ukey->storage[ukey->n_amt-1] = start_linkstorage(dependency, colgen, init_linkstorage()); 


    linkstorage* key = ukey->storage[ukey->n_amt-1];
    int i = ukey->n_amt-1;
    while (( i > 0 ) && ( dcmp(key->tag.dvalue, ukey->storage[i-1]->tag.dvalue) < 0 )){
        ukey->storage[i] = ukey->storage[i-1];
        --i;
    } ukey->storage[i] = key;
    
    return i;
}

int create_stag(t_colgen* colgen, t_ukey* ukey, t_colgen* dependency)
{
    ++ukey->n_amt;
    ukey->storage = realloc(ukey->storage, ukey->n_amt * sizeof(linkstorage*));
    ukey->storage[ukey->n_amt-1] = start_linkstorage(dependency, colgen, init_linkstorage()); 


    linkstorage* key = ukey->storage[ukey->n_amt-1];
    int i = ukey->n_amt-1;
    while (( i > 0 ) && ( strcmp(key->tag.svalue, ukey->storage[i-1]->tag.svalue) < 0 )){
        ukey->storage[i] = ukey->storage[i-1];
        --i;
    } ukey->storage[i] = key;
    
    return i;
}

int search_or_create_dtag(
    t_colgen* colgen, 
    t_ukey* ukey, 
    t_colgen* dependency, 
    double key_dvalue )
{
    //check and return tag if already exists
    int top = ukey->n_amt-1;
    int low = 0;
    int mid;
    while ( low <= top ){
        mid = ((unsigned int)low + (unsigned int)top) >> 1;
        double cmp = dcmp(key_dvalue, ukey->storage[mid]->tag.dvalue);
        if ( cmp == 0 )
            return mid;
        if ( cmp < 0 )
            top = mid-1;
        else
            low = mid+1;
    }

    //otherwise create new tag and return it 
    return create_dtag(colgen, ukey, dependency);
}

int search_or_create_stag(
    t_colgen* colgen, 
    t_ukey* ukey, 
    t_colgen* dependency, 
    char* key_svalue )
{
    //check and return tag if already exists
    int top = ukey->n_amt-1;
    int low = 0;
    int mid;
    while ( low <= top ){
        mid = ((unsigned int)low + (unsigned int)top) >> 1;
        double cmp = strcmp(key_svalue, ukey->storage[mid]->tag.svalue);
        if ( cmp == 0 )
            return mid;
        if ( cmp < 0 )
            top = mid-1;
        else
            low = mid+1;
    }

    //otherwise create new tag and return it 
    return create_stag(colgen, ukey, dependency);
}

void tukey_tdependency(t_colgen* colgen, dbconfig* database)
{
    assert(colgen);

    t_ukey *ukey = colgen->_ukey;
    t_colgen *dependency = colgen->_dependency;

    char str[50] = { 0 }; //holds entity for printing

    int i = search_or_create_dtag(
              colgen,
              ukey, 
              dependency, 
              dependency->_template->dvalue); //key_dvalue

    int j = create_xtvalue(
                colgen, 
                ukey->storage[i],
                &search_or_create_dstorage);

    if ( j == -1 )
        strcpy(str, "(NULL)");
    else
        sprintf(str, colgen->format_entity, ukey->storage[i]->store_dvalue[j], colgen->decimal_places);


    fprintf(database->out_stream, "%s%c",
        str,
        colgen->delim
    );
}

void tukey_dldependency(t_colgen* colgen, dbconfig* database)
{
    assert(colgen);

    t_ukey *ukey = colgen->_ukey;
    t_colgen *dependency = colgen->_dependency;

    char str[50] = { 0 }; //holds entity for printing

    int i = search_or_create_dtag(
              colgen,
              ukey, 
              dependency, 
              dependency->_curr_entity->dvalue); //key_dvalue

    int j = create_xtvalue(
                colgen, 
                ukey->storage[i],
                &search_or_create_dstorage);

    if ( j == -1 )
        strcpy(str, "(NULL)");
    else
        sprintf(str, colgen->format_entity, ukey->storage[i]->store_dvalue[j], colgen->decimal_places);


    fprintf(database->out_stream, "%s%c",
        str,
        colgen->delim
    );
}

void tukey_sldependency(t_colgen* colgen, dbconfig* database)
{
    assert(colgen);

    t_ukey *ukey = colgen->_ukey;
    t_colgen *dependency = colgen->_dependency;

    char str[50] = { 0 }; //holds entity for printing

    int i = search_or_create_stag(
              colgen,
              ukey, 
              dependency, 
              dependency->_curr_entity->svalue); //key_svalue

    int j = create_xtvalue(
                colgen, 
                ukey->storage[i],
                &search_or_create_dstorage);

    if ( j == -1 )
        strcpy(str, "(NULL)");
    else
        sprintf(str, colgen->format_entity, ukey->storage[i]->store_dvalue[j], colgen->decimal_places);


    fprintf(database->out_stream, "%s%c",
        str,
        colgen->delim
    );
}

void dlukey_tdependency(t_colgen* colgen, dbconfig* database)
{
    assert(colgen);

    t_ukey *ukey = colgen->_ukey;
    t_colgen *dependency = colgen->_dependency;

    char str[50] = { 0 }; //holds entity for printing

    int i = search_or_create_dtag(
              colgen,
              ukey, 
              dependency, 
              dependency->_template->dvalue); //key_dvalue

    int j = create_xlvalue(
                colgen, 
                ukey->storage[i],
                &search_or_create_dstorage);

    if ( j == -1 )
        strcpy(str, "(NULL)");
    else
        sprintf(str, colgen->format_entity, ukey->storage[i]->store_dvalue[j], colgen->decimal_places);


    fprintf(database->out_stream, "%s%c",
        str,
        colgen->delim
    );
}

void slukey_tdependency(t_colgen* colgen, dbconfig* database)
{
    assert(colgen);

    t_ukey *ukey = colgen->_ukey;
    t_colgen *dependency = colgen->_dependency;

    char str[50] = { 0 }; //holds entity for printing

    int i = search_or_create_dtag(
              colgen,
              ukey, 
              dependency, 
              dependency->_template->dvalue); //key_svalue

    int j = create_xlvalue(
                colgen, 
                ukey->storage[i],
                &search_or_create_sstorage);

    if ( j == -1 )
        strcpy(str, "(NULL)");
    else
        strcpy(str, ukey->storage[i]->store_svalue[j]);


    fprintf(database->out_stream, colgen->format_entity,
        str,
        colgen->delim
    );
}

void dlukey_dldependency(t_colgen* colgen, dbconfig* database)
{
    assert(colgen);

    t_ukey *ukey = colgen->_ukey;
    t_colgen *dependency = colgen->_dependency;

    char str[50] = { 0 }; //holds entity for printing

    int i = search_or_create_dtag(
              colgen,
              ukey, 
              dependency, 
              dependency->_curr_entity->dvalue); //key_dvalue

    int j = create_xlvalue(
                colgen, 
                ukey->storage[i],
                &search_or_create_dstorage);

    if ( j == -1 )
        strcpy(str, "(NULL)");
    else
        sprintf(str, colgen->format_entity, ukey->storage[i]->store_dvalue[j], colgen->decimal_places);


    fprintf(database->out_stream, "%s%c",
        str,
        colgen->delim
    );
}

void dlukey_sldependency(t_colgen* colgen, dbconfig* database)
{
    assert(colgen);

    t_ukey *ukey = colgen->_ukey;
    t_colgen *dependency = colgen->_dependency;

    char str[50] = { 0 }; //holds entity for printing

    int i = search_or_create_stag(
              colgen,
              ukey, 
              dependency, 
              dependency->_curr_entity->svalue); //key_svalue

    int j = create_xlvalue(
                colgen, 
                ukey->storage[i],
                &search_or_create_dstorage);

    if ( j == -1 )
        strcpy(str, "(NULL)");
    else
        sprintf(str, colgen->format_entity, ukey->storage[i]->store_dvalue[j], colgen->decimal_places);


    fprintf(database->out_stream, "%s%c",
        str,
        colgen->delim
    );
}

void slukey_sldependency(t_colgen* colgen, dbconfig* database)
{
    assert(colgen);

    t_ukey *ukey = colgen->_ukey;
    t_colgen *dependency = colgen->_dependency;

    char str[50] = { 0 }; //holds entity for printing

    int i = search_or_create_stag(
              colgen,
              ukey, 
              dependency, 
              dependency->_curr_entity->svalue); //key_svalue

    int j = create_xlvalue(
                colgen, 
                ukey->storage[i],
                &search_or_create_sstorage);

    if ( j == -1 )
        strcpy(str, "(NULL)");
    else
        strcpy(str, ukey->storage[i]->store_svalue[j]);


    fprintf(database->out_stream, colgen->format_entity,
        str,
        colgen->delim
    );
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
static void gen_scalable(t_entity** arrlist, t_colgen* colgen)
{
    double last = colgen->rwall;
    double first = colgen->lwall;
    size_t amount = colgen->amt_row;

    unsigned int pad = (last - first) / amount;

    last -= pad;
    while ( amount-- ){
        arrlist[amount]->dvalue = last + (rand() % pad);
        last -= pad; // fix: might repeat values
    }
}

static void gen_incremental(t_entity** arrlist, t_colgen* colgen)
{
    double first = colgen->lwall;
    size_t amount = colgen->amt_row;

    for ( size_t i = 1; i <= amount; ++i )
        arrlist[amount-i]->dvalue = first + (i-1);
}

static void list_swap(t_entity** ptr1, t_entity** ptr2)
{
    assert( ptr1 && ptr1 );

    t_entity *temp_ptr = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = temp_ptr;
}

static void shuffle_arrlist(t_entity** arrlist, t_colgen* colgen)
{
    for ( size_t i = 0 ; i < colgen->amt_row ; ++i ){ //shuffle
        list_swap( arrlist + i, arrlist + rand()%colgen->amt_row );
    }
}

//will print from random indexes
void dlist_random(t_colgen* colgen, dbconfig* database)
{
    int i_rand = rand() % colgen->amt_row;

    fprintf(database->out_stream, colgen->format_entity,
        colgen->_list[i_rand]->dvalue,
        colgen->delim
    );

    colgen->_curr_entity = colgen->_list[i_rand];
}

void slist_random(t_colgen* colgen, dbconfig* database)
{
    int i_rand = rand() % colgen->amt_row;

    fprintf(database->out_stream, colgen->format_entity,
        colgen->_list[i_rand]->svalue,
        colgen->delim
    );

    colgen->_curr_entity = colgen->_list[i_rand];
}

//will print in order
void dlist_incremental(t_colgen* colgen, dbconfig* database)
{
    //frees current list(one use only) index
    if ( colgen->amt_row ){
        fprintf(database->out_stream, colgen->format_entity,
            colgen->_list[colgen->amt_row-1]->dvalue,
            colgen->delim
        );

        free( colgen->_list[colgen->amt_row-1] );
        --colgen->amt_row;
    } else {
        fprintf(database->out_stream, "(NULL)%c",
            colgen->delim
        );
    }
}


void filesetter_arrlist(t_entity** arrlist, t_colgen* colgen)
{
    FILE *f_read = fopen(colgen->file, "r");
    assert(f_read);

    const int len = 50;

    char ptr_str[len];
    
    size_t i = 1;
    while ( (fgets(ptr_str, len-1, f_read)) && (i <= colgen->amt_row) ){
        arrlist[colgen->amt_row-i]->svalue = strndup(ptr_str, strlen(ptr_str)-1);
        assert(arrlist[colgen->amt_row-i]->svalue);

        ++i;
    }

    if ( !( colgen->gentype & UniqueKey ) ){ // colgen is not ukeyed
        colgen->fn = &slist_random;
    } else { // colgen is ukeyed to another colgen
        if ( !colgen->_dependency->_curr_entity ) //dependency is incremental
            colgen->fn = &slist_random;
        else if ( colgen->_dependency->gentype & File ) //dependency is sl (string list )
                colgen->fn = &slukey_sldependency;
        else // dependency is template
            colgen->fn = &slukey_tdependency;
    }

    // if dependency->_curr_entity is null then it's values are incremental,
    // and each value shall only appear once,
    // rendering ukeying unecessary

    colgen->_curr_entity = arrlist[colgen->amt_row-1];

    fclose(f_read);
}

void numsetter_arrlist(t_entity** arrlist, t_colgen* colgen)
{
    if ( colgen->method & Scl )
        gen_scalable(arrlist,colgen);
    else 
        gen_incremental(arrlist,colgen);

    if ( colgen->method & Rnd )
        shuffle_arrlist(arrlist, colgen);
   
    colgen->_curr_entity = arrlist[colgen->amt_row-1];
    
    if ( !(colgen->gentype & UniqueKey) ){ //colgen is not linked
        if ( colgen->method & Fix )
            colgen->fn = &dlist_random;
        else {
            colgen->fn = &dlist_incremental;
            colgen->_curr_entity = NULL; //is null because each value is free'd after print
        }
    } else { //colgen is linked
        if ( colgen->_dependency->gentype & List ){ //dependency is slist
            if ( !colgen->_dependency->_curr_entity ) //dependency is incremental
                colgen->fn = &dlist_random;
            else if ( colgen->_dependency->gentype & File )
                colgen->fn = &dlukey_sldependency;
            else
                colgen->fn = &dlukey_dldependency;
        } else { //dependency is template
            colgen->fn = &dlukey_tdependency;
        }
    }
}

void templ_scalable(t_colgen* colgen, dbconfig* database)
{
    double first = colgen->lwall;
    double last = colgen->rwall;
    size_t amount = database->amt_row;
    
    if ( colgen->_template->dvalue < last ){
        unsigned int pad = 2; 
        if ( amount < (last-first) ) {
            pad = (last - first) / amount;
        }

        colgen->_template->dvalue +=  (rand() % pad) + 1;

        if ( colgen->_template->dvalue > last )
            colgen->_template->dvalue = last;
    }

    fprintf(database->out_stream, colgen->format_entity,
        colgen->_template->dvalue,
        colgen->delim
    );
}

void templ_incremental(t_colgen* colgen, dbconfig* database)
{
    double last = colgen->rwall;

    if ( colgen->_template->dvalue < last )
        ++colgen->_template->dvalue;

    fprintf(database->out_stream, colgen->format_entity,
        colgen->_template->dvalue,
        colgen->delim
    );
}

void templ_random(t_colgen* colgen, dbconfig* database)
{
    double first = colgen->lwall;
    double last = colgen->rwall;
    
    size_t rnd = rand() % (size_t)(last - first);
    colgen->_template->dvalue = first + rnd; 

    fprintf(database->out_stream, colgen->format_entity,
        colgen->_template->dvalue,
        colgen->delim
    );
}

void numsetter_templ(t_colgen* colgen, dbconfig* database)
{
    if ( !(colgen->gentype & UniqueKey) ){
        if ( colgen->method & Rnd )
            colgen->fn = &templ_random;
        else if ( colgen->method & Scl )
            colgen->fn = &templ_scalable;
        else
            colgen->fn = &templ_incremental;
    } else {
        if ( colgen->_dependency->gentype & List ){ //dependency is slist
            if ( !colgen->_dependency->_curr_entity ) //dependency is incremental
                colgen->fn = &templ_random;
            else if ( colgen->_dependency->gentype & File )
                colgen->fn = &tukey_sldependency;
            else
                colgen->fn = &tukey_dldependency;
        } else { //dependency is template
            colgen->fn = &tukey_tdependency;
        }
    }
}

void generate_database(t_colgen** arrcolgen, dbconfig* database)
{
    for ( unsigned int i = 0; i < database->amt_row; ++i ){
        for ( unsigned short j = 0; j < database->amt_col; ++j ){
            if ( arrcolgen[j]->fn ){
                (arrcolgen[j]->fn)(arrcolgen[j], database);
            }
        }
    }
}
