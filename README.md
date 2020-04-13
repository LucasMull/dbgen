# generate_table

Fake data generator for testing, simulating and learning purposes. 

My aim is to create a library which I can use throughout my college term, so that I can easily generate fake stored data for manipulation.

There should be some documentation coming when I do learn how to make a proper one, until then I'll just keep on commenting everything that's relevant.

IMPORTANT NOTES#-
        -all the '#define' directives are located in src/data_fetch.h, which are crucial if you wish to generate data with static arrays, also provides details on what to expect from each type of data to be generated.

        -type 'make' to create the executable, and 'make clean' before recompiling again, also 'make debug' if you wish to debug with gdb.

        -fake data generated will be stored in data.csv by default

        -main.c is simply and example code to go by, all the relevant functions are located in src/

        -content/ contains pre-created data to be manipulated via code, there is a name.txt and surname.txt in there by default, which are used to generate fake names in main.c

TO BE IMPLEMENTED/FIXED#-
        
        -sorting function for binary tree
        -iterative method to free each tree node (maybe)
        -organize static arrays by address, by addressing each array to a struct t_STACK with address slot, such as t_STACK STACK[0]= s_agency
        -change to a datatype other than int that allows for more decimal digits
