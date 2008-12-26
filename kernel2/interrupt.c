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

#include <sys/types.h>
#include <interrupt.h>
#include <stdint.h>
#include <kprint.h>
#include <vga.h>
#include <cpu.h>
#include <pic.h>
#include <lapic.h>
#include <syscall.h>
#include <memuser.h>
#include <llist.h>
#include <signal.h>
#include <memmap.h>
#include <procm.h>
#include <elf.h>

//#define STACKTRACE

/**
 * Initializes Interrupts
 *  @return 0=Success; -1=Failure
 */
int interrupt_init() {
  size_t i;
  interrupt_enabled = 0;
  interrupt_sleep = llist_create();
  for (i=0;i<16;i++) interrupt_irq[i] = llist_create();
  if (syscall_create(SYSCALL_IRQ_HANDLER,interrupt_irq_reghandler,2)==-1) return -1;
  if (syscall_create(SYSCALL_IRQ_SLEEP,interrupt_irq_sleep,1)==-1) return -1;
  if (syscall_create(SYSCALL_TIME_SLEEP,interrupt_time_sleep,1)==-1) return -1;
  if (syscall_create(SYSCALL_TIME_USLEEP,interrupt_time_usleep,1)==-1) return -1;
  if (syscall_create(SYSCALL_TIME_GETTICKS,interrupt_time_getticks,0)==-1) return -1;
  return 0;
}

/**
 * Interrupt handler
 *  @param interrupt Interrupt number
 */
void interrupt_handler(unsigned int interrupt,uint32_t *stack) {
  asm("mov %%ss,%0; mov %0,%%ds"::"r"(0));
  cpu_t *cpu = cpu_this;

  uint32_t errorcode = 0;
  interrupt_save_stack(stack,&errorcode);

  // Exceptions
  if (interrupt<0x20 && interrupt!=0x0F) interrupt_exception_handler(interrupt,errorcode);

  // IRQs
  if (interrupt>0x1F && interrupt<0x30) interrupt_irq_handler(interrupt-0x20);
  if (!cpu->uselapic && interrupt==0x30) interrupt_irq_handler(0);

  // LAPIC
  if (cpu->uselapic && interrupt>0x2F && interrupt<0x37 && interrupt==0x0F) interrupt_lapic_handler(interrupt);

  asm("mov %%es,%0; mov %0,%%ds"::"r"(0));

  if (proc_current==NULL) proc_shedule();
}

/**
 * Timer (PIC or LAPIC)
 */
void interrupt_timer() {
  cpu_t *cpu = cpu_this;
  cpu->ticks++;
  interrupt_time_check(cpu->interval*1000);
  proc_shedule();
}

/**
 * LAPIC interrupt handler
 *  @param interrupt Interrupt
 */
void interrupt_lapic_handler(unsigned int interrupt) {
  lapic_eoi();
  if (interrupt==0x30) interrupt_timer();
}

/**
 * IRQ handler
 *  @param irq IRQ
 */
void interrupt_irq_handler(unsigned int irq) {
  pic_eoi(irq);
  if (irq==0) interrupt_timer();
  else interrupt_irq_check(irq);
}

/**
 * Exception handler
 *  @param exception Exception number
 */
