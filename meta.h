#ifndef META_H
/* =============== */

#ifndef _STDDEF_H
#include <stddef.h>
#endif

#include "crand.h"

#define META_H 1

/* Default behaviour. */
#define DEFAULT_LENGTH  16
#define DEFAULT_NUM      1
#define DEFAULT_CHARSET CRAND_PHRASE_WRITE_CHARSET_ALL
#define DEFAULT_SEP     CRAND_PHRASE_WRITE_SEP_NEWLINE

/* Completely vain stuff. */
#define STR(macro) #macro
#define XSTR(macro) STR(macro)
#if DEFAULT_NUM > 1
#define PHRASE_NUM_STR XSTR(DEFAULT_NUM phrases)
#else
#define PHRASE_NUM_STR XSTR(DEFAULT_NUM phrase)
#endif
#define PHRASE_LENGTH_STR XSTR(DEFAULT_LENGTH)

/* Metadata */
#define PROGRAM_NAME \
    "spag"
#define VERSION \
    "0.0.6"
#define DESCRIPTION \
    "Generate random ASCII passphrases of set length " \
	"(by default generate " PHRASE_NUM_STR " of length " PHRASE_LENGTH_STR ")."
#define AUTHORS \
    "Łukasz Dragon <lukasz.b.dragon@gmail.com>"
#define LICENSE \
    "This is free software. You may redistribute copies of it under the terms of\n" \
    "the GNU General Public License <https://www.gnu.org/licenses/gpl.html>.\n"     \
    "There is NO WARRANTY, to the extent permitted by law."

/* Flag definitions */
#define FLAG_LENGTH    'l'
#define FLAG_NUM       'n'
#define FLAG_CHARSET   'c'
#define FLAG_SEPARATOR 's'
#define FLAG_HELP      'h'
#define FLAG_LICENSE   'L'
#define FLAG_VERSION   'V'

#define FLAGSMAX 64
#define FLAG_INDEX(flag_char) \
	flag_char % FLAGSMAX
#define FLAG_INIT(flag_char, flag_name, flag_description, flag_accepts_value) \
	[FLAG_INDEX(flag_char)] = { \
		flag_char,              \
		flag_name,              \
		flag_description,       \
		NULL,                   \
		flag_accepts_value,     \
		0                       \
	}
static struct flag{
	char ch;
	char *name;
	char *description;
	char *value;
	unsigned accepts_value: 1;
	unsigned is_present : 1;
}FLAGS[FLAGSMAX] = {
	FLAG_INIT(FLAG_LENGTH,    "length",    "Set the length of generated phrases.",   1),
	FLAG_INIT(FLAG_NUM,       "number",    "Set the number of phrases to generate.", 1),
	FLAG_INIT(FLAG_CHARSET,   "charset",   
		"Change the available charset."
		"\n\t\tCharsets:"
		"\n\t\t\tall      -> Every printable char"
		"\n\t\t\talnum    -> Letters and digits"
		"\n\t\t\talpha    -> Letters"
		"\n\t\t\tnotalpha -> Digits and special chars"
		"\n\t\t\tlower    -> Lowercase letters"
		"\n\t\t\tupper    -> Uppercase letters"
		"\n\t\t\tdigits   -> Digits"
		"\n\t\t\tpunct    -> Special chars",                                         1),
	FLAG_INIT(FLAG_SEPARATOR, "separator", 
		"Change the separator between phrases."
		"\n\t\tSeparators:"
		"\n\t\t\tnewline"
		"\n\t\t\tspace"
		"\n\t\t\ttab"
		"\n\t\t\tvtab",                                                              1),
	FLAG_INIT(FLAG_HELP,      "help",      "Print this message.",                    0),
	FLAG_INIT(FLAG_LICENSE,   "license",   "Print license information.",             0),
	FLAG_INIT(FLAG_VERSION,   "version",   "Print version.",                         0),
};

#define FLAG(flag_char) \
	FLAGS[FLAG_INDEX(flag_char)]

/* =============== */
#endif