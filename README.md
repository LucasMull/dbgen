#dbgen
###database generator cli utility tool

##SYNOPSIS
      dbgen [OPTION]... [DELIM]... [FIELD]...

##DESCRIPTION
      (WIP)

##INSTALLING
      (WIP) type sudo make install to build dbgen in your usr/local/bin
      type make to build dbgen in it's folder.

##OPTIONS
      -r
            random values from specified field file or range of numbers.
      -u      
            unique values from specified field file or range of numbers.
      -s      
            scale values proportionally to field's range of numbers (if specified).
      ~     (WIP) will assume the next column is linked to the leftmost
            standalone (not linked to any other) column.

##DELIM
      /s
            delim of designated column will be blank space
      /t
            delim of designated column will be tab
      \ " ' ;
            delim of designated column will be any of the above chars

##FIELD
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
