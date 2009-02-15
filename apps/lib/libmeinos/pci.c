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

#include <llist.h>
#include <pci.h>
#include <ioport.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <dirent.h>
#include <stdio.h>

struct pci_config {
  uint16_t vendor_id;
  uint16_t device_id;
  uint16_t command;
  uint16_t status;
  uint8_t revision;
  uint8_t baseclass;
  uint8_t subclass;
  uint8_t interface;
  uint8_t cache_line_size;
  uint8_t latency_timer;
  uint8_t header_type;
  uint8_t bist;
  uint32_t bar[6];
  uint32_t cardbus_cis_ptr;
  uint16_t subsystem_vendor_id;
  uint16_t subsystem_id;
  uint32_t exp_rom_bar;
  uint32_t reserved[2];
  uint8_t interrupt_line;
  uint8_t interrupt_pin;
  uint8_t min_grant;
  uint8_t max_latency;
};

static llist_t pci_devices = NULL;

static int pci_read_config(struct pci_device *pci) {
  int ret = 0;
  char *filename;
  asprintf(&filename,"/dev/pci%d.%d.%d",pci->bus,pci->dev,pci->function);
  int fh = open(filename,O_RDWR);
  free(filename);

  if (fh!=-1) {
    struct pci_config conf;
    read(fh,&conf,sizeof(conf));

    if (conf.header_type==0 || conf.header_type==1) {
      pci->vendor_id = conf.vendor_id;
      pci->device_id = conf.device_id;
      pci->class_id = conf.baseclass;
      pci->subclass_id = conf.subclass;
      pci->interface_id = conf.interface;
      pci->rev_id = conf.revision;
      pci->irq = conf.interrupt_line; /// @todo Is that right?
      pci->resources = llist_create();

      size_t i;
      for (i=0;i<6;i++) {
        if (conf.bar[i]&1) {
          struct pci_resource *res = malloc(sizeof(struct pci_resource));
          res->start = conf.bar[i]&0xFFFFFFF0;
          res->type = (conf.bar[i]&1)?PCI_IOPORTS:PCI_MEMORY;
          res->index = i;
          lseek(fh,offsetof(struct pci_config,bar[i]),SEEK_SET);
          uint32_t bar_tmp = 0xFFFFFFF0|(conf.bar[i]&1);
          write(fh,&bar_tmp,4);
          lseek(fh,offsetof(struct pci_config,bar[i]),SEEK_SET);
          read(fh,&(res->length),4);
          res->length = (~res->length|0xF)+1;
          lseek(fh,offsetof(struct pci_config,bar[i]),SEEK_SET);
          write(fh,conf.bar+i,4);
          llist_push(pci->resources,res);
        }
      }
    }
    else ret = -1;

    close(fh);
  }
  else ret = -1;

  return ret;
}

void pci_get_all_devices(llist_t list) {
  if (pci_devices==NULL) {
    DIR *dir = opendir("/dev");
    if (dir!=NULL) {
      struct dirent *ent;
      do {
        ent = readdir(dir);
        if (ent!=NULL) {
          if (strncmp(ent->d_name,"pci",3)==0) {
            struct pci_device *new = malloc(sizeof(struct pci_device));
            if (sscanf(ent->d_name,"pci%d.%d.%d",&(new->bus),&(new->dev),&(new->function))==3) {
              if (pci_read_config(new)!=-1) llist_push(list,new);
            }
            else free(new);
          }
        }
      } while (ent!=NULL);
      closedir(dir);
    }
  }
}

void pci_device_destroy(struct pci_device* device) {
  struct pci_resource *res;

  pci_free_ioports(device);
  pci_free_memory(device);
  while ((res = llist_pop(device->resources))) free(res);
  llist_destroy(device->resources);
  free(device);
}

void pci_alloc_ioports(struct pci_device* device) {
  struct pci_resource *res;
  size_t i;

  for (i=0;(res = llist_get(device->resources,i));i++) {
    if (res->type==PCI_IOPORTS) {
      size_t j;
      for (j=0;j<res->length;j++) ioport_reg(res->start+j);
    }
  }
}

void pci_free_ioports(struct pci_device* device) {
  struct pci_resource *res;
  size_t i;

  for (i=0;(res = llist_get(device->resources,i));i++) {
    if (res->type==PCI_IOPORTS) {
      size_t j;
      for (j=0;j<res->length;j++) ioport_unreg(res->start+j);
    }
  }
}

void pci_alloc_memory(struct pci_device *device) {
  struct pci_resource *res;
  size_t i;

  for (i=0;(res = llist_get(device->resources,i));i++) {
    if (res->type==PCI_MEMORY) res->address = NULL; /// @todo
  }
}

void pci_free_memory(struct pci_device *device) {
  struct pci_resource *res;
  size_t i;

  for (i=0;(res = llist_get(device->resources,i));i++) {
    if (res->type==PCI_MEMORY); /// @todo
  }
}