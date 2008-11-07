#include <tss.h>
#include <cpu.h>
#include <gdt.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <debug.h>
#include <procm.h>

/**
 * Initializes TSS
 *  @return 0=Success; -1=Failure
 */
int tss_init() {
  cpu_t *cpu = cpu_this;

  // create TSS
  tss_t *tss = malloc(sizeof(tss_t));
  if (tss==NULL) return -1;
  memset(tss,0,sizeof(tss_t));
  tss->ss0 = IDX2SEL(2,PRIV_KERNEL);
  tss->esp0 = (uint32_t)(cpu->stack+cpu->stacksize-4)+sizeof(struct vm86_segmentregs);
  tss->iopb_offset = offsetof(tss_t,iopb);
  cpu->tss = tss;

  // create TSS descriptor
  int index = GDT_TSSDESC+cpu_getid();
  gdt_set_descriptor(index,sizeof(tss_t)-1,tss,GDT_PRESENT|GDT_TSS,PRIV_USER);
  gdt[index].flags &= 0x0F;

  // load TSS descriptor
  selector_t selector = {
    .index = index,
    .ti = 0,
    .priv = PRIV_KERNEL
  };
  asm("ltr %0"::"a"(selector));

  return 0;
}
