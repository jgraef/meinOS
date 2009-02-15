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

#ifndef _STRING_H_
#define _STRING_H_

#include <sys/types.h>
#include <stdint.h>

void *memccpy(void *dest,const void *src,int c,size_t n);
void *memchr(const void *src,int c,size_t n);
int memcmp(const void *src1,const void *src2,size_t n);
void *memcpy(void *dest,const void *src,size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *dest,int val,size_t n);
char *strcat(char *str1,const char *str2);
char *strchr(const char *_str,int chr);
int strcmp(const char *src1,const char *src2);
char *strcpy(char *dest,const char *src);
char *strdup(const char *src);
char *strerror(int errnum);
size_t strlen(const char *str);
char *strncat(char *src1,const char *src2,size_t n);
int strncmp(const char *src1,const char *src2,size_t n);
char *strncpy(char *dest,const char *src,size_t n);
char *strsep(char **stringp, const char *delim);
char *strtok(char *s, const char *delim);
char *strpbrk(const char *s, const char *accept);
uintmax_t strntoumax(const char *nptr, char **endptr, int base, size_t n);
char *strrchr(const char *s, int c);
size_t strspn(const char *s,const char *accept);
size_t strcspn(const char *s,const char *reject);
void *mempcpy(void *dest,const void *src,size_t n);
char *stpcpy(char *dest,const char *src);
void *memmem(const void *haystack,size_t n,const void *needle,size_t m);
char *strstr(const char *haystack,const char *needle);
void memswap(void *m1,void *m2,size_t n);

#endif
