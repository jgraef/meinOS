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

#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <ctype.h>

static const char *errno_strings[] = {
  "No Error",
  "Argument list too long.",
  "Permission denied.",
  "Address in use.",
  "Address not available.",
  "Address family not supported.",
  "Resource unavailable, try again.",
  "Connection already in progress.",
  "Bad file descriptor.",
  "Bad message.",
  "Device or resource busy.",
  "Operation canceled.",
  "No child processes.",
  "Connection aborted.",
  "Connection refused.",
  "Connection reset.",
  "Resource deadlock would occur.",
  "Destination address required.",
  "Mathematics argument out of domain of function.",
  "Reserved.",
  "File exists.",
  "Bad address.",
  "File too large.",
  "Host is unreachable.",
  "Identifier removed.",
  "Illegal byte sequence.",
  "Operation in progress.",
  "Interrupted function.",
  "Invalid argument.",
  "I/O error.",
  "Socket is connected.",
  "Is a directory.",
  "Too many levels of symbolic links.",
  "Too many open files.",
  "Too many links.",
  "Message too large.",
  "Reserved.",
  "Filename too long.",
  "Network is down.",
  "Connection aborted by network.",
  "Network unreachable.",
  "Too many files open in system.",
  "No buffer space available.",
  "No message is available on the STREAM head read queue.",
  "No such device.",
  "No such file or directory.",
  "Executable file format error.",
  "No locks available.",
  "Reserved.",
  "Not enough space.",
  "No message of the desired type.",
  "Protocol not available.",
  "No space left on device.",
  "No STREAM resources.",
  "Not a STREAM.",
  "Function not supported.",
  "The socket is not connected.",
  "Not a directory.",
  "Directory not empty.",
  "Not a socket.",
  "Not supported.",
  "Inappropriate I/O control operation.",
  "No such device or address.",
  "Operation not supported on socket.",
  "Value too large to be stored in data type.",
  "Operation not permitted.",
  "Broken pipe.",
  "Protocol error.",
  "Protocol not supported.",
  "Protocol wrong type for socket.",
  "Result too large.",
  "Read-only file system.",
  "Invalid seek.",
  "No such process.",
  "Reserved.",
  "Stream ioctl() timeout.",
  "Connection timed out.",
  "Text file busy.",
  "Operation would block.",
  "Cross-device link."
};

/**
 * Copies memory
 *  @param dest Destination for memory copy
 *  @param src Source for memory copy
 *  @param c Char which first oncurrence will stop copying
 *  @param n How many bytes to copy
 *  @return dest
 */
void *memccpy(void *dest,const void *src,int c,size_t n) {
  uint8_t *dest8 = (uint8_t*)dest;
  uint8_t *src8 = (uint8_t*)src;
  uint32_t i;

  for (i=0;i<n;i++) {
    dest8[i] = src8[i];
    if (src8[i]==((uint8_t)c)) break;
  }
  return dest;
}

/**
 * Searches for c in src
 *  @param src Pointer where to search
 *  @param c Byte to search
 *  @param n How many bytes to search in
 *  @return Pointer to c in src
 */
void *memchr(const void *src,int c,size_t n) {
  uint8_t *src8 = (uint8_t*)src;
  uint32_t i;

  for (i=0;i<n;i++) {
    if (src8[i]==((uint8_t)c)) return (void*)(src8+i);
  }
  return NULL;
}

/**
 * Compares memory
 *  @param srca First source to compare
 *  @param srcb Second source to compare
 *  @param n How many bytes to compare
 *  @return 0 = equal
 */
int memcmp(const void *srca,const void *srcb,size_t n) {
  if (srca==srcb) return 0;
  size_t fast_count = n/sizeof(int);
  size_t byte_count = n%sizeof(int);
  int *fast_srca = (int*)srca;
  int *fast_srcb = (int*)srcb;
  uint8_t *byte_srca = ((uint8_t*)srca)+fast_count*sizeof(int);
  uint8_t *byte_srcb = ((uint8_t*)srcb)+fast_count*sizeof(int);
  size_t i;

  for (i=0;i<fast_count;i++) {
    if (fast_srca[i]!=fast_srcb[i]) return fast_srca[i]-fast_srcb[i];
  }
  for (i=0;i<byte_count;i++) {
    if (byte_srca[i]!=byte_srcb[i]) return byte_srca[i]-byte_srcb[i];
  }
  return 0;
}

/**
 * Copies memory
 *  @param dest Destination for memory copy
 *  @param src Source for memory copy
 *  @param n How many bytes to copy
 *  @return dest
 */
