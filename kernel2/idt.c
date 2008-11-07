#include <idt.h>
#include <isr.h>
#include <malloc.h>
#include <gdt.h>
#include <string.h>
#include <debug.h>
#include <syscall.h>

/**
 * Initializes IDT
 *  @return 0=Success; -1=Failure
 */
int idt_init() {
  int i;
  //idt = calloc(ISR_NUM,sizeof(idtdesc_t));
  memset(idt,0,ISR_NUM*sizeof(idtdesc_t));

  selector_t selector = {
    .index = 1,
    .ti = 0,
    .priv = PRIV_KERNEL
  };
  for (i=0;i<ISR_NUM;i++) {
    idt_set_descriptor(i,isr[i],selector,i==SYSCALL_INT?PRIV_USER:PRIV_KERNEL,IDT_INTGATE32|IDT_PRESENT);
  }

  idtsel_t idtsel = {
    .size = ISR_NUM*sizeof(idtdesc_t)-1,
    .offset = (uint32_t)&idt
  };
  asm("lidt (%0)"::"r"(&idtsel));

  return 0;
}

void idt_set_descriptor(int i,void *addr,selector_t selector,priv_t priv,int type) {
  idt[i].zero = 0;
  idt[i].address0_15 = ((unsigned int)addr)&0xFFFF;
  idt[i].address16_31 = (((unsigned int)addr)>>16)&0xFFFF;
  idt[i].selector = selector;
  idt[i].type = type|((priv&3)<<5);
}
