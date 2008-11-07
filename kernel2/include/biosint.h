#ifndef _BIOSINT_H_
#define _BIOSINT_H_

#include <stdint.h>
#include <procm.h>

int biosint_init();
uint16_t biosint_call(unsigned int *meminfo,size_t meminfo_count,uint16_t ax,uint16_t bx,uint16_t cx,uint16_t dx,uint16_t si,uint16_t di,uint16_t ds,uint16_t es);

#endif