void interrupt_exception_handler(unsigned int exception,uint32_t errcode) {
  char *exceptions[] = {
    "Divide Error",
    "Debug Exception",
    "Intel reserved",
    "Breakpoint",
    "Overflow",
    "Bounds Check",
    "Invalid Opcode",
    "Coprocessor Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Exception",
    "General Protection Fault",
    "Page Fault",
    "Intel reserved",
    "Coprocessor Error"
  };

  uint32_t cr2;
  asm("mov %%cr2,%0":"=a"(cr2));
  errcode &= 0xFFFF;

  if (exception==INTERRUPT_EXCEPTION_PAGE_FAULT) {
    if (memuser_pagefault((void*)cr2)==0) return;
  }
  vga_text_cursor.color = VGA_TEXT_COLOR_RED_BLACK;
  kprintf(" **EXCEPTION[0x%x]: %s from %s (by EIP)\n",exception,exception<0x10?exceptions[exception]:"Unknown",*interrupt_curregs.eip<USERDATA_ADDRESS?"Kernel":"Userland");
  if (proc_current!=NULL) kprintf("Process: #%d %s\n",proc_current->pid,proc_current->name);
  else kprintf("Process: none\n");
  vga_text_cursor.color = VGA_TEXT_COLOR_DEFAULT;
  kprintf("EAX: 0x%x\tEBX: 0x%x\tECX: 0x%x\tEDX: 0x%x\n",*interrupt_curregs.eax,*interrupt_curregs.ebx,*interrupt_curregs.ecx,*interrupt_curregs.edx);
  kprintf("EIP: 0x%x\tEFL: 0x%x\tEBP: 0x%x\tESP: 0x%x\n",*interrupt_curregs.eip,*interrupt_curregs.efl,*interrupt_curregs.ebp,*interrupt_curregs.esp);
  kprintf("EDI: 0x%x\tESI: 0x%x\n",*interrupt_curregs.edi,*interrupt_curregs.esi);
  kprintf("CS:  0x%x\tDS:  0x%x\tES:  0x%x\tFS:  0x%x\n",*interrupt_curregs.cs,*interrupt_curregs.ds,*interrupt_curregs.es,*interrupt_curregs.fs);
  kprintf("GS:  0x%x\tSS:  0x%x\n",interrupt_curregs.gs,interrupt_curregs.ss);
  if (exception==INTERRUPT_EXCEPTION_PAGE_FAULT) {
    kprintf("Pagefault at: 0x%x\n",cr2);
    kprintf("%s by %s from %s\n",(errcode&1)?"Rights violation":"Accessing a not present page",(errcode&8)?"instruction fetch":((errcode&2)?"writing":"reading"),(errcode&4)?proc_current->name:"Kernel");
  }
  else if (errcode!=0) {
    kprintf("%s error in %s at index 0x%x\n",(errcode&1)?"External":"Internal",(errcode&2)?"IDT":((errcode&4)?"LDT":"GDT"),(errcode&0xFFF8)>>3);
  }

#ifdef STACKTRACE
  if (*interrupt_curregs.esp>=0x4000000 && *interrupt_curregs.esp<0x40001000) {
    int *i;
    kprintf("Stack:\n");
    for (i=(int*)*interrupt_curregs.esp;i<(int*)0x40001000;i++) kprintf("0x%x:\t0x%x\n",i,*i);
  }
#endif

  cpu_halt();

  if (exception==INTERRUPT_EXCEPTION_PAGE_FAULT) kill(proc_current,SIGSEGV);
  else if (exception==INTERRUPT_EXCEPTION_INVALID_OPCODE) kill(proc_current,SIGILL);
  else kill(proc_current,SIGKILL);

  /// @todo just change process
  proc_idle();
}

/**
 * Enables/Disables interrupts
 *  @param enable Whether to enable or disable interrupts
 */
void interrupt_enable(int enable) {
  interrupt_enabled = enable;
  if (enable) asm("sti");
  else asm("cli");
}

/**
 * Registers an IRQ handler (Syscall)
 *  @param irq IRQ
 *  @param func Function
 */
int interrupt_irq_reghandler(unsigned int irq,void *func) {
  if (irq<16) {
    interrupt_irq_t *new = malloc(sizeof(interrupt_irq_t));
    new->func = func;
    new->proc = proc_current;
    new->type = IRQ_HANDLER;
    llist_push(interrupt_irq[irq],new);
    return 0;
  }
  else return -1;
}

/**
 * Sleeps until an IRQ is fired (Syscall)
 *  @param irq IRQ
 */
int interrupt_irq_sleep(unsigned int irq) {
  if (irq<16) {
    interrupt_irq_t *new = malloc(sizeof(interrupt_irq_t));
    new->proc = proc_current;
    new->type = IRQ_SLEEP;
    llist_push(interrupt_irq[irq],new);
    return 0;
  }
  else return -1;
}

/**
 * Checks for IRQ events
 *  @param irq IRQ
 */
void interrupt_irq_check(unsigned int irq) {
  interrupt_irq_t *irqo;
  size_t i;
  for (i=0;(irqo = llist_get(interrupt_irq[irq],i));i++) {
    if (irqo->type==IRQ_SLEEP) {
      proc_wake(irqo->proc);
      free(irqo);
      llist_remove(interrupt_irq[irq],i);
      i--;
    }
    else proc_call(irqo->proc,irqo->func,1,irq);
  }
}

