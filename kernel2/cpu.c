#include <sys/types.h>
#include <stdint.h>
#include <cpu.h>
#include <memkernel.h>
#include <paging.h>
#include <llist.h>
#include <sizes.h>
#include <idt.h>
#include <debug.h>
#include <interrupt.h>
#include <vga.h>

/**
 * Initializes CPUs
 *  @return 0=Success; -1=Failure
 */
int cpu_init() {
  if ((cpus = llist_create())==NULL) return -1;
  cpu_t *cpu = malloc(sizeof(cpu_t));
  if (cpu==NULL) return -1;
  cpu->enabled = 1;
  cpu_stack_create(cpu,PAGE_SIZE);
  llist_push(cpus,cpu);
  return 0;
}
/**
 * Creates stack
 *  @param cpu CPU to create stack for
 *  @param size Stack size
 *  @return 0=Success; -1=Failure
 */
int cpu_stack_create(cpu_t *cpu,size_t size) {
  if ((cpu->stack = memkernel_alloc((size_t)PAGEUP(size)))==NULL) return -1;
  cpu->stacksize = size;
  return 0;
}

void cpu_id(uint32_t seax,uint32_t *deax,uint32_t *debx,uint32_t *decx,uint32_t *dedx) {
  uint32_t eax,ebx,ecx,edx;
  asm("cpuid":"=a"(eax),"=b"(ebx),"=c"(ecx),"=d"(edx):"a"(seax));
  if (deax!=NULL) *deax = eax;
  if (debx!=NULL) *debx = ebx;
  if (decx!=NULL) *decx = ecx;
  if (dedx!=NULL) *dedx = edx;
}

/**
 * Shuts down computer
 */
void cpu_shutdown() {
  //vga_text_clear();
  kprintf("Please turn off the computer\n");
  interrupt_enable(0);
  cpu_halt();
}

/**
 * Resets computer
 */
void cpu_restart() {
  idtsel_t idtsel = {
    .size = 0,
    .offset = 0
  };
  asm("lidt (%0)"::"r"(&idtsel));
}
