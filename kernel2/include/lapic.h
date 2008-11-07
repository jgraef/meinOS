#ifndef _APIC_H_
#define _APIC_H_

#include <stdint.h>

#define LAPIC_PHYS_ADDRESS 0xFEE00000

#define LAPIC_REG_ID            0x020
#define LAPIC_REG_VERSION       0x030
#define LAPIC_REG_TPR           0x080
#define LAPIC_REG_APR           0x090
#define LAPIC_REG_PPR           0x0A0
#define LAPIC_REG_EOI           0x0B0
#define LAPIC_REG_LOGDEST       0x0D0
#define LAPIC_REG_DESTFMT       0x0E0
#define LAPIC_REG_SPURIOUS      0x0F0
#define LAPIC_REG_ERROR         0x280
#define LAPIC_REG_LVT_TIMER     0x320
#define LAPIC_REG_LVT_THERMAL   0x330
#define LAPIC_REG_LVT_PERFOR    0x340
#define LAPIC_REG_LVT_LINT0     0x350
#define LAPIC_REG_LVT_LINT1     0x360
#define LAPIC_REG_LVT_ERROR     0x370
#define LAPIC_REG_TIMER_INITIAL 0x380
#define LAPIC_REG_TIMER_CURRENT 0x390
#define LAPIC_REG_TIMER_DEVIDE  0x3E0

#define lapic_reg(off) (*((uint32_t*)(lapic+(off))))

void *lapic;

int lapic_init();
void lapic_eoi();
void lapic_timer_init();

#endif
