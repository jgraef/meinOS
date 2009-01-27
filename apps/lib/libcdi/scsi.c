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

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/shm.h>
#include <rpc.h>
#include <errno.h>

#include "cdi.h"
#include "cdi/scsi.h"
#include "cdi/lists.h"

static cdi_list_t cdi_scsi_devices = NULL;

struct cdi_scsi_packet* cdi_scsi_packet_alloc(size_t size) {
  struct cdi_scsi_packet *packet = malloc(sizeof(struct cdi_scsi_packet));
  memset(packet,0,sizeof(struct cdi_scsi_packet));

  packet->bufsize = size;
  packet->buffer = malloc(size);

  return packet;
}

/**
 * Ein SCSI-Paket freigeben
 *
 * @param packet Pointer auf das Paket
 */
void cdi_scsi_packet_free(struct cdi_scsi_packet* packet) {
  free(packet->buffer);
  free(packet);
}

/**
 * SCSI-Request
 *  @param devname Device name
 *  @param shmid SHMID for buffer
 *  @param bufsize Size of buffer
 *  @param direction Direction
 *  @param cmdd1 Command dword 1
 *  @param cmdd2 Command dword 2
 *  @param cmdd3 Command dword 3
 *  @param cmdd4 Command dword 4
 *  @param cmdsize Size of command
 *  @return Success?
 */
static int cdi_scsi_request(char *devname,int shmid,size_t bufsize,int direction,uint32_t cmdd1,uint32_t cmdd2,uint32_t cmdd3,uint32_t cmdd4,size_t cmdsize) {
  uint32_t cmd[4] = {cmdd1,cmdd2,cmdd3,cmdd4};

  struct cdi_scsi_device *dev;
  size_t i;
  for (i=0;(dev = cdi_list_get(cdi_scsi_devices,i));i++) {
    if (strcmp(dev->dev.name,devname)==0) {
      int ret = 0;
      void *shmbuf = shmat(shmid,NULL,0);
      if (shmbuf!=NULL) {
        struct cdi_scsi_packet packet = {
          .buffer = shmbuf,
          .bufsize = bufsize,
          .cmdsize = cmdsize,
          .direction = bufsize==0?CDI_SCSI_NODATA:(direction?CDI_SCSI_WRITE:CDI_SCSI_READ)
        };
        memcpy(&packet.command,&cmd,sizeof(packet.command));
        ret = ((struct cdi_scsi_driver*)(dev->dev.driver))->request(dev,&packet);
      }
      else ret = -ENOMEM;
      shmdt(shmbuf);
      return ret;
    }
  }
  return -ENOENT;
}

/**
 * Initialisiert die Datenstrukturen fuer einen SCSI-Treiber
 */
void cdi_scsi_driver_init(struct cdi_scsi_driver* driver) {
  cdi_driver_init((struct cdi_driver*)driver);
}

/**
 * Deinitialisiert die Datenstrukturen fuer einen SCSI-Treiber
 *  @todo remove device from cdi_scsi_devices
 */
void cdi_scsi_driver_destroy(struct cdi_scsi_driver* driver) {
  cdi_driver_destroy((struct cdi_driver*)driver);
}

/**
 * Registiert einen SCSI-Treiber
 */
void cdi_scsi_driver_register(struct cdi_scsi_driver* driver) {
  cdi_driver_register((struct cdi_driver*)driver);

  if (cdi_scsi_devices==NULL) cdi_scsi_devices = cdi_list_create();

  struct cdi_scsi_device *dev;
  size_t i;
  for (i=0;(dev = cdi_list_get(driver->drv.devices,i));i++) {
    char *name;
    asprintf(&name,"scsi_request_%s",dev->dev.name);
    rpc_func_create(name,cdi_scsi_request,"$iiiddddi",NAME_MAX+sizeof(int)*4+16);
    free(name);
    cdi_list_push(cdi_scsi_devices,dev);
  }
}

void cdi_scsi_device_init(struct cdi_scsi_device* device) {

}
