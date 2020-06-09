#include "../config.h"
#include "../dbgen.h"

enum gentype {
    Template=1, 
    List=2,
    File=4,
    Link=8
};

enum methodtype { 
    Rnd=1, 
    Unq=2, 
    Scl=4, 
    Fix=8
};

int create_sstorage(linkstorage* storage, t_data* list)
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

int search_or_create_sstorage(linkstorage* storage, t_data* list)
{
    int top = storage->n_amt-1;
    int low = 0;
    int mid;
    while ( low <= top ){
        mid = ((unsigned int)low + (unsigned int)top) >> 1;
        int cmp = strcmp(list->svalue, storage->store_svalue[mid]);
        if ( cmp == 0 )
            return -1;
        if ( cmp < 0 )
            top = mid-1;
        else
            low = mid+1;
    }

    return create_sstorage(storage, list);
}


int create_xvalue(t_colgen* colgen, linkstorage* storage)
{
    assert(storage);

    int i = rand() % colgen->amt_row;
    int temp_i = i;

    short direction = 1;
    int ceiling = colgen->amt_row;
    
    short loop_count = 0;
    do {
        int cmp;
        while ( i != ceiling ){
            cmp = search_or_create_sstorage(storage, colgen->_list[i]);
            if ( cmp != -1 )
                return cmp;

            i += direction;
        }
        i = temp_i;
        ceiling = -1;
        direction *= -1;
    } while ( ++loop_count < 2 ); //exits loop at 2

    return -1; //can't assign new values
}

int create_stag(t_colgen* colgen, t_link* link, t_colgen* linker)
{
    ++link->n_amt;
    link->storage = realloc(link->storage, link->n_amt * sizeof(linkstorage*));
    link->storage[link->n_amt-1] = start_linkstorage(linker, colgen, init_linkstorage()); 

    linkstorage* key = link->storage[link->n_amt-1];
    int i = link->n_amt-1;
    while (( i > 0 ) && ( strcmp(key->tag.svalue, link->storage[i-1]->tag.svalue) < 0 )){
        link->storage[i] = link->storage[i-1];
        --i;
    } link->storage[i] = key;
    
    return i;
}

int search_or_create_stag(
    t_colgen* colgen, 
    t_link* link, 
    t_colgen* linker, 
    char* key_svalue )
{
    //check and return tag if already exists
    int top = link->n_amt-1;
    int low = 0;
    int mid;
    while ( low <= top ){
        mid = ((unsigned int)low + (unsigned int)top) >> 1;
        int cmp = strcmp(key_svalue, link->storage[mid]->tag.svalue);
        if ( cmp == 0 )
            return mid;
        if ( cmp < 0 )
            top = mid-1;
        else
            low = mid+1;
    }

    //otherwise create new tag and return it 
    return create_stag(colgen, link, linker);
}


void sllink_sllinker(t_colgen* colgen, dbconfig* database)
{
    assert(colgen);

    t_link *link = colgen->_link;
    t_colgen *linker = colgen->_linker;

    char slist[50] = { 0 };

    int i = search_or_create_stag(
      colgen,
      link, 
      linker, 
      linker->_lindex->svalue //key_svalue
    );

    int j = create_xvalue(colgen, link->storage[i]);
    if ( j != -1 )
        strcpy(slist, link->storage[i]->store_svalue[j]);
    else
        strcpy(slist, "(NULL)");


    fprintf(database->out_stream, colgen->format_data,
        slist,
        colgen->delim
    );
}

/*
 * Stores numbers proportionally to the scope (last - first) provided, by making use of a 'padding' tool
 * ex: 1000 to 2000; amount of elements wanted = 5
 *
 * pad = scope / amount , which translates to 1000/5 = 200
 * pad = 200 , means it will decrement from 2000 to 1000, 
 * lowering the value of the last element by 200 each run (last -= pad), until last <= first
 *
 * rand() used in each padding, from a range of last to last+pad makes sure each random number 
 * is unique, but also lower than the previous one
 */
static void gen_scalable(t_data** arrlist, t_colgen* colgen)
{
    double last = colgen->rwall;
    double first = colgen->lwall;
    size_t amount = colgen->amt_row;

    unsigned int pad = (last - first) / amount;

    last -= pad;
    while ( amount-- ){
        arrlist[amount]->dvalue = last + (rand() % pad);
        last -= arrlist[amount]->dvalue - 1;
    }
}

static void gen_incremental(t_data** arrlist, t_colgen* colgen)
{
    double first = colgen->lwall;
    size_t amount = colgen->amt_row;

    for ( size_t i = 1; i <= amount; ++i )
        arrlist[amount-i]->dvalue = first + (i-1);
}

