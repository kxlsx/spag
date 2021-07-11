#include <stdlib.h>
#include <string.h>

#include "meta.h"
#include "crand.h"

#define eprintf(fmt, ...) fprintf(stderr, fmt, ## __VA_ARGS__)

/* Return values */
#define SUCCESS 0
#define ERR_CRAND_FAILURE  1
#define ERR_NO_VALUE_PROVIDED 2
#define ERR_UNRECOGNIZED_FLAG 4
#define ERR_UNRECOGNIZED_VALUE 8
#define ERR_UNRECOGNIZED_CHARSET 16
#define ERR_UNRECOGNIZED_SEP 32

int parse_flags(int argc, char *argv[]);
struct flag *parse_long_flag(char* flag_str);
struct flag *parse_short_flag(char flag_ch);
int parse_charset_str(char *charset_str);
int parse_sep_str(char *sep_str);

void print_help(void);
void print_license(void);
void print_version(void);

int main(int argc, char *argv[]){
	int rt;
	size_t phrase_length, phrase_num;
	unsigned charset, sep;

	if((rt = parse_flags(argc, argv)) != SUCCESS)
		return rt;

	if(FLAG(FLAG_HELP).is_present){
        print_help();
        return SUCCESS;
    }else if(FLAG(FLAG_LICENSE).is_present){
        print_license();
        return SUCCESS;
    }else if(FLAG(FLAG_VERSION).is_present){
        print_version();
        return SUCCESS;
    }

	phrase_length = DEFAULT_LENGTH;
	phrase_num = DEFAULT_NUM;
	charset = DEFAULT_CHARSET;
	sep = DEFAULT_SEP;
	if(FLAG(FLAG_LENGTH).is_present)
		phrase_length = strtoull(FLAG(FLAG_LENGTH).value, NULL, 0);
	if(FLAG(FLAG_NUM).is_present)
		phrase_num = strtoull(FLAG(FLAG_NUM).value, NULL, 0);
	if(FLAG(FLAG_CHARSET).is_present){
		charset = parse_charset_str(FLAG(FLAG_CHARSET).value);
		if(charset == ERR_UNRECOGNIZED_CHARSET){
			eprintf("Unrecognized charset \"%s\".\n", FLAG(FLAG_CHARSET).value);
			return ERR_UNRECOGNIZED_CHARSET;
		}
	}
	if(FLAG(FLAG_SEPARATOR).is_present){
		sep = parse_sep_str(FLAG(FLAG_SEPARATOR).value);
		if(charset == ERR_UNRECOGNIZED_SEP){
			eprintf("Unrecognized separator \"%s\".\n", FLAG(FLAG_SEPARATOR).value);
			return ERR_UNRECOGNIZED_SEP;
		}
	}

	rt = crand_phrase_write(
		stdout, 
		phrase_length, phrase_num, 
		charset | sep
	);
	switch(rt){
	case CRAND_FAILURE:
		eprintf("Failed to acquire random sequence.\n");
		break;
	case CRAND_ARG_FAILURE:
		eprintf("Incorrect phrase length or number.\n");
		break;
	case CRAND_WRITE_FAILURE:
		eprintf("Failed writing to stdin.\n");
		break;
	case CRAND_MALLOC_FAILURE:
		eprintf("Failed allocating memory.\n");
		break;
	}

	return rt;
}

