// otpadgen.c
// Glen Wiley <glen.wiley@gmail.com>
//
// one time pad generator for use in strong cryptographic exchanges
//
// TODO: pretty formatted output, troff, HTML, PDF
// TODO: option for using numbers only (hex/decimal)
// TODO: add line break support to char table
// TODO: allow user to specify a character table

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "otpadutils.h"

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
		c = getrandchar(NULL);
		if(c == -1)
		{
			printf("ERROR: failed to read from entropy source %s, %s, exiting...\n"
			 , g_fnrand, strerror(errno));
			exit(1);
		}

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
	 "USAGE: otpadgen [-m] [-H | -h <header>] [-l <linelen>] [-w <wordlen>] [-r <devrand>] [-k <keylen>] [-s <f|a>] [-o <text|num>] \n"
	 "-h <header>   print this string as the first line of the output\n"
	 "-H            print a generated header string as the first line of the output\n"
	 "-k <keylen>   key length in chars\n"
	 "              default: %d\n"
	 "-l <linelen>  line length in chars, used by text output mode\n"
	 "              note that each char needs 2 spaces + word gaps\n"
	 "              default: %d\n"
	 "-m            character matrix output, no pad is generated\n"
	 "-o text       output as simple text (default)\n"
	 "   num        output as numbers\n"
	 "-r <devrand>  device from which to read entropy (as binary stream)\n"
	 "              default: %s\n"
	 "-s <f|a>      character set to use\n"
	 "              f = full character set: alpha, digit, punctuation (space=_)\n"
	 "              a = abbreviated character set: alpha, digit (space=_)\n"
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
	 "How to Encrypt\n"
	 "--------------\n"
	 "1. Generate your one time pad (otpsheet) and the character matrix and give\n"
	 "   a copy to your peer\n"
	 "   # otpadgen > otpsheet\n"
	 "   # otpadgen -o matrix > otpmatrix\n"
	 "2. Write your clear text above the characters on the one time pad.\n"
	 "3. Encrypt one character at a time using the matrix:\n"
	 "   - Find the column headed by the clear text character\n"
	 "   - In that column find the row whose character matches the key character\n"
	 "   - The first character in the row is your encrypted character, write\n"
	 "     this under the key on the one time pad sheet\n"
	 "4. Once you have done this for all the characters in your message, copy the\n"
	 "   encrypted characters to a separate sheet of paper - this is the message\n"
	 "   that you can hand your peer to be decrypted.\n"
	 "5. Rejoice at the strength of your cryptography!\n"
	 "\n"
	 "How to Decrypt\n"
	 "--------------\n"
	 "1. Obtain copies of the one time pad sheet and the character matrix from your\n"
	 "   peer.\n"
	 "2. Write the encrypted message above the key characters on the one time pad.\n"
	 "3. Decrypt one character at a time using the matrix:\n"
	 "   - Find the column headed by the message character \n"
	 "   - In that column fund the row whose character matches the key character\n"
	 "   - The first character in the row is your encrypted character, write this\n"
	 "     under the key on the one time pad sheet\n"
	 "4. The characters below the key characters are the decrypted message.\n"
	 "5. Glory in the light of your unbreakable message.\n"
	 "\n"
	 "Here is an example of looking up the characters using the matrix:\n"
	 "\n"
	 "Given the following table segment (this is contrived to keep the demo short):\n"
	 "\n"
	 "    _ 0 1 2 3 4 5 6 7 8 9 A B C D E F G H I J K L M N O P Q R S T U V W X Y Z\n"
	 "-----------------------------------------------------------------------------\n"
	 "_ | _ 0 1 2 3 4 5 6 7 8 9 A B C D E F G H I J K L M N O P Q R S T U V W X Y Z\n"
	 "0 | 0 1 2 3 4 5 6 7 8 9 A B C D E F G H I J K L M N O P Q R S T U V W X Y Z _\n"
	 "1 | 1 2 3 4 5 6 7 8 9 A B C D E F G H I J K L M N O P Q R S T U V W X Y Z _ 0\n"
	 "2 | 2 3 4 5 6 7 8 9 A B C D E F G H I J K L M N O P Q R S T U V W X Y Z _ 0 1\n"
	 "3 | 3 4 5 6 7 8 9 A B C D E F G H I J K L M N O P Q R S T U V W X Y Z _ 0 1 2\n"
	 "\n"
	 "And given this message text:  M U S T _\n"
	 "And given this one time pad:  O X T V 0\n"
	 "The encrypted text would be:  1 2 0 1 0\n"
	 "\n"
	 "The Decrypt would look like this:\n"
	 "Given this encrypted message:       1 2 0 1 0\n"
	 "Given the same OTP used to encrypt: O X T V 0\n"
	 "Results in the cleartext:           M U S T _\n"
	 "\n"
	 , OTPAD_DFLT_KEYLEN, OTPAD_DFLT_LINE, OTPAD_DFLT_RAND, OTPAD_DFLT_SET
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
	char outputmode = OTPAD_OUTPUT_TEXT;
	char charset    = OTPAD_DFLT_SET;
	char *hdr       = NULL;
	time_t now;
	
	//-------------------- command line options

	while((opt=getopt(argc, argv, "?Hh:k:l:mo:r:s:w:")) != -1)
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

			case 'r':
				g_fnrand = optarg;
				break;

			case 's':
				charset = optarg[0];
				break;

			case 'w':
				wordlen = atoi(optarg);
				break;

            case '?':
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

	if(getrandchar(g_fnrand) == -1)
	{
		printf("ERROR: failed to read from entropy source %s, %s, exiting...\n"
		 , g_fnrand, strerror(errno));
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

// otpadgen.c
