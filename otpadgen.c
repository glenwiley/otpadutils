// main.c
// Glen Wiley <glen.wiley@gmail.com>
//
// one time pad generator for use in strong cryptographic exchanges
//
// TODO: pretty formatted output, troff, HTML, PDF
// TODO: option for using numbers only (hex/decimal)
// TODO: add line break support
// TODO: randomicity test output
// TODO: encrypt/decrypt function

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define OTPAD_MAXHDR        80
#define OTPAD_OUTPUT_MATRIX 'm'
#define OTPAD_OUTPUT_TEXT   't'
#define OTPAD_DFLT_LINE     25
#define OTPAD_DFLT_KEYLEN   400
#define OTPAD_DFLT_WORDLEN  5
#define OTPAD_DFLT_LINELEN  30
#define OTPAD_SETFULL       'f'
#define OTPAD_SETAB         'a'
#define OTPAD_DFLT_SET      'f'

char g_setfull[] = "_!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char g_setab[]   = "_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char *g_chartbl  = NULL;

//---------------------------------------- getrandchar
char
getrandchar(void)
{
	int tbllen;
	char c;

	tbllen = strlen(g_chartbl);

	c = g_chartbl[arc4random() % tbllen];

	return c;
} // getrandchar

//---------------------------------------- output_matrix
void
output_matrix(char *hdr)
{
	int line;
	int col;
	int tbllen;
	int c;

	if(hdr != NULL)
	{
		printf("%s\n\n", hdr);
	}

	tbllen = strlen(g_chartbl);

	printf("    ");
	for(col=0; col<tbllen; col++)
		printf("%c ", g_chartbl[col]);
	printf("\n");
	printf("---");
	for(col=0; col<tbllen; col++)
		printf("--");
	printf("\n");

	for(line=0; line<tbllen; line++)
	{
		printf("%c | ", g_chartbl[line]);
		for(col=0; col<tbllen; col++)
		{
			c = line + col;
			if(c >= tbllen)
				c = c - tbllen;
			printf("%c ", g_chartbl[c]);
		}
		printf("\n");
	}

	return;
} // output_matrix

//---------------------------------------- output_text
// produce plain text output
void
output_text(char *hdr, int keylen, int wordlen, int linelen)
{
	int  i;
	char c;
	char lc = 0;

	if(hdr != NULL)
	{
		printf("%s\n", hdr);
	}

	printf("\n\n");

	for(i=0; i<keylen; i++)
	{
		lc++;
		c = getrandchar();

		printf("%c ", c);

		if(wordlen > 0 && lc % wordlen == 0)
			printf("   ");

		if(lc % linelen == 0 && (wordlen == 0 || lc % wordlen == 0))
		{
			lc = 0;
			printf("\n\n\n");
		}
	}

	printf("\n");
	return;
} // output_text

