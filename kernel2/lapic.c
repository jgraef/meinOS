#include <lapic.h>
#include <stdint.h>
#include <pic.h>
#include <interrupt.h>
#include <paging.h>
#include <cpu.h>
#include <isr.h>
#include <debug.h>
#include <memkernel.h>

/**
 * Initializes LAPIC
 *  @return 0=Success; -1=Failure
 */
int lapic_init() {
  uint32_t edx = 0;
  cpu_t *cpu = cpu_this;

  pic_init();

  cpu_id(1,NULL,NULL,NULL,&edx);
  if (edx&(1<<9) && 0) {
    isr_uselapic = 1;
    cpu->uselapic = 1;

    lapic = memkernel_findvirt(1)+PAGEOFF(LAPIC_PHYS_ADDRESS);
    if (paging_map(PAGEDOWN(lapic),PAGEDOWN(LAPIC_PHYS_ADDRESS),0,1)<0) panic("Cannot map LAPIC\n");

    lapic_reg(LAPIC_REG_TPR) = 0x20;
    //lapic_timer_init();
    //lapic_reg(LAPIC_REG_LVT_TIMER) = 0x20030;
    lapic_reg(LAPIC_REG_LVT_THERMAL) = 0x20031;
    lapic_reg(LAPIC_REG_LVT_PERFOR) = 0x20032;
    lapic_reg(LAPIC_REG_LVT_LINT0) = 0x08700;
    lapic_reg(LAPIC_REG_LVT_LINT1) = 0x00400;
    lapic_reg(LAPIC_REG_LVT_ERROR) = 0x20035;
    lapic_reg(LAPIC_REG_SPURIOUS) = 0x0010F;

    return 0;
  }
  else {
    isr_uselapic = 0;
    cpu->uselapic = 0;
    cpu->interval = 10; // IRQ0 all 10 ms
    pic_pit_setinterval(0,cpu->interval);
    return -1;
  }
}

/**
 * Sends EOI
 */
void lapic_eoi() {
  lapic_reg(LAPIC_REG_EOI) = 0;
}
