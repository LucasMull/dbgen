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
    Fix=8,
    Lnk=16,

    Undef=0
};

t_entity *init_entity()
{
    t_entity *new_entity = malloc(sizeof(t_entity));
    assert(new_entity);

    const t_entity default_entity = { NULL };
    *new_entity = default_entity;

    return new_entity;
}

void start_arrlist(t_colgen *colgen, dbconfig *database)
{
    assert(!colgen->_list);

    t_entity **new_arrlist = malloc(colgen->amt_row * sizeof(t_entity*));
    assert(new_arrlist);

    for ( int i = 0; i < colgen->amt_row; ++i )
        new_arrlist[i] = init_entity();

    colgen->_list = new_arrlist;
}

void destroy_list(t_entity* list, t_colgen* colgen)
{
    assert(list);

    short gentype = colgen->gentype;

    if (( gentype & File ) && ( list->svalue )){
        free(list->svalue);
    } free(list);
}

void destroy_arrlist(t_entity** arrlist, t_colgen* colgen)
{
    assert(arrlist);

    for ( int i = 0; i < colgen->amt_row; ++i ){
        destroy_list(colgen->_list[i], colgen);
    } free(arrlist);
}

void start_templ(t_colgen* colgen, t_entity* new_templ, dbconfig* database)
{
    assert(new_templ);
    assert(!colgen->_template);

    new_templ->dvalue = colgen->lwall;
    
    colgen->_template = new_templ;
}

void destroy_templ(t_entity *templ)
{
    assert(templ);

    free(templ);
}

linkstorage *init_linkstorage()
{
    linkstorage *new_storage = malloc(sizeof(linkstorage));
    assert(new_storage);

    const linkstorage default_storage = { NULL };
    *new_storage = default_storage;

    return new_storage;
}

linkstorage *start_linkstorage(t_colgen* dependency, t_colgen* colgen, linkstorage* new_storage)
{
    assert(dependency);

    if (( dependency->gentype & List ) && ( dependency->_curr_entity )){
        if ( dependency->gentype & File )
            new_storage->tag.svalue = dependency->_curr_entity->svalue;
        else
            new_storage->tag.dvalue = dependency->_curr_entity->dvalue;
    } else { //then it's a template
        new_storage->tag.dvalue = dependency->_template->dvalue;
    }

    return new_storage;
}

void destroy_linkstorage(linkstorage* storage, const int gentype)
{
    assert(storage);

    if ( gentype & File ){
        free(storage->store_svalue);
    } else {
        free(storage->store_dvalue);
    } free(storage);
}

t_ukey *init_ukey()
{
    t_ukey *new_ukey = malloc(sizeof(t_ukey));
    assert(new_ukey);

    const t_ukey default_ukey = { NULL };
    *new_ukey = default_ukey;

    return new_ukey;
}

void start_ukey(t_colgen *colgen, t_colgen *dependency, t_ukey *new_ukey, dbconfig *database)
{
    assert(new_ukey);
    new_ukey->storage = malloc(sizeof(linkstorage*));
    assert(new_ukey->storage);

    colgen->_dependency = dependency;
    colgen->_ukey = new_ukey;
}

void destroy_ukey(t_ukey *ukey, const int gentype)
{
    assert(ukey);

    if ( ukey->storage ){
        for ( unsigned int i = 0; i < ukey->n_amt ; ++i ){
            destroy_linkstorage(ukey->storage[i], gentype);
        } free(ukey->storage);
    } free(ukey);
}

static void def_typeof(t_colinfo *info, t_colgen *colgen)
{
    assert(info);
    assert(colgen);

    short new_method = info->method;
    short new_gentype = Undef;
   
    /*
     * if amt of elements given is fixed (will be double checked later) it's a list
     * else if new_method is random and also unique or scalable it's a list
     * otherwise it's a template
     */
    if ( info->file )
        new_gentype = ( File | List );
    else if ( new_method & Fix )
        new_gentype = List;
    else if (( new_method & Rnd ) && ( new_method & ( Unq | Scl )))
        new_gentype = List;
    else
        new_gentype = Template; 

    if ( new_method & Lnk )
        new_gentype |= UniqueKey;

    colgen->method = new_method;
    colgen->gentype = new_gentype;
}

t_colgen *init_colgen(dbconfig* database)
{
    t_colgen *new_colgen = malloc(sizeof(t_colgen));
    assert(new_colgen);

    //everything else is set to NULL value
    const t_colgen default_colgen = { 
        .rwall = database->amt_row,
        .delim = database->delim,
        .amt_row = database->amt_row
    };

    *new_colgen = default_colgen;

    return new_colgen;
}

static size_t count_flines(char *file)
{
    FILE *f_count = fopen(file, "r");
    assert(f_count);

    const short len = 50;

    char ptr_str[len];
    size_t ln = 0;
    while ( fgets(ptr_str, len-1, f_count) ){
        ++ln;
    };

    fclose(f_count);
    
    return ln;
}

static void def_fn(t_colgen* colgen, dbconfig* database)
{
    if ( colgen->gentype & List ){
        if ( colgen->gentype & File )
            filesetter_arrlist(colgen->_list, colgen);
        else
            numsetter_arrlist(colgen->_list, colgen);
    } else { // colgen->gentype & Template
        numsetter_templ(colgen, database);
    }
}