/**
 * Sleeps until an time elapsed (Syscall)
 *  @param sec Seconds
 */
int interrupt_time_sleep(unsigned int sec) {
  interrupt_sleep_t *new = malloc(sizeof(interrupt_sleep_t));
  new->type = TIME_SLEEP;
  new->usec = sec*1000000;
  new->proc = proc_current;
  llist_push(interrupt_sleep,new);
  proc_sleep(proc_current);
  return 0;
}

/**
 * Sleeps until an time elapsed (Syscall)
 *  @param sec Microseconds
 */
int interrupt_time_usleep(unsigned int usec) {
  interrupt_sleep_t *new = malloc(sizeof(interrupt_sleep_t));
  new->type = TIME_SLEEP;
  new->usec = usec;
  new->proc = proc_current;
  llist_push(interrupt_sleep,new);
  proc_sleep(proc_current);
  return 0;
}

/**
 * Checks for ending sleeps
 *  @param usec Time since last check
 */
void interrupt_time_check(unsigned int usec) {
  interrupt_sleep_t *sleep;
  size_t i;
  for (i=0;(sleep = llist_get(interrupt_sleep,i));i++) {
    if (sleep->usec<=usec) {
      if (sleep->type==TIME_SLEEP) proc_wake(sleep->proc);
      llist_remove(interrupt_sleep,i);
      i--;
    }
    else sleep->usec -= usec;
  }
}

/**
 * Gets number of ticks since boot
 *  @return Number of ticks
 */
clock_t interrupt_time_getticks() {
  return cpu_this->ticks;
}

uint32_t *interrupt_save_stack(uint32_t *stack,uint32_t *errorcode) {
  interrupt_curregs.edi = stack++;
  interrupt_curregs.esi = stack++;
  interrupt_curregs.ebp = stack++;
  stack++; // esp
  interrupt_curregs.ebx = stack++;
  interrupt_curregs.edx = stack++;
  interrupt_curregs.ecx = stack++;
  interrupt_curregs.eax = stack++;
  interrupt_curregs.ds = stack++;
  interrupt_curregs.es = stack++;
  interrupt_curregs.fs = stack++;
  interrupt_curregs.gs = stack++;
  if (errorcode!=NULL) *errorcode = *stack++; // error code
  else stack++;
  interrupt_curregs.eip = stack++;
  interrupt_curregs.cs = stack++;
  interrupt_curregs.efl = stack++;
  interrupt_curregs.esp = stack++;
  interrupt_curregs.ss = stack++;
  vm86_curregs.es = stack++;
  vm86_curregs.ds = stack++;
  vm86_curregs.fs = stack++;
  vm86_curregs.gs = stack++;

/*kprintf("Stack: 0x%x\n",stack);
kprintf("EDI: 0x%x\n",*interrupt_curregs.edi);
kprintf("ESI: 0x%x\n",*interrupt_curregs.esi);
kprintf("EBP: 0x%x\n",*interrupt_curregs.ebp);
kprintf("EBX: 0x%x\n",*interrupt_curregs.ebx);
kprintf("EDX: 0x%x\n",*interrupt_curregs.edx);
kprintf("ECX: 0x%x\n",*interrupt_curregs.ecx);
kprintf("EAX: 0x%x\n",*interrupt_curregs.eax);
kprintf("ERR: 0x%x\n",*errorcode);
kprintf("EIP: 0x%x\n",*interrupt_curregs.eip);
kprintf("CS:  0x%x\n",*interrupt_curregs.cs);
kprintf("EFL: 0x%x\n",*interrupt_curregs.efl);
kprintf("ESP: 0x%x\n",*interrupt_curregs.esp);
kprintf("DS:  0x%x\n",*interrupt_curregs.ds);
kprintf("ES:  0x%x\n",*interrupt_curregs.es);
kprintf("FS:  0x%x\n",*interrupt_curregs.fs);
kprintf("GS:  0x%x\n",*interrupt_curregs.gs);
kprintf("SS:  0x%x\n",*interrupt_curregs.ss);*/

  return stack;
}
