/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/types.h>
#include <stdint.h>
#include <ioport.h>
#include <devfs.h>
#include <stdio.h>

#define PCI_CONFIG_DATA    0xCFC
#define PCI_CONFIG_ADDRESS 0xCF8

#define PCI_MAXBUSSES 256
#define PCI_MAXSLOTS  32
#define PCI_MAXFUNCS  8

typedef struct {
  unsigned int bus,slot,func;
} pci_dev_t;

/**
 * Reads dword from config register of PCI device
 *  @param dev PCI device
 *  @param offset Register offset
 *  @return Register content
 */
uint32_t pci_config_readd(unsigned int bus,unsigned int slot,unsigned int func,size_t offset) {
  uint32_t val;
  uint32_t address = 0x80000000|(((uint32_t)bus)<<16)|(((uint32_t)slot)<<11)|(((uint32_t)func)<<8)|(offset&0xFC);
  outl(PCI_CONFIG_ADDRESS,address);
  val = inl(PCI_CONFIG_DATA);
  return val;
}

/**
 * Writes dword in config register of PCI device
 *  @param dev PCI device
 *  @param offset Register offset
 *  @param val Value to write
 */
void pci_config_writed(unsigned int bus,unsigned int slot,unsigned int func,size_t offset,uint32_t val) {
  uint32_t address = 0x80000000|(((uint32_t)bus)<<16)|(((uint32_t)slot)<<11)|(((uint32_t)func)<<8)|(offset&0xFC);
  outl(PCI_CONFIG_ADDRESS,address);
  outl(PCI_CONFIG_DATA,val);
}

/**
 * Reads data from PCI configuration registers
 *  @param dev DevFS device
 *  @param buf Buffer
 *  @param count How many bytes to read
 *  @param offset Offset in configuration registers
 *  @return How many bytes read
 */
ssize_t pci_read(devfs_dev_t *dev,void *buf,size_t count,off_t offset) {
  size_t count_rem = count;
  off_t dword_off = offset%4;
  offset = (offset/4)*4;
  pci_dev_t *pci = dev->user_data;

  while (count_rem>0) {
    uint32_t dword = pci_config_readd(pci->bus,pci->slot,pci->func,offset);
    size_t size_cur = count_rem>4?4:count_rem;
    memcpy(buf,(&dword)+dword_off,size_cur);

    buf += size_cur;
    count_rem -= size_cur;
    offset += 4;
    dword_off = 0;
  }
  return count;
}

/**
 * Writes data to PCI configuration registers
 *  @param dev DevFS device
 *  @param buf Buffer
 *  @param count How many bytes to write
 *  @param offset Offset in configuration registers
 *  @return How many bytes written
 */
ssize_t pci_write(devfs_dev_t *dev,void *buf,size_t count,off_t offset) {
  size_t count_rem = count;
  off_t dword_off = offset%4;
  offset = (offset/4)*4;
  pci_dev_t *pci = dev->user_data;

  while (count_rem>0) {
    uint32_t dword;
    size_t size_cur = count_rem>4?4:count_rem;
    if (dword_off>0) dword = pci_config_readd(pci->bus,pci->slot,pci->func,offset);
    memcpy((&dword)+dword_off,buf,size_cur);
    pci_config_writed(pci->bus,pci->slot,pci->func,offset,dword);

    buf += size_cur;
    count_rem -= size_cur;
    offset += 4;
    dword_off = 0;
  }
  return count;
}

/**
 * Initializes PCI device
 *  @param bus PCI bus
 *  @param slot PCI slot
 *  @param func PCI func
 *  @return Success?
 */
int pci_dev_init(int bus,int slot,int func) {
  char *name;
  asprintf(&name,"pci%d.%d.%d",bus,slot,func);
  devfs_dev_t *dev = devfs_createdev(name);
  free(name);
  if (dev!=NULL) {
    pci_dev_t *pci = malloc(sizeof(pci_dev_t));
    pci->bus = bus;
    pci->slot = slot;
    pci->func = func;
    dev->user_data = pci;
    devfs_onread(dev,pci_read);
    devfs_onwrite(dev,pci_write);
    return 0;
  }
  else return -1;
}

int main(int argc,char *argv[]) {
  unsigned int bus,slot,func;

  ioport_reg(PCI_CONFIG_DATA);
  ioport_reg(PCI_CONFIG_ADDRESS);
  devfs_init();

  for (bus=0;bus<PCI_MAXBUSSES;bus++) {
    for (slot=0;slot<PCI_MAXSLOTS;slot++) {
      for (func=0;func<PCI_MAXFUNCS;func++) {
        int vendor = pci_config_readd(bus,slot,func,0)&0x0000FFFF;
        if (vendor!=0x0000 && vendor!=0xFFFF) {
          if (pci_dev_init(bus,slot,func)==-1) fprintf(stderr,"pci: Could not create device for: pci%d.%d.%d\n",bus,slot,func);
        }
      }
    }
  }

  devfs_mainloop();

  return 0;
}
