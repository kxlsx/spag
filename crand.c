#include <stdio.h>
#include <stdlib.h>

#include "crand.h"

#ifdef _WIN32
	#include <windows.h>
	#include <ntsecapi.h>

	int rand_bytes(void *buf, size_t n){
		if(RtlGenRandom(buf, n) == FALSE)
			return CRAND_FAILURE;
		return CRAND_SUCCESS;
	}
#else
	#include <sys/random.h>

	int rand_bytes(void *buf, size_t n){
		if((size_t)getrandom(buf, n, 0) != n)
			return CRAND_FAILURE;
		return CRAND_SUCCESS;
	}
#endif

int crandb(void *buf, size_t n){
	if(buf == NULL || n == 0)
		return CRAND_ARG_FAILURE;
	return rand_bytes(buf, n);
};

#define CHARSET_MAX 93
const char CHARSET[CHARSET_MAX] = 
	"abcdefghijklmnopqrstuvwxyz"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"0123456789"
	"!?@#$%&()[]{}<>*+-^=/\\|_.,:;\'\"`";

struct charset_bound{
	int lower;
	int higher;
};
#define CHARSET_BOUND_ALL      { 0, CHARSET_MAX}
#define CHARSET_BOUND_ALNUM    { 0,          62}
#define CHARSET_BOUND_ALPHA    { 0,          52}
#define CHARSET_BOUND_NOTALPHA {52, CHARSET_MAX}
#define CHARSET_BOUND_LOWER    { 0,          26}
#define CHARSET_BOUND_UPPER    {26,          52}
#define CHARSET_BOUND_DIGITS   {52,          62}
#define CHARSET_BOUND_PUNCT    {62, CHARSET_MAX}

int crand_phrase_write(FILE *dst, size_t phraselen, size_t phrasen, int flags){
	void set_crand_phrase_write_params(int flags, struct charset_bound *chb, char *sep);
	char sep;
	struct charset_bound chb;
	unsigned int *buf;
	size_t buf_len, buf_size;
	size_t i;

	if(!(phraselen && phrasen))
		return CRAND_ARG_FAILURE;
	set_crand_phrase_write_params(flags, &chb, &sep);
	
	buf_len = phrasen * phraselen;
 	buf_size = buf_len * sizeof(unsigned int);
	if((buf = malloc(buf_size)) == NULL)
		return CRAND_MALLOC_FAILURE;

	if(crandb(buf, buf_size) == CRAND_FAILURE)
		return CRAND_FAILURE;

	i = 0;
	while(i < buf_len){
		putc(CHARSET[LIMIT_TO_RANGE(buf[i++], chb.lower, chb.higher)], dst);
		if(i % phraselen == 0) putc(sep, dst);
	}

	if(ferror(dst) != 0)
		return CRAND_WRITE_FAILURE;

	free(buf);
	return CRAND_SUCCESS;
}

/* Set charset bound and separator based on flags */
void set_crand_phrase_write_params(int flags, struct charset_bound *chb, char *sep){
	switch(flags & CRAND_PHRASE_WRITE_CHARSET){
	case CRAND_PHRASE_WRITE_CHARSET_ALL:
		*chb = 
			(struct charset_bound)CHARSET_BOUND_ALL;
		break;
	case CRAND_PHRASE_WRITE_CHARSET_ALNUM:
		*chb =
			(struct charset_bound)CHARSET_BOUND_ALNUM;
		break;
	case CRAND_PHRASE_WRITE_CHARSET_ALPHA:
		*chb = 
			(struct charset_bound)CHARSET_BOUND_ALPHA;
		break;
	case CRAND_PHRASE_WRITE_CHARSET_NOTALPHA:
		*chb= 
			(struct charset_bound)CHARSET_BOUND_NOTALPHA;
		break;
	case CRAND_PHRASE_WRITE_CHARSET_LOWER:
		*chb = 
			(struct charset_bound)CHARSET_BOUND_LOWER;
		break;
	case CRAND_PHRASE_WRITE_CHARSET_UPPER:
		*chb = 
			(struct charset_bound)CHARSET_BOUND_UPPER;
		break;
	case CRAND_PHRASE_WRITE_CHARSET_DIGITS:
		*chb = 
			(struct charset_bound)CHARSET_BOUND_DIGITS;
		break;
	case CRAND_PHRASE_WRITE_CHARSET_PUNCT:
		*chb = 
			(struct charset_bound)CHARSET_BOUND_PUNCT;
		break;
	}

	switch(flags & CRAND_PHRASE_WRITE_SEP){
		case CRAND_PHRASE_WRITE_SEP_NEWLINE:
			*sep = '\n';
			break;
		case CRAND_PHRASE_WRITE_SEP_SPACE:
			*sep = ' ';
			break;
		case CRAND_PHRASE_WRITE_SEP_TAB:
			*sep = '\t';
			break;
		case CRAND_PHRASE_WRITE_SEP_VTAB:
			*sep = '\v';
			break;
	}
}
