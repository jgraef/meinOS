/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _LIBMEINOS_PCI_H_
#define _LIBMEINOS_PCI_H_

#include <sys/types.h>
#include <stdint.h>

#define PCI_MAXBUSSES 256
#define PCI_MAXSLOTS  32
#define PCI_MAXFUNCS  8

#define PCI_VENDOR_NONE         0xFFFF
#define PCI_VENDOR_MICROSOFT    0x045E
#define PCI_VENDOR_ATI          0x1002
#define PCI_VENDOR_IBM          0x1014
#define PCI_VENDOR_HP           0x103C
#define PCI_VENDOR_SONY         0x104D
#define PCI_VENDOR_SUN          0x108E
#define PCI_VENDOR_PACKARDBELL  0x109A
#define PCI_VENDOR_NVIDIA       0x10DE
#define PCI_VENDOR_REALTEK      0x10EC
#define PCI_VENDOR_CREATIVELABS 0x1102
#define PCI_VENDOR_ATMEL        0x1114
#define PCI_VENDOR_INTEL        0x8086
// for more vendors see http://pci-ids.ucw.cz/iii//?p=%2A or /etc/pci.ids

#define PCI_CLASSCODE_PRE2        0x00
#define PCI_CLASSCODE_MASSSTORAGE 0x01
#define PCI_CLASSCODE_NETWORK     0x02
#define PCI_CLASSCODE_DISPLAY     0x03
#define PCI_CLASSCODE_MULTIMEDIA  0x04
#define PCI_CLASSCODE_MEMORY      0x05
#define PCI_CLASSCODE_BRIDGE      0x06
#define PCI_CLASSCODE_SIMPLECOM   0x07
#define PCI_CLASSCODE_BASESYSPER  0x08
#define PCI_CLASSCODE_INPUT       0x09
#define PCI_CLASSCODE_DOCKING     0x0A
#define PCI_CLASSCODE_PROCESSORS  0x0B
#define PCI_CLASSCODE_SERIALBUS   0x0C
#define PCI_CLASSCODE_NONE        0xFE // is that allowed?
#define PCI_CLASSCODE_MISC        0xFF

