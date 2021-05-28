#ifndef _CRAND_H
/* =============== */

#ifndef _STDDEF_H
#include <stddef.h>
#endif
#ifndef _STDIO_H
#include <stdio.h>
#endif

#define _CRAND_H 1

/* Fit n into <lbound; hbound) */
#define LIMIT_TO_RANGE(n, lbound, hbound) \
	((n) % ((hbound) - (lbound)) + (lbound))

#define CRAND_SUCCESS 0
#define CRAND_FAILURE 1
#define CRAND_ARG_FAILURE 2
#define CRAND_WRITE_FAILURE 3
#define CRAND_MALLOC_FAILURE 4

/* 
 * Fill n bytes of buf with cryptographically secure random values.
 * Returns RAND_SUCCESS or RAND_FAILURE.
 */
int crandb(void *buf, size_t n);

#define CRAND_PHRASE_WRITE_CHARSET          7
#define CRAND_PHRASE_WRITE_CHARSET_ALL      0
#define CRAND_PHRASE_WRITE_CHARSET_ALNUM    1
#define CRAND_PHRASE_WRITE_CHARSET_ALPHA    2
#define CRAND_PHRASE_WRITE_CHARSET_NOTALPHA 3
#define CRAND_PHRASE_WRITE_CHARSET_LOWER    4
#define CRAND_PHRASE_WRITE_CHARSET_UPPER    5
#define CRAND_PHRASE_WRITE_CHARSET_DIGITS   6
#define CRAND_PHRASE_WRITE_CHARSET_PUNCT    7

#define CRAND_PHRASE_WRITE_SEP             24
#define CRAND_PHRASE_WRITE_SEP_NEWLINE      0
#define CRAND_PHRASE_WRITE_SEP_SPACE        8
#define CRAND_PHRASE_WRITE_SEP_TAB         16
#define CRAND_PHRASE_WRITE_SEP_VTAB        24

/*
 * Write phrasen cryptographically secure phrases of length phraselen into dst.
 * Flags can be set to control the charset and separator between phrases
 * (CRAND_PHRASE_CHARSET_* and CRAND_PHRASE_SEP_* respectively, 0 for defaults).
 */
int crand_phrase_write(FILE *dst, size_t phraselen, size_t phrasen, int flags);

/* =============== */
#endif