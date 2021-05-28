#ifndef _META_H
/* =============== */

#ifndef _STDDEF_H
#include <stddef.h>
#endif

#define _META_H 1

#define PROGRAM_NAME \
    "spag"
#define VERSION \
    "0.0.3"
#define DESCRIPTION \
    "Generate random ASCII passphrases of set length (by default generate 1 phrase of length 16)."
#define AUTHORS \
    "Łukasz Dragon <lukasz.b.dragon@gmail.com>"
#define LICENSE \
    "This is free software. You may redistribute copies of it under the terms of\n" \
    "the GNU General Public License <https://www.gnu.org/licenses/gpl.html>.\n" \
    "There is NO WARRANTY, to the extent permitted by law."

#define FLAG_LENGTH    'l'
#define FLAG_NUM       'n'
#define FLAG_CHARSET   'c'
#define FLAG_SEPARATOR 's'
#define FLAG_HELP      'h'
#define FLAG_LICENSE   'L'
#define FLAG_VERSION   'V'

#define FLAGSMAX 256
static struct flag{
	char *name;
	char *description;
	char *value;
	unsigned int accepts_value: 1;
	unsigned int is_present : 1;
}flags[FLAGSMAX] = {
	[FLAG_LENGTH]    = {"length",    "Set the length of generated phrases.",   NULL, 1, 0},
	[FLAG_NUM]       = {"number",    "Set the number of phrases to generate.", NULL, 1, 0},
	[FLAG_CHARSET]   = {"charset",   
						"Change the available charset."
						"\n\t\tCharsets:"
						"\n\t\t\tall      -> Every printable char"
						"\n\t\t\talnum    -> Letters and digits"
						"\n\t\t\talpha    -> Letters"
						"\n\t\t\tnotalpha -> Digits and special chars"
						"\n\t\t\tlower    -> Lowercase letters"
						"\n\t\t\tupper    -> Uppercase letters"
						"\n\t\t\tdigits   -> Digits"
						"\n\t\t\tpunct    -> Special chars",                   NULL, 1, 0},
	[FLAG_SEPARATOR] = {"separator", 
						"Change the separator between phrases."
						"\n\t\tSeparators:"
						"\n\t\t\tnewline"
						"\n\t\t\tspace"
						"\n\t\t\ttab"
						"\n\t\t\tvtab",                                        NULL, 1, 0},
	[FLAG_HELP]      = {"help",      "Print this message.",                    NULL, 0, 0},
	[FLAG_LICENSE]   = {"license",   "Print license information.",             NULL, 0, 0},
	[FLAG_VERSION]   = {"version",   "Print version.",                         NULL, 0, 0},
};

/* =============== */
#endif