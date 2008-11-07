#include <syscall.h>
#include <biosint.h>
#include <stdint.h>
#include <debug.h>
#include <memmap.h>
#include <procm.h>

#define IVT_GET_IP(i) (((uint16_t*)IVT_ADDRESS)[i*2])
#define IVT_GET_CS(i) (((uint16_t*)IVT_ADDRESS)[i*2+1])

int biosint_init() {
  if (syscall_create(SYSCALL_MISC_BIOSINT,biosint_call,10)==-1) return -1;
  return 0;
}

uint16_t biosint_call(unsigned int *meminfo,size_t meminfo_count,uint16_t ax,uint16_t bx,uint16_t cx,uint16_t dx,uint16_t si,uint16_t di,uint16_t ds,uint16_t es) {
  size_t i;
  proc_t *proc = vm86_proc_create("bios_int",proc_current->uid,proc_current->gid,proc_current);

  proc->registers.eax = ax;
  proc->registers.ebx = bx;
  proc->registers.ecx = cx;
  proc->registers.edx = dx;
  proc->registers.esi = si;
  proc->registers.edi = di;
  proc->registers.ds = ds;
  proc->registers.es = es;

  proc->registers.eip = IVT_GET_IP(0x10);
  proc->registers.cs = IVT_GET_CS(0x10);

  kprintf("AX: 0x%x\n",proc->registers.eax);
  kprintf("BX: 0x%x\n",proc->registers.ebx);
  kprintf("CX: 0x%x\n",proc->registers.ecx);
  kprintf("DX: 0x%x\n",proc->registers.edx);
  kprintf("SI: 0x%x\n",proc->registers.esi);
  kprintf("DI: 0x%x\n",proc->registers.edi);
  kprintf("DS: 0x%x\n",proc->registers.ds);
  kprintf("ES: 0x%x\n",proc->registers.es);
  kprintf("IP: 0x%x\n",proc->registers.eip);
  kprintf("CS: 0x%x\n",proc->registers.cs);

  for (i=0;i<meminfo_count*3;i+=3) {
    kprintf("Dest: 0x%x\n",meminfo[i]);
    kprintf("Src:  0x%x\n",meminfo[i+1]);
    kprintf("Size: 0x%x\n",meminfo[i+2]);
  }

  proc_sleep(proc_current);

  return -1;
}