static void list_swap(t_data** ptr1, t_data** ptr2)
{
    assert( ptr1 && ptr1 );

    t_data *temp_ptr = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = temp_ptr;
}

static void shuffle_arrlist(t_data** arrlist, t_colgen* colgen)
{
    for ( size_t i = 0 ; i < colgen->amt_row ; ++i ){ //shuffle
        list_swap( arrlist + i, arrlist + rand()%colgen->amt_row );
    }
}

//will print from random indexes
void dlist_random(t_colgen* colgen, dbconfig* database)
{
    int i_rand = rand() % colgen->amt_row;

    fprintf(database->out_stream, colgen->format_data,
        colgen->_list[i_rand]->dvalue,
        colgen->delim
    );

    colgen->_lindex = colgen->_list[i_rand];
}

void slist_random(t_colgen* colgen, dbconfig* database)
{
    int i_rand = rand() % colgen->amt_row;

    fprintf(database->out_stream, colgen->format_data,
        colgen->_list[i_rand]->svalue,
        colgen->delim
    );

    colgen->_lindex = colgen->_list[i_rand];
}

//will print in order
void dlist_incremental(t_colgen* colgen, dbconfig* database)
{
    //frees current list(one use only) index
    if ( colgen->amt_row ){
        fprintf(database->out_stream, colgen->format_data,
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


void filesetter_arrlist(t_data** arrlist, t_colgen* colgen)
{
    FILE *f_read = fopen(colgen->file, "r");
    assert(f_read);

    const int LEN = 50;

    char ptr_str[LEN];
    
    size_t i = 1;
    while ( (fgets(ptr_str, LEN-1, f_read)) && (i <= colgen->amt_row) ){
        arrlist[colgen->amt_row-i]->svalue = strndup(ptr_str, strlen(ptr_str)-1);
        assert(arrlist[colgen->amt_row-i]->svalue);

        ++i;
    }

    // if _lindex is null then it's values are incremental,
    // and each value shall only appear once,
    // making linking impossible
    if ( colgen->gentype & Link ){
        if ( colgen->_linker->gentype & List ){
            if ( colgen->_linker->gentype & File )
                colgen->fn = &sllink_sllinker;
            /*else
                colgen->fn = &sllink_dllinker;
            */
        } else { // Linker is template
            //colgen->fn = &sllink_tlinker;
        }
    } else {
        colgen->fn = &slist_random;
    }

    colgen->_lindex = arrlist[colgen->amt_row-1];

    fclose(f_read);
}

void numsetter_arrlist(t_data** arrlist, t_colgen* colgen)
{
    if ( colgen->method & Scl )
        gen_scalable(arrlist,colgen);
    else
        gen_incremental(arrlist,colgen);

    //no need to shuffle if method has Fixed elements or is Linked
    if (( colgen->method & Rnd ) && !( colgen->method & ( Fix | Link )))
        shuffle_arrlist(arrlist, colgen);
   
    colgen->_lindex = arrlist[colgen->amt_row-1];
    /*
    if ( colgen->gentype & Link ) 
        colgen->fn = &dlist_link;
    else*/if ( colgen->method & Fix )
        colgen->fn = &dlist_random;
    else {
        colgen->fn = &dlist_incremental;
        colgen->_lindex = NULL;
        //no need to hold onto the value, since each will appear only once
    }
}

void templ_scalable(t_colgen* colgen, dbconfig* database)
{
    double first = colgen->lwall;
    double last = colgen->rwall;
    size_t amount = database->amt_rows;
    
    if ( colgen->_template->dvalue < last ){
        unsigned int pad = 2; 
        if ( amount < (last-first) ) {
            pad = (last - first) / amount;
        }

        colgen->_template->dvalue +=  rand() % pad;

        if ( colgen->_template->dvalue > last )
            colgen->_template->dvalue = last;
    }

    fprintf(database->out_stream, colgen->format_data,
        colgen->_template->dvalue,
        colgen->delim
    );
}

void templ_incremental(t_colgen* colgen, dbconfig* database)
{
    double last = colgen->rwall;

    if ( colgen->_template->dvalue < last )
        ++colgen->_template->dvalue;

    fprintf(database->out_stream, colgen->format_data,
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

    fprintf(database->out_stream, colgen->format_data,
        colgen->_template->dvalue,
        colgen->delim
    );
}

void numsetter_templ(t_colgen* colgen, dbconfig* database)
{
    if ( colgen->method & Rnd )
        colgen->fn = &templ_random;
    else if ( colgen->method & Scl )
        colgen->fn = &templ_scalable;
    else
        colgen->fn = &templ_incremental;
}
