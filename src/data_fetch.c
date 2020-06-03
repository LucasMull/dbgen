#include "../config.h"
#include "../dbgen.h"

enum gentype {
    Template=1, 
    List=2,
    File=4,

    TotalGens=3
};

enum methodtype { 
    Rnd=1, 
    Unq=2, 
    Scl=4, 

    TotalMethods=3,
    Undef=0
};

//will print from random indexes
void list_random(t_colgen* colgen, dbconfig* database)
{
    fprintf(database->out_stream, colgen->format_data,
        colgen->_list[rand()%colgen->amt_row]->svalue,
        colgen->delim
    );
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

/*
 * Read file and store each line as a string value of index i in the array
 */
void file_to_arrlist(t_list** arrlist, t_colgen* colgen)
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
    colgen->fn = &list_random;

    fclose(f_read);
}

void list_swap(t_list** ptr1, t_list** ptr2)
{
    assert( ptr1 && ptr1 );

    t_list *temp_ptr = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = temp_ptr;
}

void shuffle_arrlist(t_list **arrlist, t_colgen* colgen)
{
    for ( size_t i = 0 ; i < colgen->amt_row ; ++i ){ //shuffle
        list_swap( arrlist + i, arrlist + rand()%colgen->amt_row );
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

/*
void filesetter_templ(t_templ* templ, t_colgen* colgen)
{

}
*/

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
void gen_scalable(t_list** arrlist, t_colgen* colgen)
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

void gen_incremental(t_list** arrlist, t_colgen* colgen)
{
    double first = colgen->lwall;
    size_t amount = colgen->amt_row;

    for ( size_t i = 1; i <= amount; ++i )
        arrlist[amount-i]->dvalue = first + (i-1);
}

void nums_to_arrlist(t_list** arrlist, t_colgen* colgen)
{
        if ( colgen->method & Scl ){
            gen_scalable(arrlist,colgen);
        } else {
            gen_incremental(arrlist,colgen);
        }

        if ( colgen->method & Rnd )
            shuffle_arrlist(arrlist, colgen);

        colgen->fn = &list_incremental;
}
