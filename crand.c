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
#elif defined __unix__
	#define URANDOM_PATH "/dev/urandom"
	static FILE *urandom = NULL;

	void urandom_close(){
		fclose(urandom);
	}

	int rand_bytes(void *buf, size_t n){
		if(urandom == NULL){
			if((urandom = fopen(URANDOM_PATH, "r")) == NULL)
				return CRAND_FAILURE;
			else
				atexit(urandom_close);
		}
		
		if(fread(buf, 1, n, urandom) != n)
			return CRAND_FAILURE;
		return CRAND_SUCCESS;
	}
#else
	#error Unsupported platform.
#endif

int crandb(void *buf, size_t n){
	if(buf == NULL || n == 0)
		return CRAND_ARG_FAILURE;
	return rand_bytes(buf, n);
};

int crand_phrase_write(FILE *dst, size_t phraselen, size_t phrasen, int flags){
	void set_crand_phrase_write_params(int flags, struct charset_bound *chb, char *sep);
	char sep;
	struct charset_bound chb;
	unsigned char *buf;
	size_t buf_len;
	size_t i;

	if(!(phraselen && phrasen))
		return CRAND_ARG_FAILURE;
	set_crand_phrase_write_params(flags, &chb, &sep);
	
	buf_len = phrasen * phraselen;
	if((buf = malloc(buf_len)) == NULL)
		return CRAND_MALLOC_FAILURE;

	if(crandb(buf, buf_len) == CRAND_FAILURE){
		free(buf);
		return CRAND_FAILURE;
	}

	i = 0;
	while(i < buf_len){
		putc(CHARSET[LIMIT_TO_RANGE(buf[i++], chb.lower, chb.higher)], dst);
		if(i % phraselen == 0) putc(sep, dst);
	}

	if(ferror(dst) != 0){
		free(buf);
		return CRAND_WRITE_FAILURE;
	}

	free(buf);
	return CRAND_SUCCESS;
}

/* Set charset bound and separator based on flags */
void set_crand_phrase_write_params(int flags, struct charset_bound *chb, char *sep){
	switch(flags & CRAND_PHRASE_WRITE_CHARSET){
	case CRAND_PHRASE_WRITE_CHARSET_ALL:
		*chb = CHARSET_BOUND_ALL;
		break;
	case CRAND_PHRASE_WRITE_CHARSET_ALNUM:
		*chb =CHARSET_BOUND_ALNUM;
		break;
	case CRAND_PHRASE_WRITE_CHARSET_ALPHA:
		*chb = CHARSET_BOUND_ALPHA;
		break;
	case CRAND_PHRASE_WRITE_CHARSET_NOTALPHA:
		*chb= CHARSET_BOUND_NOTALPHA;
		break;
	case CRAND_PHRASE_WRITE_CHARSET_LOWER:
		*chb = CHARSET_BOUND_LOWER;
		break;
	case CRAND_PHRASE_WRITE_CHARSET_UPPER:
		*chb = CHARSET_BOUND_UPPER;
		break;
	case CRAND_PHRASE_WRITE_CHARSET_DIGITS:
		*chb = CHARSET_BOUND_DIGITS;
		break;
	case CRAND_PHRASE_WRITE_CHARSET_PUNCT:
		*chb = CHARSET_BOUND_PUNCT;
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
