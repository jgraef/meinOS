/* Copyright (C) 1999 Free Software Foundation, Inc.
   This file is part of the GNU UTF-8 Library.

   The GNU UTF-8 Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU UTF-8 Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU UTF-8 Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <wchar.h>

wchar_t* wcpcpy (wchar_t* dest, const wchar_t* src)
{
  for (; (*dest = *src) != (wchar_t)'\0'; src++, dest++);
  return dest;
}

wchar_t* wcpncpy (wchar_t* dest, const wchar_t* src, size_t n)
{
  for (; n > 0 && (*dest = *src) != (wchar_t)'\0'; src++, dest++, n--);

  /* This behavior is rarely useful, but it is here for consistency with
     wcsncpy. */
  for (; n > 0; n--)
    *dest++ = (wchar_t)'\0';

  return dest-1;
}

wchar_t* wcscat (wchar_t* dest, const wchar_t* src)
{
  wchar_t* destptr = dest + wcslen(dest);

  for (; (*destptr = *src) != (wchar_t)'\0'; src++, destptr++);
  return dest;
}

wchar_t* wcschr (const wchar_t* wcs, wchar_t wc)
{
  for (;; wcs++) {
    if (*wcs == wc)
      break;
    if (*wcs == (wchar_t)'\0')
      goto notfound;
  }
  return (wchar_t*) wcs;
notfound:
  return NULL;
}

int wcscmp (const wchar_t* s1, const wchar_t* s2)
{
  for (;;) {
    wchar_t wc1 = *s1++;
    wchar_t wc2 = *s2++;
    if (wc1 != (wchar_t)'\0' && wc1 == wc2)
      continue;
    /* Note that wc1 and wc2 each have at most 31 bits. */
    return (int)wc1 - (int)wc2;
    /* > 0 if wc1 > wc2, < 0 if wc1 < wc2, = 0 if wc1 and wc2 are both '\0'. */
  }
}

wchar_t* wcscpy (wchar_t* dest, const wchar_t* src)
{
  wchar_t* destptr = dest;

  for (; (*destptr = *src) != (wchar_t)'\0'; src++, destptr++);
  return dest;
}

size_t wcscspn (const wchar_t* wcs, const wchar_t* reject)
{
  /* Optimize two cases. */
  if (reject[0] == (wchar_t)'\0')
    return wcslen(wcs);
  if (reject[1] == (wchar_t)'\0') {
    wchar_t wc = reject[0];
    const wchar_t* ptr = wcs;
    for (; *ptr != (wchar_t)'\0'; ptr++) {
      if (*ptr == wc)
        break;
    }
    return ptr-wcs;
  }
  /* General case. */
  {
    const wchar_t* ptr = wcs;
    for (; *ptr != (wchar_t)'\0'; ptr++) {
      if (wcschr(reject, *ptr))
        break;
    }
    return ptr-wcs;
  }
}

wchar_t* wcsdup (const wchar_t* s)
{
  size_t n = wcslen(s) + 1;
  wchar_t* copy = (wchar_t*) malloc (n * sizeof(wchar_t));
  if (copy != NULL)
    return wmemcpy(copy, s, n);
  else
    return NULL;
}

size_t wcslen (const wchar_t* s)
{
  const wchar_t* ptr;

  for (ptr = s; *ptr != (wchar_t)'\0'; ptr++);
  return ptr - s;
}

wchar_t* wcsncat (wchar_t* dest, const wchar_t* src, size_t n)
{
  wchar_t* destptr = dest + wcslen(dest);

  for (; n > 0 && (*destptr = *src) != (wchar_t)'\0'; src++, destptr++, n--);
  if (n == 0)
    *destptr = (wchar_t)'\0';
  return dest;
}

int wcsncmp (const wchar_t* s1, const wchar_t* s2, size_t n)
{
  for (; n > 0;) {
    wchar_t wc1 = *s1++;
    wchar_t wc2 = *s2++;
    if (wc1 != (wchar_t)'\0' && wc1 == wc2) {
      n--;
      continue;
    }
    /* Note that wc1 and wc2 each have at most 31 bits. */
    return (int)wc1 - (int)wc2;
    /* > 0 if wc1 > wc2, < 0 if wc1 < wc2, = 0 if wc1 and wc2 are both '\0'. */
  }
  return 0;
}

wchar_t* wcsncpy (wchar_t* dest, const wchar_t* src, size_t n)
{
  wchar_t* destptr = dest;

  for (; n > 0 && (*destptr = *src) != (wchar_t)'\0'; src++, destptr++, n--);

  /* This behavior is rarely useful, but it is specified by the ISO C
     standard. */
  for (; n > 0; n--)
    *destptr++ = (wchar_t)'\0';

  return dest;
}

size_t wcsnlen (const wchar_t* s, size_t maxlen)
{
  const wchar_t* ptr;

  for (ptr = s; maxlen > 0 && *ptr != (wchar_t)'\0'; ptr++, maxlen--);
  return ptr - s;
}

