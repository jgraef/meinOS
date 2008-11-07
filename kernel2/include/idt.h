#ifndef _IDT_H_
#define _IDT_H_

#include <stdint.h>
#include <gdt.h>
#include <isr.h>

#define IDT_PRESENT   0x80
#define IDT_INTGATE32 0x0E

typedef struct {
  uint16_t address0_15;
  selector_t selector;
  uint8_t zero;
  uint8_t type;
  uint16_t address16_31;
} __attribute__ ((packed)) idtdesc_t;

/*typedef struct {
  uint16_t address0_15;
  selector_t selector;
  uint8_t zero1;
  unsigned present:1;
  priv_t priv:2;
  unsigned zero2:1;
  enum {
    IDT_TASK = 0x05,
    IDT_INT_16 = 0x06,
    IDT_TRAP_16 = 0x07,
    IDT_INT_32 = 0x0E,
    IDT_TRAP_32 = 0x0F
  } gatetype:4;
  uint16_t address16_31;
} __attribute__ ((packed)) idtdesc_t;*/

typedef struct {
  uint16_t size;
  uint32_t offset;
} __attribute__ ((packed)) idtsel_t;

typedef idtdesc_t* idt_t;

//idt_t idt;
idtdesc_t idt[ISR_NUM];

int idt_init();
void idt_set_descriptor(int i,void *addr,selector_t selector,priv_t priv,int type);

#endif