int parse_flags(int argc, char *argv[]){
	char *flag_str;
	struct flag *fnd_flag;
	unsigned is_value_expected;
	int argi;

	fnd_flag = NULL;
	is_value_expected = 0;
	for(argi = 1; argi < argc; argi++){
        if(*(argv[argi]) == '-'){
			if(is_value_expected){
				eprintf("No value provided for \"%s\".\n", argv[argi - 1]);
				return ERR_NO_VALUE_PROVIDED;
			}

			flag_str = argv[argi] + 1;
			if(*flag_str == '-'){
				if((fnd_flag = parse_long_flag(flag_str++)) == NULL){
					eprintf("Unrecognized flag \"%s\".\n", argv[argi]);
					return ERR_UNRECOGNIZED_FLAG;
				}
				flag_str += strlen(fnd_flag->name) - 1;
			}else{
				if((fnd_flag = parse_short_flag(flag_str[0])) == NULL){
					eprintf("Unrecognized flag \"%s\".\n", argv[argi]);
					return ERR_UNRECOGNIZED_FLAG;
				}
			}
			fnd_flag->is_present = 1;
			if(fnd_flag->accepts_value){
				if(*(++flag_str) == '='){
					if(*(fnd_flag->value = flag_str + 1) == '\0'){
						eprintf("No value provided for \"%s\".\n", argv[argi]);
						return ERR_NO_VALUE_PROVIDED;
					}
				}else if(*flag_str != '\0'){
					fnd_flag->value = flag_str;
				}else{
					is_value_expected = 1;
				}
			}
        }else{
			if(is_value_expected){
				fnd_flag->value = argv[argi];
				is_value_expected = 0;
			}else{
				eprintf("Unrecognized argument \"%s\".\n", argv[argi]);
				return ERR_UNRECOGNIZED_VALUE;
			}
		}
	}
	if(is_value_expected){
		eprintf("No value provided for \"%s\".\n", argv[argi - 1]);
		return ERR_NO_VALUE_PROVIDED;
	}
	return SUCCESS;
}

struct flag *parse_short_flag(char flag_ch){
	if(FLAG(flag_ch).ch != '\0'){
		return FLAGS + FLAG_INDEX(flag_ch);
	}
	return NULL;
}

struct flag *parse_long_flag(char* flag_str){
	for(unsigned flagi = 0; flagi < FLAGSMAX; flagi++){
		if(FLAGS[flagi].ch != '\0'
		&& strstr(flag_str, FLAGS[flagi].name) != NULL){
			return FLAGS + flagi;
		}
	}
	return NULL;
}

int parse_charset_str(char *charset_str){
	if(strcmp(charset_str, "all") == 0)
		return CRAND_PHRASE_WRITE_CHARSET_ALL;
	if(strcmp(charset_str, "alnum") == 0)
		return CRAND_PHRASE_WRITE_CHARSET_ALNUM;
	if(strcmp(charset_str, "alpha") == 0)
		return CRAND_PHRASE_WRITE_CHARSET_ALPHA;
	if(strcmp(charset_str, "notalpha") == 0)
		return CRAND_PHRASE_WRITE_CHARSET_NOTALPHA;
	if(strcmp(charset_str, "lower") == 0)
		return CRAND_PHRASE_WRITE_CHARSET_LOWER;
	if(strcmp(charset_str, "upper") == 0)
		return CRAND_PHRASE_WRITE_CHARSET_UPPER;
	if(strcmp(charset_str, "digits") == 0)
		return CRAND_PHRASE_WRITE_CHARSET_DIGITS;
	if(strcmp(charset_str, "punct") == 0)
		return CRAND_PHRASE_WRITE_CHARSET_PUNCT;
	return ERR_UNRECOGNIZED_CHARSET;
}

int parse_sep_str(char *sep_str){
	if(strcmp(sep_str, "newline") == 0)
		return CRAND_PHRASE_WRITE_SEP_NEWLINE;
	if(strcmp(sep_str, "space") == 0)
		return CRAND_PHRASE_WRITE_SEP_SPACE;
	if(strcmp(sep_str, "tab") == 0)
		return CRAND_PHRASE_WRITE_SEP_TAB;
	if(strcmp(sep_str, "vtab") == 0)
		return CRAND_PHRASE_WRITE_SEP_VTAB;
	return ERR_UNRECOGNIZED_SEP;
}

void print_help(void){
    puts(
        PROGRAM_NAME " v" VERSION "\n"
        AUTHORS "\n"
        DESCRIPTION "\n"
        "\n"
        "USAGE:"
    );
    printf("\t" PROGRAM_NAME " [FLAGS]=<VALUES>");
    puts(
        "\n" 
        "FLAGS:"
    );
    for(unsigned flagi = 0; flagi < FLAGSMAX; flagi++){
        if(FLAGS[flagi].ch != '\0'){
            printf("\t-%c, --%s\n", FLAGS[flagi].ch, FLAGS[flagi].name);
            printf("\t\t%s\n\n", FLAGS[flagi].description);
        }
    }
}

void print_license(void){
    puts(
        PROGRAM_NAME " v" VERSION "\n"
		AUTHORS "\n"
		"\n"
        LICENSE
    );
}

void print_version(void){
    puts(
        PROGRAM_NAME " v" VERSION
    );
}