void *memcpy(void *dest,const void *src,size_t n) {
  if (dest==src) return dest;
  size_t fast_count = n/sizeof(int);
  size_t byte_count = n%sizeof(int);
  int *fast_dest = (int*)dest;
  int *fast_src = (int*)src;
  uint8_t *byte_dest = ((uint8_t*)dest)+fast_count*sizeof(int);
  uint8_t *byte_src = ((uint8_t*)src)+fast_count*sizeof(int);
  size_t i;

  for (i=0;i<fast_count;i++) fast_dest[i] = fast_src[i];
  for (i=0;i<byte_count;i++) byte_dest[i] = byte_src[i];
  return dest;
}

/**
 * Moves memory
 *  @param dest Destination
 *  @param src Source
 *  @param n How many bytes to move
 *  @todo Please do it the right way
 */
void *memmove(void *dest, const void *src, size_t n) {
  if (dest==src) return dest;
  size_t fast_count = n/sizeof(int);
  size_t byte_count = n%sizeof(int);
  int *fast_dest = (int*)dest;
  int *fast_src = (int*)src;
  uint8_t *byte_dest = ((uint8_t*)dest)+fast_count*sizeof(int);
  uint8_t *byte_src = ((uint8_t*)src)+fast_count*sizeof(int);
  size_t i;

  if (src>dest) {
    for (i=0;i<fast_count;i++) fast_dest[i] = fast_src[i];
    for (i=0;i<byte_count;i++) byte_dest[i] = byte_src[i];
  }
  else {
    for (i=byte_count;i>0;i--) byte_dest[i-1] = byte_src[i-1];
    for (i=fast_count;i>0;i--) fast_dest[i-1] = fast_src[i-1];
  }
  return dest;
}

/**
 * Writes specified value in memory
 *  @param dest Destination memory
 *  @param val Value to write in memory
 *  @param n How many bytes to write
 *  @return dest
 */
void *memset(void *dest,int val,size_t n) {
  /*size_t fast_count = n/sizeof(int);
  size_t byte_count = n%sizeof(int);
  int *fast_dest = (int*)dest;
  uint8_t *byte_dest = ((uint8_t*)dest)+fast_count*sizeof(int);
  uint8_t byte_val = (uint8_t)val;
  int fast_val = 0;
  size_t i;

  for (i=0;i<sizeof(int)*8;i+=8) fast_val |= (val&0xFF)<<i;
  for (i=0;i<fast_count;i++) fast_dest[i] = fast_val;
  for (i=0;i<byte_count;i++) byte_dest[i] = byte_val;
  return dest;*/

  size_t i;
  uint8_t *byte_dest = (uint8_t*)dest;
  for (i=0;i<n;i++) byte_dest[i] = val;
  return dest;
}

/**
 * Puts two strings together
 *  @param str1 First string
 *  @param str2 Second string
 *  @return New string
 */
char *strcat(char *str1,const char *str2) {
  return strcpy(str1+strlen(str1),str2);
}

/**
 * Locates the first occurrence of a character in a string
 *  @param str String to search in
 *  @param chr Character to search
 *  @return Pointer (String) to character
 */
char *strchr(const char *str,int chr) {
  size_t i;
  for (i=0;(str[i]!=0 && str[i]!=(char)chr);i++);
  return str[i]==0?NULL:(char*)str+i;
}

/**
 * Compares two strings
 *  @param str1 First string
 *  @param str2 Second string
 *  @return 0 if strings are equal
 */
int strcmp(const char *str1,const char *str2) {
  if (str1==str2) return 0;
  size_t i;

  for (i=0;(str1[i] || str2[i]);i++) {
    if (str1[i]!=str2[i]) return str1[i]-str2[i];
  }
  return 0;
}

/**
 * Copies a string
 *  @param dest Destination where to store the string
 *  @param src Source string
 *  @return returns dest
 */
char *strcpy(char *dest,const char *src) {
  size_t i;
  for (i=0;src[i];i++) dest[i] = src[i];
  dest[i] = 0;
  return dest;
}

/**
 * Duplicates a string
 *  @param src String to duplicate
 *  @return New string
 */
char *strdup(const char *src) {
  size_t len = strlen(src)+1;
  return memcpy(malloc(len),src,len);
}

/**
 * Returns an error string
 *  @param errnum Error number
 *  @return Error string
 */
char *strerror(int errnum) {
  return (char*)errno_strings[errnum];
}

/**
 * Gets string length
 *  @param str String
 *  @return length of string
 */
size_t strlen(const char *str) {
  size_t i;
  for (i=0;str[i];i++);
  return i;
}

/**
 * Appends one string to another with limited size
 *  @param src1 First string
 *  @param src2 Second string, it will be appended to src1
 *  @param n Maximal bytes to append
 */
