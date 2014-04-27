// otpadmsg.c
// Glen Wiley <glen.wiley@gmail.com>
//
// one time pad based message encrypt decrypt
//
// TODO: fix stats summary for hex sheet (count 2 char bytes)
// TODO: translate message character case as needed
// TODO: auto-detect char set 
// TODO: auto-generated and checked checksum/hash
// TODO: recommended approach for hash will fail because message includes
//       chars that are ignored - fix the instructions
// TODO: auto-detect headers etc. in sheet and message

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "otpadutils.h"

struct summary_st
{
	int msgbytestot;
	int msgbytesbad;
	int msglines;
	int sheetbytes;
	int sheetlines;
}; 

//---------------------------------------- usage
void
usage(void)
{
	printf(
	 "USAGE: otpmsg [-l] [-dft] [-T <tbl_file>] [-s <a|f|h>] -S <sheet_file> -m <msg_file>\n"
	 "-d              add message divider to output\n"
	 "                and print summary data after message\n"
	 "-f              fail if message character is not in character set\n"
	 "                newline chars are always ignored\n"
	 "                default: ignore non-matching characters\n"
	 "-l              operate in character lookup mode, input one character\n"
	 "                at a time on stdin and produce translated chars\n"
	 "                in this case the sheet and message files are ignored\n"
	 "-m <msg_file>   file containing a message to encrypt/decrypt\n"
	 "-S <sheet_file> one time pad sheet to use to encrypt/decrypt msg\n"
	 "-s <a|f|h>      character set to use (must match the one used by gen)\n"
	 "                f = full character set: alpha, digit, punctuation (space=_)\n"
	 "                a = abbreviated character set: alpha, digit (space=_)\n"
	 "                h = hex numbers from 00-FF\n"
	 "                default: %c\n"
	 "-t              translate spaces in message to underscore\n"
	 "                default is no\n"
	 "\n"
	 "It is a good idea to generate a hash and include it in the message.\n"
	 "For example, given a clear text file 'msg':\n"
	 "# openssl sha1 msg | tr '[a-z]' '[A-Z]' | sed 's/.* //g' >> msg.sha1\n"
	 "# cat msg msg.sha1 > msg.all\n"
	 "# otpadmsg -s a -S sheet -m msg.all > msg.enc\n"
	 "\n"
	 "The recipient of message.enc could decrypt, separate the hash from the file\n"
	 "and run the hash for himself to erify that the entire message was trnasmitted.\n"
	 "\n"
	 , OTPAD_DFLT_SET);
} // usage

//---------------------------------------- translatemsg_num
void
translatemsg_num(struct summary_st *stats, char *fnsht, char *fnmsg, char failchars)
{
	FILE *fhsht;
	FILE *fhmsg;
	char shtbuf[OTPAD_MAX_LINE+3];
	char msgbuf[OTPAD_MAX_LINE+3];
	int  i;
	int  l;
	int  spos;
	int  mpos;
	char c;
	int csht;
	int cmsg;

	stats->msgbytestot = 0;
	stats->msgbytesbad = 0;
	stats->msglines    = 0;
	stats->sheetbytes  = 0;
	stats->sheetlines  = 0;

	fhsht = fopen(fnsht, "r");
	if(fhsht == NULL)
	{
		fprintf(stderr, "ERROR: failed to open sheet file %s, %s, exiting\n"
		 , fnsht, strerror(errno));
		exit(1);
	}
	fhmsg = fopen(fnmsg, "r");
	if(fhmsg == NULL)
	{
		fprintf(stderr, "ERROR: failed to open message file %s, %s, exiting\n"
		 , fnmsg, strerror(errno));
		exit(1);
	}

	// read message and consume the sheet as we go
	// need to keep track of our position in the sheet stream since
	// the chars/lines in message may not match the sheet
	shtbuf[0] = '\0';
	spos = 0;
	while(fgets(msgbuf, OTPAD_MAX_LINE+2, fhmsg) != NULL)
	{
		stats->msglines++;
		l = strlen(msgbuf);
		stats->msgbytestot += l;
		for(mpos=0; mpos < l; mpos++)
		{
			if(msgbuf[mpos] == '\n' || msgbuf[mpos] == '\r')
				continue;

			// fail on bogus chars if user wants us to

			if(isxdigit(msgbuf[mpos]) != 0 || isxdigit(msgbuf[mpos+1]) != 0)
			{
				if(failchars == 'y')
				{
					printf("message line %d, char %d is not a hex digit, "
					 "exiting\n", stats->msglines, mpos);
					exit(1);
				}
				stats->msgbytesbad++;
				continue;
			}

			sscanf(msgbuf+mpos, "%.2X", &cmsg);
			mpos++; // the loop will increment it once more for us

			// when we run out of sheet bytes read some more

			while(1)
			{
				while(shtbuf[spos] == '\0')
				{
					if(fgets(shtbuf, OTPAD_MAX_LINE+2, fhsht) == NULL)
					{
						fprintf(stderr, "ERROR: exhausted sheet file before end of"
						 " message, exiting\n");
						exit(1);
					}
					spos = 0;
				}

				// consume chars until we are able to translate one

				if(shtbuf[spos] != ' ' && shtbuf[spos] != '\n' 
				 && shtbuf[spos] != '\r')
				{
					if(isxdigit(shtbuf[spos]) != 0 || isxdigit(shtbuf[spos+1]) != 0)
					{
						fprintf(stderr, "ERROR: sheet contains invalid hex digit "
						 "at line %d, char %d, exiting\n", stats->sheetlines, spos);
						exit(1);
					}
					sscanf(shtbuf+spos, "%.2X", &csht);
					spos = spos + 2;

					c = charlookup(cmsg, csht);
					printf("%.2X", c);
					stats->sheetbytes++;
					break;
				}
				else
				{
					printf("%c", shtbuf[spos]);
					spos++;
				}
			} // while(1)
		} // for(mpos=0; mpos < l; mpos++)
	} // while(fgets(msgbuf, OTPAD_MAX_LINE+2, fhmsg) != NULL)

	fflush(stdout);

	fclose(fhsht);
	fclose(fhmsg);

	return;
} // translatemsg_num


