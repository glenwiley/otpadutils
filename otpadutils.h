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
#define OTPAD_DFLT_KEYLEN   400
#define OTPAD_DFLT_WORDLEN  5
#define OTPAD_DFLT_LINELEN  30
#define OTPAD_SETFULL       'f'
#define OTPAD_SETAB         'a'
#define OTPAD_DFLT_SET      'a'
#define OTPAD_DFLT_RAND     "/dev/urandom"

extern char *g_setfull;
extern char *g_setab;
extern char *g_chartbl;
extern char *g_fnrand;

int getrandchar(char *fnrand);

#endif // OTPADUTILS_H