char *strncat(char *src1,const char *src2,size_t n) {
  size_t src2len = strlen(src2);
  src2len = n<src2len?n:src2len;
  size_t src1len = strlen(src1);
  uint32_t i;

  for (i=0;i<src2len;i++) {
    src1[src1len+i] = src2[i];
  }
  src1[src1len+src2len] = 0;
  return src1;
}

/**
 * Compares to strings until end of string or n bytes
 *  @param str1 String 1
 *  @param str2 String 2
 *  @param n How many bytes to compare maximum
 *  @return 0 if strings are equal
 */
int strncmp(const char *str1,const char *str2,size_t n) {
  if (str1==str2) return 0;
  size_t i;

  for (i=0;(str1[i] || str2[i]) && i<n;i++) {
    if (str1[i]!=str2[i]) return str1[i]-str2[i];
  }
  return 0;
}

/**
 * Copies string, but maximal n bytes
 *  @param dest Destination to copy to
 *  @param src Source to copy from
 *  @param n How many bytes to copy maximal
 *  @return dest
 */
char *strncpy(char *dest,const char *src,size_t n) {
  size_t len = strlen(src)+1;
  if (n<len) len = n;
  memcpy(dest,src,len-1);
  dest[len-1] = 0;
  return dest;
}

static size_t __strxspn(const char *s, const char *map, int parity)
{
	char matchmap[UCHAR_MAX + 1];
	size_t n = 0;

	/* Create bitmap */
	memset(matchmap, 0, sizeof matchmap);
	while (*map)
		matchmap[(unsigned char)*map++] = 1;

	/* Make sure the null character never matches */
	matchmap[0] = parity;

	/* Calculate span length */
	while (matchmap[(unsigned char)*s++] ^ parity)
		n++;

	return n;
}

char *strpbrk(const char *s, const char *accept)
{
	const char *ss = s + __strxspn(s, accept, 1);

	return *ss ? (char *)ss : NULL;
}

char *strsep(char **stringp, const char *delim)
{
	char *s = *stringp;
	char *e;

	if (!s)
		return NULL;

	e = strpbrk(s, delim);
	if (e)
		*e++ = '\0';

	*stringp = e;
	return s;
}

char *strtok(char *s, const char *delim)
{
	static char *holder;

	if (s)
		holder = s;

	do {
		s = strsep(&holder, delim);
	} while (s && !*s);

	return s;
}

// in inttypes.h
static inline int digitval(int ch) {
  if (ch>='0' && ch<='9') return ch-'0';
  else if (ch>='A' && ch<='Z') return ch-'A'+10;
  else if (ch>='a' && ch<='z') return ch-'a'+10;
  else return -1;
}

uintmax_t strntoumax(const char *nptr, char **endptr, int base, size_t n) {
  int minus = 0;
  uintmax_t v = 0;
  int d;

  while (n && isspace((unsigned char)*nptr)) {
    nptr++;
    n--;
  }

  /* Single optional + or - */
  if (n) {
    char c = *nptr;
    if (c=='-' || c=='+') {
      minus = (c=='-');
      nptr++;
      n--;
    }
  }

  if (base==0) {
    if (n>=2 && nptr[0]=='0' && (nptr[1]=='x' || nptr[1]=='X')) {
      n -= 2;
      nptr += 2;
      base = 16;
    }
    else if (n >= 1 && nptr[0] == '0') {
      n--;
      nptr++;
      base = 8;
    }
    else base = 10;
  }
  else if (base==16) {
    if (n>=2 && nptr[0]=='0' && (nptr[1]=='x' || nptr[1]=='X')) {
      n -= 2;
      nptr += 2;
    }
  }

  while (n && (d = digitval(*nptr))>=0 && d<base) {
    v = v*base+d;
    n--;
    nptr++;
  }

  if (endptr)
  *endptr = (char*)nptr;

  return minus?-v:v;
}

char *strrchr(const char *s, int c) {
  char *l = NULL;
  size_t i;
  for (i=0;s[i];i++) {
    if (s[i]==c) l = (char*)s+i;
  }
  return l;
}

static size_t strxspn(const char *s,const char *map,int parity) {
  char matchmap[UCHAR_MAX+1];
  size_t n = 0;

  /* Create bitmap */
  memset(matchmap,0,sizeof matchmap);
  while (*map) matchmap[(unsigned char)*map++] = 1;

  /* Make sure the null character never matches */
  matchmap[0] = parity;

  /* Calculate span length */
  while (matchmap[(unsigned char)*s++]^parity) n++;

  return n;
}

size_t strspn(const char *s,const char *accept) {
  return strxspn(s,accept,0);
}

size_t strcspn(const char *s,const char *reject) {
  return strxspn(s,reject,1);
}
