#include <gdt.h>
#include <malloc.h>
#include <string.h>
#include <debug.h>

void gdt_reloadsregs() {
  /*const selector_t csel = {
    .index = 1,
    .ti = 0,
    .priv = PRIV_KERNEL
  };*/
  const selector_t dsel = {
    .index = 2,
    .ti = 0,
    .priv = PRIV_KERNEL
  };
  /*asm("mov %0,%%ds;"
      "ljmp %1,$gdt_reloadregs_fi;"
      "gdt_reloadregs_fi:"
      ::"r"((uint16_t)16),"i"((uint16_t)8));*/

  asm("ljmpl $0x08, $1f;"
      "1:"
      "mov %0, %%ds;"
      "mov %0, %%es;"
      "mov %0, %%fs;"
      "mov %0, %%gs;"
      "mov %0, %%ss;"
      ::"r"(dsel):"eax");
  return;
}

/**
 * Initializes GDT
 *  @return 0=Success; -1=Failure
 */
int gdt_init() {
  //gdt = calloc(GDT_MAXDESC,sizeof(gdtdesc_t));
  memset(gdt,0,GDT_MAXDESC*sizeof(gdtdesc_t));

  // Ring 0
  gdt_set_descriptor(1,0x000FFFFF,0x00000000,GDT_SEGMENT|GDT_PRESENT|GDT_CODESEG,PRIV_KERNEL);
  gdt_set_descriptor(2,0x000FFFFF,0x00000000,GDT_SEGMENT|GDT_PRESENT|GDT_DATASEG,PRIV_KERNEL);

  // Ring 3
  gdt_set_descriptor(3,0x000FFFFF,0x00000000,GDT_SEGMENT|GDT_PRESENT|GDT_CODESEG,PRIV_USER);
  gdt_set_descriptor(4,0x000FFFFF,0x00000000,GDT_SEGMENT|GDT_PRESENT|GDT_DATASEG,PRIV_USER);

  gdtsel_t selector = {
    .size = GDT_MAXDESC*sizeof(gdtdesc_t)-1,
    .offset = (uint32_t)&gdt
  };
  asm("lgdt (%0)"::"r"(&selector));
  gdt_reloadsregs();

  return 0;
}

void gdt_set_descriptor(int segment,size_t size,void *vdbase,int access,priv_t priv) {
  uint32_t base = (uint32_t)vdbase;
  gdt[segment].size0_15 = size&0xFFFF;
  gdt[segment].flags = ((size>>16)&0x0F)|0xC0;
  gdt[segment].base0_15 = base&0xFFFF;
  gdt[segment].base16_23 = (base>>16)&0xFF;
  gdt[segment].base24_31 = ((base>>24)&0xFF);
  gdt[segment].access = access|((priv&3)<<5);
}
