/*
 * Copyright (c) 2009 Janosch Gräf
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#ifndef _CDI_PCI_H_
#define _CDI_PCI_H_

#include <stddef.h>
#include <cdi/lists.h>
#include <stdint.h>

struct cdi_pci_device {
  uint16_t bus;
  uint16_t dev;
  uint16_t function;
  uint16_t vendor_id;
  uint16_t device_id;
  uint16_t class_id;
  uint8_t irq;
  cdi_list_t resources;
};

typedef enum {
  CDI_PCI_MEMORY,
  CDI_PCI_IOPORTS
} cdi_res_t;

struct cdi_pci_resource {
  cdi_res_t type;
  uint32_t start;
  size_t length;
  unsigned int index;
  // If resource is memory: virtual address where memory is mapped
  void *address;
};

void cdi_pci_get_all_devices(cdi_list_t list);
void cdi_pci_device_destroy(struct cdi_pci_device* device);
void cdi_pci_alloc_ioports(struct cdi_pci_device* device);
void cdi_pci_free_ioports(struct cdi_pci_device* device);
void cdi_pci_alloc_memory(struct cdi_pci_device* device);
void cdi_pci_free_memory(struct cdi_pci_device* device);

#endif
