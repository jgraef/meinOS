/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
