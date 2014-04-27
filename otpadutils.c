// otpadutils.c
// Glen Wiley <glen.wiley@gmail.com>
//
// common routines for the one time pad utilities

#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "otpadutils.h"

char *g_ctbl_full   = "_!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char *g_ctbl_alnum  = "_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char *g_chartbl     = NULL;
char *g_fnrand      = OTPAD_DFLT_RAND;
int   g_chartbl_typ = otpad_tbl_chr;

//---------------------------------------- readchartbl
// read character table from a file, every character that appears in the file
// will be included in the table 
// newline characters are ignored
// return allocated table (caller must free) or NULL on error
char *
readchartbl(char *filename)
{
	FILE *fh;
	char *tbl = NULL;
	char *p;
	char buf[OTPAD_MAX_LINE+3];
	char c;
	int  i;
	int  l;
	int  tc = 0;

	fh = fopen(filename, "r");
	if(fh == NULL)
	{
		fprintf(stderr, "ERROR: failed to read character table file %s, %s, exiting\n"
		 , filename, strerror(errno));
		exit(1);
	}

	tbl = (char *) malloc(OTPAD_TABLE_SIZE);
	if(tbl == NULL)
	{
		fprintf(stderr, "ERROR: failed to allocate memory for character table, exiting\n");
		exit(1);
	}

	while(fgets(buf, OTPAD_MAX_LINE, fh) != NULL)
	{
		l = strlen(buf);
		for(i=0; i<l; i++)
		{
			c = buf[i];
			if(isprint(c))
				tbl[tc++] = c;
		}
	}
	tbl[tc] = '\0';

	fclose(fh);

	return tbl;
} // readchartbl

//---------------------------------------- bldlkuptbl
// construct a lookup table using the character set
// it is a set of 255 arrarys of 255 chars
// we do this to allow for an odd mix of chars (not all ascii)
// caller must free the
char **
bldlkuptbl(void)
{
	int  i;
	int  j;
	int  r;
	int  c;
	int  l;
	int  p;
	char **tbl;

	tbl = (char **) malloc(sizeof(char *) * OTPAD_TABLE_SIZE);
	if(tbl == NULL)
	{
		fprintf(stderr, "ERROR: failed to allocate memory for lookup table, exiting\n");
		exit(1);
	}
	for(r=0; r<OTPAD_TABLE_SIZE; r++)
	{
		tbl[r] = (char *) malloc(OTPAD_TABLE_SIZE);
		if(tbl[r] == NULL)
		{
			fprintf(stderr, "ERROR: failed to allocate memory for lookup table, exiting\n");
			exit(1);
		}
		memset(tbl[r], '\0', OTPAD_TABLE_SIZE);
	}

	// there is a row 'r' for each msg char
	l = strlen(g_chartbl);
	for(i=0; i<l; i++)
	{
		// there is a char in row r for each key char
		r = g_chartbl[i];
		// we start each column with the row char
		// see the matrix if this is confusing
 		j = i;
		p = 0;
		do
		{
			tbl[r][g_chartbl[j]] = g_chartbl[p];
			p++;
			if(p == l)
				p = 0;

			j++;
			if(j == l)
				j = 0;
		} while(j != i);
	} // for i

	return tbl;
} // bldlkuptbl

//---------------------------------------- charlookup
// lookup character in character set and return the appropriate char
// 
// return '\0' if lookup fails
unsigned char
charlookup(unsigned char msg, unsigned char key)
{
	static char **tbl = NULL;
	unsigned char retval;

	if(g_chartbl_typ == otpad_tbl_chr)
	{
		if(tbl == NULL)
			tbl = bldlkuptbl();
		retval = tbl[msg][key];
	}
	else
	{
		retval = msg ^ key;
	}

	return retval;
} // charlookup

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

		if(g_chartbl_typ == otpad_tbl_chr)
			tbllen = strlen(g_chartbl);
		else
			tbllen = OTPAD_TABLE_SIZE;
	}

	n = read(fdrand, &c, 1);
	if(n != 1)
		return -1;
	else
		if(g_chartbl_typ == otpad_tbl_chr)
			c = g_chartbl[c % tbllen];

	return c;
} // getrandchar

// otpadutils.c