wchar_t* wcspbrk (const wchar_t* wcs, const wchar_t* accept)
{
  /* Optimize two cases. */
  if (accept[0] == (wchar_t)'\0')
    return NULL;
  if (accept[1] == (wchar_t)'\0') {
    wchar_t wc = accept[0];
    const wchar_t* ptr = wcs;
    for (; *ptr != (wchar_t)'\0'; ptr++) {
      if (*ptr == wc)
        return (wchar_t*) ptr;
    }
    return NULL;
  }
  /* General case. */
  {
    const wchar_t* ptr = wcs;
    for (; *ptr != (wchar_t)'\0'; ptr++) {
      if (wcschr(accept, *ptr))
        return (wchar_t*) ptr;
    }
    return NULL;
  }
}

wchar_t* wcsrchr (const wchar_t* wcs, wchar_t wc)
{
  /* Calling wcslen and then searching from the other end would cause more
     memory accesses. Avoid that, at the cost of a few more comparisons. */
  wchar_t* result = NULL;

  for (;; wcs++) {
    if (*wcs == wc)
      result = (wchar_t*) wcs;
    if (*wcs == (wchar_t)'\0')
      break;
  }
  return result;
}

size_t wcsspn (const wchar_t* wcs, const wchar_t* accept)
{
  /* Optimize two cases. */
  if (accept[0] == (wchar_t)'\0')
    return 0;
  if (accept[1] == (wchar_t)'\0') {
    wchar_t wc = accept[0];
    const wchar_t* ptr = wcs;
    for (; *ptr != (wchar_t)'\0'; ptr++) {
      if (*ptr != wc)
        break;
    }
    return ptr-wcs;
  }
  /* General case. */
  {
    const wchar_t* ptr = wcs;
    for (; *ptr != (wchar_t)'\0'; ptr++) {
      if (!wcschr(accept, *ptr))
        break;
    }
    return ptr-wcs;
  }
}

wchar_t* wcsstr (const wchar_t* haystack, const wchar_t* needle)
{
  wchar_t n = needle[0];
  /* is needle empty? */
  if (n == (wchar_t)'\0')
    return (wchar_t*) haystack;
  /* is needle nearly empty? */
  if (needle[1] == (wchar_t)'\0')
    return wcschr(haystack, n);
  /* search for needle's first character */
  for (; *haystack != (wchar_t)'\0'; haystack++) {
    if (*haystack == n) {
      /* compare with needle's remaining characters */
      const wchar_t* hptr = haystack+1;
      const wchar_t* nptr = needle+1;
      for (;;) {
        if (*hptr != *nptr)
          break;
        hptr++; nptr++;
        if (*nptr == (wchar_t)'\0')
          return (wchar_t*) haystack;
      }
    }
  }
  return NULL;
}

wchar_t* wcstok (wchar_t* wcs, const wchar_t* delim, wchar_t** ptr)
{
  if (wcs == NULL) {
    wcs = *ptr;
    if (wcs == NULL)
      return NULL; /* reminder that end of token sequence has been reached */
  }
  /* Skip leading delimiters. */
  wcs += wcsspn(wcs, delim);
  /* Found a token? */
  if (*wcs == (wchar_t)'\0') {
    *ptr = NULL;
    return NULL;
  }
  /* Move past the token. */
  {
    wchar_t* token_end = wcspbrk(wcs, delim);
    if (token_end) {
      /* NUL-terminate the token. */
      *token_end = (wchar_t)'\0';
      *ptr = token_end+1;
    } else
      *ptr = NULL;
  }
  return wcs;
}

wchar_t* wmemchr (const wchar_t* s, wchar_t c, size_t n)
{
  for (; n > 0; s++, n--) {
    if (*s == c)
      return (wchar_t*) s;
  }
  return NULL;
}

int wmemcmp (const wchar_t* s1, const wchar_t* s2, size_t n)
{
  for (; n > 0;) {
    wchar_t wc1 = *s1++;
    wchar_t wc2 = *s2++;
    if (wc1 == wc2) {
      n--;
      continue;
    }
    /* Note that wc1 and wc2 each have at most 31 bits. */
    return (int)wc1 - (int)wc2;
    /* > 0 if wc1 > wc2, < 0 if wc1 < wc2. */
  }
  return 0;
}

wchar_t* wmemcpy (wchar_t* dest, const wchar_t* src, size_t n)
{
  wchar_t* destptr = dest;

  for (; n > 0; n--)
    *destptr++ = *src++;
  return dest;
}

wchar_t* wmemmove (wchar_t* dest, const wchar_t* src, size_t n)
{
  if (dest < src) {
    wchar_t* destptr = dest;
    const wchar_t* srcptr = src;
    for (; n > 0; n--)
      *destptr++ = *srcptr++;
  } else if (dest > src) {
    wchar_t* destptr = dest + n - 1;
    const wchar_t* srcptr = src + n - 1;
    for (; n > 0; n--)
      *destptr-- = *srcptr--;
  }
  return dest;
}

wchar_t* wmemset (wchar_t* s, wchar_t c, size_t n)
{
  wchar_t* ptr = s;

  for (; n > 0; n--)
    *ptr++ = c;
  return s;
}
