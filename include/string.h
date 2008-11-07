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

#endif