t_colgen *start_colgen(t_colinfo* info, t_colgen* colgen, t_colgen *dependency, dbconfig* database)
{
    if ( info->file ){
        colgen->file = strdup(info->file);
    } else {
        if ( info->lwall )
            colgen->lwall = strtod(info->lwall, NULL);
        if ( info->rwall )
            colgen->rwall = strtod(info->rwall, NULL);
    } // can be generate either file or range of numbers

    if ( info->delim )
        colgen->delim = info->delim;

    if ( info->decimal_places )
        colgen->decimal_places = info->decimal_places;


    def_typeof(info, colgen);


    if (( colgen->method & Fix ) && ( atoi(info->amount) >= database->amt_row )){
        colgen->method ^= Fix; //can't have fixed amt be >= dbsize
        colgen->gentype = Template; //and shouldn't be a list
    }

    if ( colgen->gentype & File )
        colgen->amt_row = count_flines(colgen->file);
    else if (( colgen->method & Fix ) && ( atoi(info->amount) < database->amt_row ))
        colgen->amt_row = atoi(info->amount);
    else if (( colgen->rwall - colgen->lwall ) < database->amt_row )
        colgen->amt_row = colgen->rwall - colgen->lwall;
    /*
     * if is of file type and random method then it is necessary to
     * fetch the entire file into memory
     * (otherwise random elements scope would be limited to default db size)
     * else if designated amount to be generated is lower than default db size
     * else if range is lower than default dbsize, assign rante as amt
     */


    if ( colgen->gentype & File )
        sprintf(colgen->format_entity,"%%s%%c");
    else
        sprintf(colgen->format_entity,"%%.%df%%c",colgen->decimal_places);


    if ( colgen->gentype & List )
        start_arrlist(colgen, database);
    else if ( colgen->gentype & Template )
        start_templ(colgen, init_entity(), database);

    if ( colgen->gentype & UniqueKey )
        start_ukey(colgen, dependency, init_ukey(), database);


    def_fn(colgen, database);


    return colgen;
}

int start_ukey_colgen(t_colgen** new_arrcolgen, t_colgen* dependency, t_colinfo* ptr_ukey, dbconfig* database)
{
    int j = 0;

    while ( ptr_ukey ){
        ++j;

        new_arrcolgen[j] = start_colgen(ptr_ukey, init_colgen(database), dependency, database);

        ptr_ukey = ptr_ukey->ukey;
    }

    return j;
}

t_colgen **start_arrcolgen(t_colinfo *info, dbconfig* database)
{
    assert(info);

    t_colgen **new_arrcolgen = malloc(database->amt_col * sizeof(t_colgen*));
    assert(new_arrcolgen);

    int j = 0; //iterate through, and add to, colgen index
    int i = 0; //iterate through colinfo index
    while ( j < database->amt_col ){ //j will be greater than i if there are ukeys
        new_arrcolgen[j] = start_colgen(info+i, init_colgen(database), NULL, database); 

        if ( (info+i)->ukey ){ //adds amount of ukeys to j
            j += start_ukey_colgen(new_arrcolgen+j, new_arrcolgen[j], (info+i)->ukey, database);
        }
        
        if ( i < database->amt_col ){
            ++i;        
        } ++j;
    }
    // makes sure last col will not have delim and will jump line
    new_arrcolgen[database->amt_col-1]->delim = '\n';

    return new_arrcolgen;
}

void print_arrlist(size_t amt_row, t_entity **arrlist, short gentype)
{
    assert(arrlist);
    
    for ( int i = 0; i < 5; ++i ){
        if ( gentype & File )
            fprintf(stderr, "\t# %s\n", arrlist[i]->svalue);
        else
            fprintf(stderr, "\t# %f\n", arrlist[i]->dvalue);
    } fputc('\n', stderr);
    fprintf(stderr, "\tand %d more ...\n",abs(amt_row-5));
}

void print_templ(t_entity *templ)
{
    assert(templ);

    fprintf(stderr, "\tvalue: %f\n", templ->dvalue);
}

void print_arrcolgen(t_colgen **colgen, int amt_col)
{
    assert(colgen);

    for ( int i = 0; i < amt_col; ++i ){
        fprintf(stderr, "n#: %d\nmethod: %d\namt_row: %d\ndelim: %c\ndecimal_places: %d\n", i+1, colgen[i]->method, colgen[i]->amt_row, colgen[i]->delim, colgen[i]->decimal_places);
        
        char gentype[50] = { 0 }; 
        if ( colgen[i]->gentype & UniqueKey ){
            fprintf(stderr,"dependency: %p\n", (void*)colgen[i]->_dependency);
            strcpy(gentype,"UniqueKey");
        }
        if ( colgen[i]->gentype & File ){
            fprintf(stderr,"file: %s\n", colgen[i]->file);
            strcat(gentype,"File");
        } else {
            fprintf(stderr,"range: %f-%f\n", colgen[i]->lwall, colgen[i]->rwall);
        }
        if ( colgen[i]->gentype & Template){
            strcat(gentype, "Template");
            fprintf(stderr, "gentype: %s\n", gentype);
            print_templ(colgen[i]->_template);
        } 
        else if ( colgen[i]->gentype & List ){
            strcat(gentype,"List");
            fprintf(stderr, "gentype: %s\n", gentype);
            print_arrlist( colgen[i]->amt_row, colgen[i]->_list, colgen[i]->gentype );
        }
    }
}

void destroy_colgen(t_colgen *colgen)
{
    assert(colgen);

    if ( colgen->gentype & List ){
        if ( colgen->gentype & File ){
            free(colgen->file); 
        } destroy_arrlist(colgen->_list, colgen);
    } else {
        destroy_templ(colgen->_template);
    }

    if ( colgen->gentype & UniqueKey )
        destroy_ukey(colgen->_ukey, colgen->gentype);
    
    free(colgen);
}

void destroy_arrcolgen(t_colgen **arrcolgen, int amt_col)
{
    assert(arrcolgen);

    for ( int i = 0; i < amt_col; ++i ){
        destroy_colgen(arrcolgen[i]);
    } free (arrcolgen);
}
