# FAKE DATABASE GENERATOR
### C library for compiling of fake databases

My aim is to create a library that I can use throughout my college term, so that I can easily generate fake data for manipulation.

There should be some documentation coming when I do learn how to make a proper one, until then I'll just keep on commenting everything relevant.

## IMPORTANT NOTES

all the pre-processing directives are located in src/data_fetch.h, which are crucial on generating static array data with dynamic user-defined sizes, also serves as documentation for each data to be generated.

main.c is just an example code to go by, every important library functions are located at .h/.c files in src folder
         
main.c will generate a bank's client database of 2million individuals. Each row represents a client, with each column pertaining to a particular data about its client. The first column indicates a unique ID, followed by the client's agency, an account with a unique value to his agency, and his/her name.
Because the third column (account) is dependant on the second (agency), it will check for any occurrence of its current value in the agency's tree of accounts,and will add it's value to the tree if there is none. 
The code's speed (in this example case) is directly affected by the amount of individuals to be generated (as in DBSIZE) and the amount of agencies available (as in AMT_3). More agencies means fewer accounts being verified for duplicates, and as the number of individuals increases, so must follow the number of agencies if you wish to increase the likelihood of coming across a unique account in fewer tries.

## COMPILE AND EXECUTION

type 'make' to create the 'run' executable, 'make clean' before recompiling, and 'make debug' if you wish to debug with gdb.

database will be stored in data.csv by default

content file contains pre-existing data to be dynamically manipulated in the heap, there is a nome.txt and sobrenome.txt in there by default, which are used in main.c to compile fake names

## TO BE FIXED/IMPLEMENTED
        
* sorting function for binary tree

* iterative method to free each tree node (maybe)

* change to a datatype that allows for more decimal digits
