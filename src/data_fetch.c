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
        last -= pad;
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
void list_random(t_colgen* colgen, dbconfig* database)
{
    int i_rand = rand() % colgen->amt_row;

    fprintf(database->out_stream, colgen->format_data,
        colgen->_list[i_rand]->dvalue,
        colgen->delim
    );

    colgen->_lindex = colgen->_list[i_rand];
}

void flist_random(t_colgen* colgen, dbconfig* database)
{
    int i_rand = rand() % colgen->amt_row;

    fprintf(database->out_stream, colgen->format_data,
        colgen->_list[i_rand]->svalue,
        colgen->delim
    );

    colgen->_lindex = colgen->_list[i_rand];
}

//will print in order
void list_incremental(t_colgen* colgen, dbconfig* database)
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

//turn this into binary search
unsigned int search_tag(t_link* link, t_colgen* linker)
{
    t_data *curr_data = linker->_lindex; // current data output by linker

    if ( linker->gentype & File ){
        for ( unsigned int i = 0; i < link->n_amt; ++i ){
            if ( !strcmp(curr_data->svalue, link->storage[i]->tag.svalue) )
                return i;
        }
    } else {
        for ( unsigned int i = 0; i < link->n_amt; ++i ){
            if ( curr_data->dvalue == link->storage[i]->tag.dvalue )
                return i;
        }
    }

    return -1;
}

//fix: split into various functions instead of a bunch of if elses
void flist_link(t_colgen* colgen, dbconfig* database)
{
    assert(colgen);

    t_link *link = colgen->_link;
    t_colgen *linker = colgen->_linker;

    unsigned int i_found = search_tag(link, linker);;
    if ( i_found != -1 ){
        fprintf(database->out_stream, colgen->format_data,
            colgen->_list[colgen->amt_row-1]->svalue,
            colgen->delim
        );
    } else { // means it has not been found, create new tag
        ++link->n_amt;
        link->storage = realloc(link->storage, link->n_amt * sizeof(linkstorage*));
        link->storage[link->n_amt-1] = start_linkstorage(linker, colgen, init_linkstorage()); 
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
    if (( colgen->gentype & Link ) && ( colgen->_linker->_lindex ))
        colgen->fn = &flist_link;
    else
        colgen->fn = &flist_random;

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
        colgen->fn = &list_link;
    else*/if ( colgen->method & Fix )
        colgen->fn = &list_random;
    else {
        colgen->fn = &list_incremental;
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