//---------------------------------------- translatemsg
void
translatemsg(struct summary_st *stats, char *fnsht, char *fnmsg, char failchars
 , char txspc)
{
	FILE *fhsht;
	FILE *fhmsg;
	char shtbuf[OTPAD_MAX_LINE+3];
	char msgbuf[OTPAD_MAX_LINE+3];
	int  i;
	int  l;
	int  spos;
	int  mpos;
	char c;

	stats->msgbytestot = 0;
	stats->msgbytesbad = 0;
	stats->msglines    = 0;
	stats->sheetbytes  = 0;
	stats->sheetlines  = 0;

	fhsht = fopen(fnsht, "r");
	if(fhsht == NULL)
	{
		fprintf(stderr, "ERROR: failed to open sheet file %s, %s, exiting\n"
		 , fnsht, strerror(errno));
		exit(1);
	}
	fhmsg = fopen(fnmsg, "r");
	if(fhmsg == NULL)
	{
		fprintf(stderr, "ERROR: failed to open message file %s, %s, exiting\n"
		 , fnmsg, strerror(errno));
		exit(1);
	}

	// read message and consume the sheet as we go
	// need to keep track of our position in the sheet stream since
	// the chars/lines in message may not match the sheet
	shtbuf[0] = '\0';
	spos = 0;
	while(fgets(msgbuf, OTPAD_MAX_LINE+2, fhmsg) != NULL)
	{
		stats->msglines++;
		l = strlen(msgbuf);
		stats->msgbytestot += l;
		for(mpos=0; mpos < l; mpos++)
		{
			if(msgbuf[mpos] == '\n' || msgbuf[mpos] == '\r')
				continue;

			// translate spaces in message if user wants us to

			if(msgbuf[mpos] == ' ' && txspc == 'y')
				msgbuf[mpos] = '_';

			// fail on bogus chars if user wants us to

			if(strchr(g_chartbl, msgbuf[mpos]) == NULL)
			{
				if(failchars == 'y')
				{
					printf("message line %d, char %d is not in message set, "
					 "exiting\n", stats->msglines, mpos);
					exit(1);
				}
				stats->msgbytesbad++;
				continue;
			}

			// when we run out of sheet bytes read some more

			while(1)
			{
				while(shtbuf[spos] == '\0')
				{
					if(fgets(shtbuf, OTPAD_MAX_LINE+2, fhsht) == NULL)
					{
						fprintf(stderr, "ERROR: exhausted sheet file before end of"
						 " message, exiting\n");
						exit(1);
					}
					stats->sheetlines++;
					spos = 0;
				}

				// consume chars until we are able to translate one

				if(shtbuf[spos] != ' ' && shtbuf[spos] != '\n' 
				 && shtbuf[spos] != '\r')
				{
					if(strchr(g_chartbl, c) == NULL)
					{
						fprintf(stderr, "ERROR: sheet file has invalid char at "
						 "line %d, char %d, exiting\n", stats->sheetlines, spos);
						exit(1);
					}
					c = charlookup(msgbuf[mpos], shtbuf[spos]);
					printf("%c", c);
					spos++;
					stats->sheetbytes++;
					break;
				}
				else
				{
					printf("%c", shtbuf[spos]);
					spos++;
				}
			} // while(1)
		} // for(mpos=0; mpos < l; mpos++)
	} // while(fgets(msgbuf, OTPAD_MAX_LINE+2, fhmsg) != NULL)

	fflush(stdout);

	fclose(fhsht);
	fclose(fhmsg);

	return;
} // translatemsg

