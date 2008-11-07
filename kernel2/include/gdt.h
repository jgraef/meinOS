#ifndef _GDT_H_
#define _GDT_H_

#include <sys/types.h>
#include <stdint.h>

#define GDT_CODESEG 0x0A
#define GDT_DATASEG 0x02
#define GDT_TSS 0x09
#define GDT_PRESENT 0x80
#define GDT_SEGMENT 0x10

#define GDT_TSSDESC 8

#define GDT_MAXDESC 64

#define IDX2SEL(idx,priv) (((idx)<<3)|(priv))

typedef enum {
  PRIV_KERNEL = 0,
  PRIV_USER = 3
} priv_t;

/*typedef struct {
  unsigned limit0_15:16;
  unsigned base0_15:16;
  unsigned base16_23:8;
  unsigned accessed:1;
  unsigned int rw:1;
  enum {
    GDT_UP = 0,
    GDT_DOWN = 1
  } dc:1;
  enum {
    GDT_DATA = 0,
    GDT_CODE = 1
  } type:1;
  unsigned notsystem:1;
  priv_t priv:2;
  unsigned present:1;
  unsigned zero:2;
  enum {
    GDT_16BIT = 0,
    GDT_32BIT = 1,
  } size:1;
  gran_t granular:1;
  unsigned limit16_19:4;
  unsigned base24_31:8;
} __attribute__ ((packed)) gdtdesc_t;*/
typedef struct {
  uint16_t size0_15;
  uint16_t base0_15;
  uint8_t base16_23;
  uint8_t access;
  uint8_t flags;
  uint8_t base24_31;
} __attribute__ ((packed)) gdtdesc_t;

typedef struct {
  uint16_t size;
  uint32_t offset;
} __attribute__ ((packed)) gdtsel_t;

typedef struct {
  priv_t priv:2;
  unsigned ti:1;
  unsigned index:13;
} __attribute__ ((packed)) selector_t;

typedef gdtdesc_t* gdt_t;

//gdt_t gdt;
gdtdesc_t gdt[GDT_MAXDESC];

int gdt_init();
void gdt_set_descriptor(int segment,size_t size,void *base,int access,priv_t priv);

#endif
