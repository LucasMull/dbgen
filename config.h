#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#define _error(A,B) fprintf(stderr, "dbgen: %s%s\n", A, B)

#define ERR_READ "READ ERROR -- "
#define ERR_FILE "CANT READ FILE -- "
#define ERR_LINK "CANT LINK MORE THAN TWO FILES"

#endif