//---------------------------------------- usage
void
usage(void)
{
	printf(
	 "USAGE: otpadgen [-m] [-H | -h <header>] [-l <linelen>] [-w <wordlen>] [-k <keylen>] [-s <f|a>] -o <text|num> \n"
	 "-h <header>   print this string as the first line of the output\n"
	 "-H            print a generated header string as the first line of the output\n"
	 "-k <keylen>   key length in chars\n"
	 "              default: %d\n"
	 "-l <linelen>  line length in chars, used by text output mode\n"
	 "              note that each char needs 2 spaces + word gaps\n"
	 "              default: %d\n"
	 "-m            character matrix output, no pad is generated"
	 "-o text       output as simple text\n"
	 "   num        output as numbers\n"
	 "-s <f|a>      character set to use\n"
	 "              f = full character set: alpha, digit, punctuation\n"
	 "              a = abbreviated character set: alpha, digit \n"
	 "              default: %c\n"
	 "-w <wordlen>  word length in chars, affects printing only\n"
	 "              default: %d\n"
	 "\n"
	 "Generate a one time pad that can be used for encrypted message exchanges.\n"
	 "By default a printable format is produced in ASCII text that includes the\n"
	 "following characters for the full set:\n"
	 "%s\n"
	 "and the following characters for the abbreviated set:\n"
	 "%s\n"
	 "<space> rendered as underscore (the underscore character is not supported)\n"
	 "\n"
	 "You will need the character matrix (Vigenere square) to encrypt/decrypt.\n"
	 "\n"
	 "Steps to encrypt:\n"
	 "1. Coordinate which sheet you will be using with the receiving party.\n"
	 "2. Write your clear text above the characters on the sheet.\n"
	 "3. For each character:\n"
	 "   a. Look up the clear text character on the column header in the matrix.\n"
	 "   b. Look up the key character on the row header in the matrix\n"
	 "   c. The enrypted character is at the interesction of the column/row\n"
	 "      write that below the key character on your sheet.\n"
	 "4. The characters under the key text are your encrypted message, send this\n"
	 "   to the receiving party.\n"
	 "5. Rejoice at the strength of your cryptography!\n"
	 "\n"
	 "Steps to decrypt:\n"
	 "1. Coordinate which sheet you will use with the sender.\n"
	 "2. Write the encrypted message BELOW the key characters on the sheet.\n"
	 "3. Look up the key character on the column header.\n"
	 "4. Find the encrypted character in that column.\n"
	 "5. Write the character in the row header above the key character.\n"
	 "6. The characters above the key characters are the decrypted message.\n"
	 , OTPAD_DFLT_KEYLEN, OTPAD_DFLT_LINE, OTPAD_DFLT_SET
	 , OTPAD_DFLT_WORDLEN, g_setfull, g_setab
	);
} // usage

//---------------------------------------- main
int
main(int argc, char *argv[])
{
	int opt;
	int keylen      = OTPAD_DFLT_KEYLEN;
	int wordlen     = OTPAD_DFLT_WORDLEN;
	int linelen     = OTPAD_DFLT_LINE;
	char outputmode = '\0';
	char charset    = OTPAD_DFLT_SET;
	char *hdr       = NULL;
	time_t now;
	
	//-------------------- command line options

	while((opt=getopt(argc, argv, "Hh:k:l:mo:s:w:")) != -1)
	{
		switch(opt)
		{
			case 'h':
				hdr = optarg;
				break;

			case 'H':
				hdr = malloc(OTPAD_MAXHDR+1);
				time(&now);
				snprintf(hdr, OTPAD_MAXHDR-1, "Sheet Generated %s", ctime(&now));
				hdr[OTPAD_MAXHDR-1] = '\0';
				break;

			case 'k':
				keylen = atoi(optarg);
				break;

			case 'l':
				linelen = atoi(optarg);
				break;

			case 'm':
				outputmode = OTPAD_OUTPUT_MATRIX;
				break;

			case 'o':
				outputmode = optarg[0];
				break;

			case 's':
				charset = optarg[0];
				break;

			case 'w':
				wordlen = atoi(optarg);
				break;

			default:
				usage();
				exit(1);
				break;
		}
	} // while

	if(charset == OTPAD_SETFULL)
		g_chartbl = g_setfull;
	else if(charset == OTPAD_SETAB)
		g_chartbl = g_setab;
	else
	{
		printf("otpadgen ERROR, unrecognized character set, exiting...\n");
		exit(1);
	}

	if(outputmode != OTPAD_OUTPUT_MATRIX && keylen == 0)
	{
		printf("otpadgen ERROR, no key length specified, exiting...\n");
		exit(1);
	}

	if(outputmode == OTPAD_OUTPUT_TEXT)
	{
		output_text(hdr, keylen, wordlen, linelen);
	} else if(outputmode == OTPAD_OUTPUT_MATRIX)
	{
		output_matrix(hdr);
	}
	else
	{
		printf("otpadgen ERROR, no output mode specified, exiting...\n");
		exit(1);
	}

	return 0;
} // main

// main.c
