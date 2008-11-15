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

#include <sys/types.h>
#include <stdint.h>
#include <ioport.h>
#include <stdlib.h>
#include <pci.h>

/**
 * Reads dword from config register of PCI device
 *  @param dev PCI device
 *  @param offset Register offset
 *  @return Register content
 */
uint32_t pci_config_readd(pcidev_t *dev,size_t offset) {
  return 0;
}

/**
 * Writes dword in config register of PCI device
 *  @param dev PCI device
 *  @param offset Register offset
 *  @param val Value to write
 */
void pci_config_writed(pcidev_t *dev,size_t offset,uint32_t val) {

}


/**
 * Finds PCI device by VendorID and DeviceID
 *  @param dev Pointer to PCI device
 *  @param vendorid VendorID
 *  @param deviceid DeviceID
 *  @param index If more than 1 device is found, select n'th one
 *  @return Whether device was found
 */
int pci_finddev_byids(pcidev_t *dev,int vendorid,int deviceid,int index) {
  for (dev->bus=0;dev->bus<PCI_MAXBUSSES;dev->bus++) {
    for (dev->slot=0;dev->slot<PCI_MAXSLOTS;dev->slot++) {
      if (pci_get_vendorid(dev)==vendorid && pci_get_deviceid(dev)==deviceid) {
        if (!index) return 1;
        else index--;
      }
    }
  }
  return 0;
}

/**
 * Finds PCI device by Classcode and Subclass
 *  @param dev Pointer to PCI device
 *  @param classcode Classcode
 *  @param subclass Subclass
 *  @param index If more than 1 device is found, select n'th one
 *  @return Whether device was found
 */
int pci_finddev_byclass(pcidev_t *dev,int classcode,int subclass,int index) {
  for (dev->bus=0;dev->bus<PCI_MAXBUSSES;dev->bus++) {
    for (dev->slot=0;dev->slot<PCI_MAXSLOTS;dev->slot++) {
      if (pci_get_subclass(dev)==classcode && pci_get_subclass(dev)==subclass) {
        if (!index) return 1;
        else index--;
      }
    }
  }
  return 0;
}

/**
 * Returns Pointer to BAR structure
 *  @param dev PCI device
 *  @param barnum Number of BAR register (0..5)
 *  @return Pointer to BAR structure
 */
pcibar_t *pci_getbar(pcidev_t *dev,int barnum) {
  pcibar_t *bar = malloc(sizeof(pcibar_t));
  uint32_t bar_value = pci_config_readd(dev,barnum*4+0x10);
  bar->type = bar_value&1;
  if (bar->type==PCI_BARTYPE_MEMORY) {
    bar->addr = (void*)(bar_value&0xFFFFFFF0);
    pci_config_writed(dev,barnum*4+0x10,0xFFFFFFF0|bar->type);
    bar->size = pci_config_readd(dev,barnum*4+0x10);
    bar->size = (~bar->size|0xF)+1;
    pci_config_writed(dev,barnum*4+0x10,bar_value);
  }
  else bar->ioport = bar_value&0xFFFFFFFC;
  return bar;
}
