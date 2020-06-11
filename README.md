# dbgen
### database generator cli utility tool

## SYNOPSIS
      dbgen [OPTION]... [DELIM]... [FIELD]... [LINK]...

## DESCRIPTION
      (WIP)

## INSTALLING
      (WIP) type sudo make install to build dbgen in your usr/local/bin
      type make to build dbgen in it's folder.

## OPTIONS
      -r
            random values from specified field file or range of numbers.
      -u      
            unique values from specified field file or range of numbers.
      -s      
            scale values proportionally to field's range of numbers (if specified).
      -S
            size of database
            Ex: -S 1000 ; database of 1000 elements
      -o
            output stream
            ex: -o data.csv ; outputs database to data.csv

## DELIM
      /s
            delim of designated column will be blank space
      /t
            delim of designated column will be tab
      \ " ' ;
            delim of designated column will be any of the above chars

## FIELD
     Every field must be enclosed by square brackets, and can either assume 
     a range of numbers to be manipulated, or a file to have it's members 
     manipulated. 

     To indicate a limit on how much numbers or file strings are to be, a value
     should be indicated after a comma.

     Examples:

        dbgen [100-500]
            will generate any values from 1 to 500 linearly
        dbgen -r [100-500,25]
            will generate 25 possible values from 1 to 500 randomly
        dbgen -r [file.txt]
            will fetch any line from file.txt from 1 to n randomly
        dbgen -rus [1000-3000]
            will generate unique values from 1000 to 3000, that scale
            proportionally to the database size, and are randomly arranged

## LINK
     A field may be linked to another field (aka the linker), meaning that
     the linked field holds a dependency on the output of the linker. For each value input by
     the linker, each corresponding value input by the linked field must be unique to that particular
     linker's value. For example, if the linker inputs a value of "John" and the linked proceeds with
     a value of "1234", the value "1234" becomes unavailable for every proceeding input of "John",
     rendering each value that's input by the linked field, unique to that particular linker value.

     \~     
        will link the field to the right of the symbol with the leftmost (not linked to any) column

     Examples:
        
        dbgen [data.txt] \~ [100-500]
                links values from range 100-500 to the values of data.txt,
