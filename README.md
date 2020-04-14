# FAKE DATABASE GENERATOR

Fake data generator for testing, simulating and learning purposes. 

My aim is to create a library which I can use throughout my college term, so that I can easily generate fake database for manipulation.

There should be some documentation coming when I do learn how to make a proper one, until then I'll just keep on commenting everything relevant.

## IMPORTANT NOTES

all the pre-processing directives are located in src/data_fetch.h, which are crucial on generating data with static arrays with dynamic values, also provides easy to access info on each data to be generated.

type 'make' to create the 'run executable, 'make clean' before recompiling again, and 'make debug' if you wish to debug with gdb.

database will be stored in data.csv by default

content file contains pre-existing data to be manipulated in the heap, there is a nome.txt and sobrenome.txt in there by default, which are used in main.c to compile fake names

main.c is just an example code to go by, all the important library functions are located in at .h files in src/
         
main.c example code generates a bank user's database. Each row is a user, and each column is a data pertaining to its user. The first column indicates its unique ID, the second to the user's agency, the third is a unique account to its corresponding agency, and the fourth is its name. 
Because the third column (account) is dependant on the second (agency), it will check for any occurrence of its current value in the agency's tree,and will bind it's value to it if there is none. The code's speed is directly affected by the amount of individuals to be generated (as in DBSIZE) and the amount of agencies available (as in AMT_3). More agencies means fewer accounts being verified for uniqueness, and, the number of individuals increases, so must the number of agencies as to increase the likeability of unique accounts.

## TO BE FIXED/IMPLEMENTED
        
* sorting function for binary tree

* iterative method to free each tree node (maybe)

* change to a datatype other than int that allows for more decimal digits
