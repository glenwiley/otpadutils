// otpadutils.h
// Glen Wiley <glen.wiley@gmail.com>
//
// one time pad generator for use in strong cryptographic exchanges
//

#ifndef OTPADUTILS_H
#define OTPADUTILS_H

#define OTPAD_MAXHDR        80
#define OTPAD_OUTPUT_MATRIX 'm'
#define OTPAD_OUTPUT_TEXT   't'
#define OTPAD_DFLT_LINE     25
#define OTPAD_MAX_LINE      255
#define OTPAD_DFLT_KEYLEN   400
#define OTPAD_DFLT_WORDLEN  5
#define OTPAD_DFLT_LINELEN  30
#define OTPAD_TABLE_SIZE    256
#define OTPAD_SETFULL       'f'
#define OTPAD_SETAB         'a'
#define OTPAD_SETHEX        'h'
#define OTPAD_DFLT_SET      'a'
#define OTPAD_DFLT_RAND     "/dev/urandom"
enum OTPAD_TBL_TYPE {otpad_tbl_chr, otpad_tbl_num};
extern char *g_ctbl_full;
extern char *g_ctbl_alnum;
extern char *g_chartbl;
extern int   g_chartbl_typ;
extern char *g_fnrand;
int getrandchar(char *fnrand);
char *readchartbl(char *filename);

#endif // OTPADUTILS_H
