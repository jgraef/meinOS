#ifndef _KPRINT_H_
#define _KPRINT_H_

#include <stdarg.h>

void kprintchar(char chr);
void kprintstr(char *str);
void kprintnum(unsigned long long value,unsigned int radix,unsigned int uppercase);
void vakprintf(char *format,va_list args);
void kprintf(char *format,...);
void panic(char *fmt,...);

#endif
