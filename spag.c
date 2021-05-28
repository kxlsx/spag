#include <stdlib.h>
#include <string.h>

#include "meta.h"
#include "crand.h"

#define eprintf(fmt, ...) fprintf(stderr, fmt, ## __VA_ARGS__)

#define SUCCESS 0
#define ERR_CRAND_FAILURE  1
#define ERR_NO_VALUE_PROVIDED 2
#define ERR_UNRECOGNIZED_FLAG 4
#define ERR_UNRECOGNIZED_VALUE 8
#define ERR_UNRECOGNIZED_CHARSET 16
#define ERR_UNRECOGNIZED_SEP 32

#define DEFAULT_LENGTH 16
#define DEFAULT_NUM 1
#define DEFAULT_CHARSET CRAND_PHRASE_WRITE_CHARSET_ALL
#define DEFAULT_SEP CRAND_PHRASE_WRITE_SEP_NEWLINE

int read_flags(int argc, char *argv[]);
int parse_charset_str(char *charset_str);
int parse_sep_str(char *sep_str);
void print_help(void);
void print_license(void);
void print_version(void);

int main(int argc, char *argv[]){
	int rt;
	size_t phrase_length, phrase_num;
	unsigned charset, sep;

	if((rt = read_flags(argc, argv)) != SUCCESS)
		return rt;

	if(flags[FLAG_HELP].is_present){
        print_help();
        return SUCCESS;
    }else if(flags[FLAG_LICENSE].is_present){
        print_license();
        return SUCCESS;
    }else if(flags[FLAG_VERSION].is_present){
        print_version();
        return SUCCESS;
    }

	phrase_length = DEFAULT_LENGTH;
	phrase_num = DEFAULT_NUM;
	charset = DEFAULT_CHARSET;
	sep = DEFAULT_SEP;
	if(flags[FLAG_LENGTH].is_present)
		phrase_length = strtoull(flags[FLAG_LENGTH].value, NULL, 0);
	if(flags[FLAG_NUM].is_present)
		phrase_num = strtoull(flags[FLAG_NUM].value, NULL, 0);
	if(flags[FLAG_CHARSET].is_present){
		charset = parse_charset_str(flags[FLAG_CHARSET].value);
		if(charset == ERR_UNRECOGNIZED_CHARSET){
			eprintf("Unrecognized charset \"%s\".\n", flags[FLAG_CHARSET].value);
			return ERR_UNRECOGNIZED_CHARSET;
		}
	}
	if(flags[FLAG_SEPARATOR].is_present){
		sep = parse_sep_str(flags[FLAG_SEPARATOR].value);
		if(charset == ERR_UNRECOGNIZED_SEP){
			eprintf("Unrecognized separator \"%s\".\n", flags[FLAG_SEPARATOR].value);
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

int read_flags(int argc, char *argv[]){
	char *flag_str;
	struct flag *flag;
	unsigned is_value_expected;
	size_t i;

	flag = NULL;
	is_value_expected = 0;
	for(i = 1; i < (size_t)argc; i++){
        if(*(argv[i]) == '-'){
			if(is_value_expected){
				eprintf("No value provided for \"%s\".\n", argv[i - 1]);
				return ERR_NO_VALUE_PROVIDED;
			}

			flag_str = argv[i] + 1;
			if(*flag_str == '-'){
				flag_str++;
				for(size_t j = 0; j < FLAGSMAX; j++){
					if(flags[j].name != NULL){
						if(strstr(flag_str, flags[j].name) != NULL){
							flag = flags + j;
							flag_str += strlen(flag->name) - 1;
						}
					}
				}
				if(*(flag_str - 1) == '-'){
					eprintf("Unrecognized flag \"%s\".\n", argv[i]);
					return ERR_UNRECOGNIZED_FLAG;
				}

			}else{
				if(flags[(size_t)*flag_str].name != NULL){
					flag = flags + *flag_str;
				}else{
					eprintf("Unrecognized flag \"%s\".\n", argv[i]);
					return ERR_UNRECOGNIZED_FLAG;
				}
			}
			flag->is_present = 1;
			if(flag->accepts_value){
				if(*(++flag_str) == '='){
					if(*(flag->value = flag_str + 1) == '\0'){
						eprintf("No value provided for \"%s\".\n", argv[i]);
						return ERR_NO_VALUE_PROVIDED;
					}
				}else if(*flag_str != '\0'){
					flag->value = flag_str;
				}else{
					is_value_expected = 1;
				}
			}
        }else{
			if(is_value_expected){
				flag->value = argv[i];
				is_value_expected = 0;
			}else{
				eprintf("Unrecognized argument \"%s\".\n", argv[i]);
				return ERR_UNRECOGNIZED_VALUE;
			}
		}
	}
	if(is_value_expected){
		eprintf("No value provided for \"%s\".\n", argv[i - 1]);
		return ERR_NO_VALUE_PROVIDED;
	}
	return SUCCESS;
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
    size_t i;

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
    for(i = 0; i < FLAGSMAX; i++){
        if(flags[i].name != NULL){
            printf("\t-%c, --%s\n", (char)i, flags[i].name);
            printf("\t\t%s\n\n", flags[i].description);
        }
    }
}

void print_license(void){
    puts(
        PROGRAM_NAME " v" VERSION "\n"
        LICENSE
    );
}

void print_version(void){
    puts(
        PROGRAM_NAME " v" VERSION
    );
}
