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

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include <stdint.h>
#include <llist.h>
#include <procm.h>

#define INTERRUPT_EXCEPTION_DIVIDE_ERROR      0x00
#define INTERRUPT_EXCEPTION_DEBUG_EXCEPTION   0x01
#define INTERRUPT_EXCEPTION_BREAKPOINT        0x03
#define INTERRUPT_EXCEPTION_OVERFLOW          0x04
#define INTERRUPT_EXCEPTION_BOUNDS_CHECK      0x05
#define INTERRUPT_EXCEPTION_INVALID_OPCODE    0x06
#define INTERRUPT_EXCEPTION_COPROCESSOR_NA    0x07
#define INTERRUPT_EXCEPTION_DOUBLE_FAULT      0x08
#define INTERRUPT_EXCEPTION_COPROCESSOR_SO    0x09
#define INTERRUPT_EXCEPTION_INVALID_TSS       0x0A
#define INTERRUPT_EXCEPTION_SEGMENT_NP        0x0B
#define INTERRUPT_EXCEPTION_STACK_EXCEPTION   0x0C
#define INTERRUPT_EXCEPTION_GPF               0x0D
#define INTERRUPT_EXCEPTION_PAGE_FAULT        0x0E
#define INTERRUPT_EXCEPTION_COPROCESSOR_ERROR 0x10

typedef struct interrupt_sleep_S interrupt_sleep_t;

struct interrupt_sleep_S {
  enum { TIME_SLEEP, TIME_HANDLER } type;
  unsigned long long usec;
  proc_t *proc;
};

typedef struct {
  enum { IRQ_SLEEP, IRQ_HANDLER } type;
  proc_t *proc;
  void *func;
} interrupt_irq_t;

struct cpu_registers interrupt_curregs;

llist_t interrupt_sleep;
int interrupt_enabled;
llist_t interrupt_irq[16];

int interrupt_init();
void interrupt_handler(unsigned int interrupt,uint32_t *stack);
void interrupt_lapic_handler(unsigned int interrupt);
void interrupt_irq_handler(unsigned int irq);
void interrupt_exception_handler(unsigned int exception,uint32_t errcode);
void interrupt_enable(int enable);
int interrupt_irq_reghandler(unsigned int irq,void *func);
int interrupt_irq_sleep(unsigned int irq);
void interrupt_irq_check(unsigned int irq);
int interrupt_time_sleep(unsigned int sec);
int interrupt_time_usleep(unsigned int usec);
void interrupt_time_check(unsigned int usec);
clock_t interrupt_time_getticks();
uint32_t *interrupt_save_stack(uint32_t *stack,uint32_t *errorcode);

#endif