//---------------------------------------- interactivelookup
// interactive lookup
void
interactivelookup(void)
{
	int  cmsg;
	int  ckey;
	int  c;
	char buf[10];

	while(( 1 ))
	{
		printf("message char? ");
		fflush(stdout);
		buf[0] = '\0';
		fgets(buf, 4, stdin);
		if(buf[0] == '\0' || buf[0] == '\n')
			exit(0);
		if(g_chartbl_typ == otpad_tbl_chr)
		{
			cmsg = buf[0];
			if(strchr(g_chartbl, cmsg) == NULL)
			{
				printf("character set does not contain this character\n");
				printf("char set: %s\n", g_chartbl);
				continue;
			}
		}
		else
			sscanf(buf, "%X", &cmsg);


		printf("key char? ");
		fflush(stdout);
		buf[0] = '\0';
		fgets(buf, 4, stdin);
		if(g_chartbl_typ == otpad_tbl_chr)
		{
			ckey = buf[0];
			if(strchr(g_chartbl, ckey) == NULL)
			{
				printf("character set does not contain this character\n");
				printf("char set: %s\n", g_chartbl);
				continue;
			}
		}
		else
			sscanf(buf, "%X", &ckey);

		c = charlookup(cmsg, ckey);
		if(g_chartbl_typ == otpad_tbl_chr)
		{
			if(c == '\0')
				printf("char lookup failed\n");
			else
				printf("char=%c\n\n", c);
		}
		else
			printf("char=%.2X\n\n", c);
	}
	return;
} // interactivelookup

//---------------------------------------- main
int
main(int argc, char *argv[])
{
	struct summary_st stats;
	int opt;
	char mode       = 't';
	char failchars  = 'n';
	char summary    = 'n';
	char txpsc      = 'n';
	char charset    = OTPAD_DFLT_SET;
	char *fnmsg     = NULL;
	char *fnsht     = NULL;
	char *fntbl     = NULL;
	
	//-------------------- command line options

	while((opt=getopt(argc, argv, "?dflm:s:S:tT:")) != -1)
	{
		switch(opt)
		{
			case 'l':
				mode = 'l';
				break;
				
			case 'd':
				summary = 'y';
				break;

			case 'f':
				failchars = 'y';
				break;

			case 'm':
				fnmsg = optarg;
				break;

			case 's':
				charset = optarg[0];
				break;

			case 'S':
				fnsht = optarg;
				break;

			case 't':
				txpsc = 'y';
				break;

			case 'T':
				fntbl = optarg;
				break;

            case '?':
			default:
				usage();
				exit(1);
				break;
		}
	} // while

	// arrange our character table

	if(charset == OTPAD_SETFULL)
		g_chartbl = g_ctbl_full;
	else if(charset == OTPAD_SETAB)
		g_chartbl = g_ctbl_alnum;
	else if(charset == OTPAD_SETHEX)
	{
		g_chartbl = NULL;
		g_chartbl_typ = otpad_tbl_num;
	}
	if(fntbl != NULL)
	{
		g_chartbl = readchartbl(fntbl);
	}

	if(g_chartbl == NULL && g_chartbl_typ == otpad_tbl_chr)
	{
		fprintf(stderr, "ERROR, unrecognized character set, exiting...\n");
		exit(1);
	}

	// if user asked for interactive lookup of characters then we will
	// not encrypt/decrypt

	if(mode == 'l')
		interactivelookup();

	// interactive mode will exit() when done, so if we are here then
	// the user expects us to translate the message

	if(fnsht == NULL)
	{
		fprintf(stderr, "ERROR, no sheet file specified, exiting...\n");
		exit(1);
	}

	if(fnmsg == NULL)
	{
		fprintf(stderr, "ERROR, no message file specified, exiting...\n");
		exit(1);
	}

	if(g_chartbl_typ == otpad_tbl_chr)
		translatemsg(&stats, fnsht, fnmsg, failchars, txpsc);

	if(g_chartbl_typ == otpad_tbl_num)
		translatemsg_num(&stats, fnsht, fnmsg, failchars);

	if(summary == 'y')
	{
		printf("\n-------------------- end of message\n\n"
		 "message bytes, total:          %d\n"
		 "message bytes not in char set: %d\n"
		 "message lines:                 %d\n"
		 "sheet bytes used:              %d\n"
		 , stats.msgbytestot, stats.msgbytesbad, stats.msglines, stats.sheetbytes);
	}

	return 0;
} // main

// otpadmsg.c
