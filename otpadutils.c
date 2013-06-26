// otpadutils.c
// Glen Wiley <glen.wiley@gmail.com>
//
// common routines for the one time pad utilities

#include <stdlib.h>
#include "otpadutils.h"

char *g_setfull = "_!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char *g_setab   = "_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char *g_chartbl = NULL;

// otpadutils.c
