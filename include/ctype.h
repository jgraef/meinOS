#ifndef _CTYPE_H_
#define _CTYPE_H_

#define isalnum(c)  (isalpha(c) || isdigit(c))
#define isalpha(c)  (isupper(c) || islower(c))
#define iscntrl(c)  (c<' ')
#define isdigit(c)  (c>='0' && c<='9')
#define isgraph(c)  (isalnum(c) && c!=' ') /** @todo please do it the right way */
#define islower(c)  (c>='a' && c<='z')
#define isprint(c)  (c>=' ')
#define ispunct(c)  (isprint(c) && !isalnum(c)) /** @todo is that right? **/
#define isspace(c)  (c==' ' || c=='\f' || c=='\n' || c=='\r' || c=='\t' || c=='\v')
#define isupper(c)  (c>='A' && c<='Z')
#define isxdigit(c) (isdigit(c) || (c>='A' && c<='F') || (c>='a' && c<='f'))

#define tolower(c)  (isupper(c)?c+('a'-'A'):c)
#define toupper(c)  (islower(c)?c-('a'-'A'):c)

#endif
