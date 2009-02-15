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

#include <ctype.h>

int iscntrl(int c) {
  return c<' ';
}

int isdigit(int c) {
  return c>='0' && c<='9';
}

int islower(int c) {
  return c>='a' && c<='z';
}

int isprint(int c) {
  return c>=' ';
}

int isspace(int c) {
  return c==' ' || c=='\f' || c=='\n' || c=='\r' || c=='\t' || c=='\v';
}

int isblank(int c) {
  return isspace(c);
}

int isupper(int c) {
  return c>='A' && c<='Z';
}

int isxdigit(int c) {
  return isdigit(c) || (c>='A' && c<='F') || (c>='a' && c<='f');
}

int isalpha(int c) {
  return isupper(c) || islower(c);
}

int isalnum(int c) {
  return isalpha(c) || isdigit(c);
}

int isgraph(int c) {
  return isalnum(c) && c!=' '; /** @todo please do it the right way */
}

int ispunct(int c) {
  return isprint(c) && !isalnum(c); /** @todo is that right? **/
}

char tolower(int c) {
  return isupper(c)?c+('a'-'A'):c;
}

char toupper(int c) {
  return islower(c)?c-('a'-'A'):c;
}
