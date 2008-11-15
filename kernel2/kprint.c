/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdarg.h>
#include <vga.h>
#include <cpu.h>
#include <interrupt.h>

unsigned long long divmod(unsigned long long dividend,unsigned int divisor,unsigned int *remainder) {
  unsigned long long quotient;
  unsigned int rem;

  quotient = dividend / divisor;
  rem = dividend % divisor;

  if (remainder) *remainder = rem;
  return quotient;
}

void kprintchar(char chr) {
  vga_text_printchar(chr);
}

void kprintstr(char *str) {
  if (str==NULL) str = "(null)";
  while (*str) kprintchar(*str++);
}

void kprintnum(long long value,unsigned int radix,unsigned int uppercase,int sign) {
  char buf[65];
  char *p = buf;
  const char * const chars = uppercase ? "0123456789ABCDEFGHIJKLMOPQRSTUVWXYZ" : "0123456789abcdefghijklmopqrstuvwxyz";
  unsigned long long temp;
  unsigned int digits;
  unsigned int remainder;

  if (value<0 && sign) {
    kprintchar('-');
    value = -value;
  }

  // Es werden nur Basen zwischen 2 und 36 unterstuezt
  if(radix < 2 || radix > 36) return;

  // Anzahl der Ziffern zaehlen
  temp = value;
  digits = 0;
  do {
    digits++;
    temp = divmod(temp, radix, 0);
  }
  while (temp>0);

  // Zeiger auf das Ende der Zahl setzen und Nullterminierung einfuegen
  p += digits;
  *p = 0;

  // Ziffern rckw�ts in den Puffer schreiben
  temp = value;
  do {
    temp = divmod(temp, radix, &remainder);
    *--p = chars[remainder];
  }
  while (--digits);

  kprintstr(buf);
}

void vakprintf(char *format,va_list args) {
  while (*format) {
    if (*format=='%') {
      format++;
      if (!*format) return;
      else if (*format=='%') kprintchar('%');
      else if (*format=='b') kprintnum(va_arg(args,unsigned int),2,1,0);
      else if (*format=='c') kprintchar(va_arg(args,unsigned int));
      else if (*format=='o') kprintnum(va_arg(args,unsigned int),8,1,0);
      else if (*format=='d') kprintnum(va_arg(args,int),10,1,1);
      else if (*format=='x') kprintnum(va_arg(args,unsigned int),16,1,0);
      else if (*format=='s') kprintstr(va_arg(args,char*));
      else {
        kprintchar('%');
        kprintchar(*format);
      }
    }
    else kprintchar(*format);
    format++;
  }
}

void kprintf(char *format,...) {
  va_list args;

  va_start(args,format);
  vakprintf(format,args);
  va_end(args);
}

void panic(char *fmt,...) {
  va_list args;

  va_start(args,fmt);
  vga_text_setcolor(VGA_TEXT_COLOR_RED_BLACK);
  kprintstr(" *PANIC* ");
  vakprintf(fmt,args);
  interrupt_enable(0);
  cpu_halt();
}
