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

#ifndef _WCHAR_H_
#define _WCHAR_H_

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>

typedef wchar_t wint_t;

wint_t btowc(int c);

wchar_t* wcpcpy (wchar_t* dest, const wchar_t* src);
wchar_t* wcpncpy (wchar_t* dest, const wchar_t* src, size_t n);
wchar_t* wcscat (wchar_t* dest, const wchar_t* src);
wchar_t* wcschr (const wchar_t* wcs, wchar_t wc);
int wcscmp (const wchar_t* s1, const wchar_t* s2);
wchar_t* wcscpy (wchar_t* dest, const wchar_t* src);
size_t wcscspn (const wchar_t* wcs, const wchar_t* reject);
wchar_t* wcsdup (const wchar_t* s);
size_t wcslen (const wchar_t* s);
wchar_t* wcsncat (wchar_t* dest, const wchar_t* src, size_t n);
int wcsncmp (const wchar_t* s1, const wchar_t* s2, size_t n);
wchar_t* wcsncpy (wchar_t* dest, const wchar_t* src, size_t n);
size_t wcsnlen (const wchar_t* s, size_t maxlen);
wchar_t* wcspbrk (const wchar_t* wcs, const wchar_t* accept);
wchar_t* wcsrchr (const wchar_t* wcs, wchar_t wc);
size_t wcsspn (const wchar_t* wcs, const wchar_t* accept);
wchar_t* wcsstr (const wchar_t* haystack, const wchar_t* needle);
wchar_t* wcstok (wchar_t* wcs, const wchar_t* delim, wchar_t** ptr);
wchar_t* wmemchr (const wchar_t* s, wchar_t c, size_t n);
int wmemcmp (const wchar_t* s1, const wchar_t* s2, size_t n);
wchar_t* wmemcpy (wchar_t* dest, const wchar_t* src, size_t n);
wchar_t* wmemmove (wchar_t* dest, const wchar_t* src, size_t n);
wchar_t* wmemset (wchar_t* s, wchar_t c, size_t n);

#endif
