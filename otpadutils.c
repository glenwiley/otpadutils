// otpadutils.c
// Glen Wiley <glen.wiley@gmail.com>
//
// common routines for the one time pad utilities

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "otpadutils.h"

char *g_setfull = "_!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char *g_setab   = "_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char *g_chartbl = NULL;
char *g_fnrand  = OTPAD_DFLT_RAND;

//---------------------------------------- getrandchar
// the first time the function is called it opens the entropy file
// returns -1 on error
int
getrandchar(char *fnrand)
{
	static int fdrand = -1;
	static int tbllen;
	unsigned char c;
	int n;


	if(fdrand == -1)
	{
		fdrand = open(fnrand, O_RDONLY);
		if(fdrand == -1)
			return -1;

		tbllen = strlen(g_chartbl);
	}

	n = read(fdrand, &c, 1);
	if(n != 1)
		return -1;
	else
		c = g_chartbl[c % tbllen];

	return c;
} // getrandchar

// otpadutils.c