#define PCI_SUBCLASS_NONE               0xFF // is that allowed?
// Pre 2.0
#define PCI_SUBCLASS_PRE2_OTHERS        0x00
#define PCI_SUBCLASS_PRE2_VGA           0x01
// Mass Storage
#define PCI_SUBCLASS_MASSSTORAGE_SCSI   0x00
#define PCI_SUBCLASS_MASSSTORAGE_IDE    0x01
#define PCI_SUBCLASS_MASSSTORAGE_FLOPPY 0x02
#define PCI_SUBCLASS_MASSSTORAGE_IPI    0x02
#define PCI_SUBCLASS_MASSSTORAGE_RAID   0x04
#define PCI_SUBCLASS_MASSSTORAGE_OTHERS 0x80
// Network
#define PCI_SUBCLASS_NETWORK_ETHERNET   0x00
#define PCI_SUBCLASS_NETWORK_TOKENRING  0x01
#define PCI_SUBCLASS_NETWORK_FDDI       0x02
#define PCI_SUBCLASS_NETWORK_ATM        0x03
#define PCI_SUBCLASS_NETWORK_OTHERS     0x80
// Display
#define PCI_SUBCLASS_DISPLAY_VGA        0x00
#define PCI_SUBCLASS_DISPLAY_XGA        0x01
#define PCI_SUBCLASS_DISPLAY_OTHERS     0x80
// Multimedia
#define PCI_SUBCLASS_MULTIMEDIA_VIDEO   0x00
#define PCI_SUBCLASS_MULTIMEDIA_AUDIO   0x01
#define PCI_SUBCLASS_MULTIMEDIA_OTHERS  0x80
// Memory
#define PCI_SUBCLASS_MEMORY_RAM         0x00
#define PCI_SUBCLASS_MEMORY_FLASH       0x01
#define PCI_SUBCLASS_MEMORY_OTHERS      0x80
// Bridge
#define PCI_SUBCLASS_BRIDGE_HOST_PCI    0x00
#define PCI_SUBCLASS_BRIDGE_PCI_ISA     0x01
#define PCI_SUBCLASS_BRIDGE_PCI_EISA    0x02
#define PCI_SUBCLASS_BRIDGE_PCI_MC      0x03
#define PCI_SUBCLASS_BRIDGE_PCI_PCI     0x04
#define PCI_SUBCLASS_BRIDGE_PCI_PCMCIA  0x05
#define PCI_SUBCLASS_BRIDGE_PCI_NUBUS   0x06
#define PCI_SUBCLASS_BRIDGE_PCI_CARDBUS 0x07
#define PCI_SUBCLASS_BRIDGE_OTHERS      0x80
// Simple Communication
#define PCI_SUBCLASS_SIMPLECOM_SERIAL   0x00
#define PCI_SUBCLASS_SIMPLECOM_PARALLEL 0x01
#define PCI_SUBCLASS_SIMPLECOM_OTHERS   0x80
// Base System Peripherals
#define PCI_SUBCLASS_BASESYSPER_PIC     0x00
#define PCI_SUBCLASS_BASESYSPER_DMA     0x01
#define PCI_SUBCLASS_BASESYSPER_TIMER   0x02
#define PCI_SUBCLASS_BASESYSPER_RTC     0x03
#define PCI_SUBCLASS_BASESYSPER_OTHERS  0x80
// Input Devices
#define PCI_SUBCLASS_INPUT_KEYBOARD     0x00
#define PCI_SUBCLASS_INPUT_DIGITIZER    0x01
#define PCI_SUBCLASS_INPUT_MOUSE        0x02
#define PCI_SUBCLASS_INPUT_OTHERS       0x80
// Docking Stations
#define PCI_SUBCLASS_DOCKING_GENERIC    0x00
#define PCI_SUBCLASS_DOCKING_OTHERS     0x80
// Processors
#define PCI_SUBCLASS_PROCESSORS_386     0x00
#define PCI_SUBCLASS_PROCESSORS_486     0x01
#define PCI_SUBCLASS_PROCESSORS_PENTIUM 0x02
#define PCI_SUBCLASS_PROCESSORS_ALPHA   0x10
#define PCI_SUBCLASS_PROCESSORS_PPC     0x20
#define PCI_SUBCLASS_PROCESSORS_CO      0x40
// Serial Bus
#define PCI_SUBCLASS_SERIALBUS_FIREWIRE 0x00
#define PCI_SUBCLASS_SERIALBUS_ACCESS   0x01
#define PCI_SUBCLASS_SERIALBUS_SSA      0x02
#define PCI_SUBCLASS_SERIALBUS_USB      0x03

#define pci_config_readw(dev,offset) (pci_config_readd(dev,offset&0xFC)>>((offset&0x2)*8))
#define pci_config_readb(dev,offset) (pci_config_readd(dev,offset&0xFC)>>((offset&0x3)*8))

#define pci_get_vendorid(dev)             pci_config_readw(dev,0)
#define pci_get_deviceid(dev)             pci_config_readw(dev,2)
#define pci_get_classcode(dev)            pci_config_readb(dev,11)
#define pci_get_subclass(dev)             pci_config_readb(dev,10)
#define pci_get_present(dev)              (pci_get_vendorid(dev)!=PCI_VENDOR_NONE && pci_get_vendorid(dev)!=0)
#define pci_get_irq(dev)                  pci_config_readb(dev,0x3C)

typedef struct {
  unsigned int bus;
  unsigned int slot;
  unsigned int func;
} pcidev_t;

typedef struct {
  enum {
   PCI_BARTYPE_MEMORY = 0,
   PCI_BARTYPE_IOPORT = 1
  } type;
  void* addr;
  size_t size;
  uint32_t ioport;
} pcibar_t;

uint32_t pci_config_readd(pcidev_t *dev,size_t offset);
void pci_config_writed(pcidev_t *dev,size_t offset,uint32_t val);
int pci_finddev_byids(pcidev_t *dev,int vendorid,int deviceid,int index);
int pci_finddev_byclass(pcidev_t *dev,int classcode,int subclass,int index);
pcibar_t *pci_getbar(pcidev_t *dev,int barnum);

#endif
