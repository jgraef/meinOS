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
#include <sys/shm.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <rpc.h>
#include <unistd.h>

#include "device.h"

//#define DEBUG

static void debug(const char *fmt,...) {
#ifdef DEBUG
  va_list args;
  va_start(args,fmt);
  fprintf(stderr,"cdrom: ");
  vfprintf(stderr,fmt,args);
  va_end(args);
#endif
}

static int cdrom_shmid;
static void *cdrom_shmbuf;
struct {
  enum {
    CDROM_SK_NOSENSE = 0x0,
    CDROM_SK_RECOVERED_ERROR = 0x1,
    CDROM_SK_NOT_READY = 0x2,
    CDROM_SK_MEDIUM_ERROR = 0x3,
    CDROM_SK_HARDWARE_ERROR = 0x4,
    CDROM_SK_ILLEGAL_REQUEST = 0x5,
    CDROM_SK_UNIT_ATTENTION = 0x6,
    CDROM_SK_DATA_PROTECT = 0x7,
    CDROM_SK_ABORTED_COMMAND = 0xB,
    CDROM_SK_MISCOMPARE = 0xE
  } sense_key;
  unsigned int asc;
  unsigned int ascq;
} cdrom_error;

static int cdrom_command(struct cdrom_device *dev,cdrom_cmd_t *cmd,int dir,size_t count) {
  if (count>CDROM_BUFSIZE) return -ERANGE;
  int sense_key = rpc_call(dev->rpc_func,0,dev->name,cdrom_shmid,count,dir,cmd->raw32[0],cmd->raw32[1],cmd->raw32[2],0,sizeof(cdrom_cmd_t));
  if (sense_key==CDROM_SK_NOSENSE) return 0;
  else {
    // p55, p183, p179
    struct cdrom_request_sense_data *rqs = cdrom_request_sense(dev);
    cdrom_error.sense_key = sense_key;
    cdrom_error.asc = rqs->additional_sense_code;
    cdrom_error.ascq = rqs->additional_sense_code_qualifier;
    return -1;
  }
}

struct cdrom_request_sense_data *cdrom_request_sense(struct cdrom_device *dev) {
  cdrom_cmd_t command;

  command.dfl.opcode = CDROM_REQUEST_SENSE;
  command.raw8[4] = sizeof(struct cdrom_request_sense_data);
  if (cdrom_command(dev,&command,CDROM_R,sizeof(struct cdrom_inquiry_data))!=-1) {
    debug("%s request sense...\n",dev->name);
    return cdrom_shmbuf;
  }
  else return NULL;
}

int cdrom_inquiry(struct cdrom_device *dev) {
  cdrom_cmd_t command;
  struct cdrom_inquiry_data *inq = cdrom_shmbuf;

  command.dfl.opcode = CDROM_INQUIRY;
  command.raw8[4] = sizeof(struct cdrom_inquiry_data);
  if (cdrom_command(dev,&command,CDROM_R,sizeof(struct cdrom_inquiry_data))!=-1) {
    debug("%s inquiry...\n",dev->name);
    if (inq->type==5 && inq->rmb==1 && inq->atapi_version>=2 && inq->response_type==1) {
      dev->vendor_id = bes(inq->vendor_id);
      dev->product_id = bes(inq->product_id);
      return 0;
    }
  }
  return -1;
}

int cdrom_start(struct cdrom_device *dev,int start,int load) {
  cdrom_cmd_t command;

  command.dfl.opcode = CDROM_START_STOP;
  command.raw8[1] = 0;
  command.raw8[4] = (start?1:0)|(load?2:0);
  if (cdrom_command(dev,&command,0,0)!=-1) {
    debug("%s start: start=%d; load=%d\n",dev->name,start,load);
    dev->started = start;
    dev->loaded = load;
    return 0;
  }
  else return -1;
}

int cdrom_lock(struct cdrom_device *dev,int lock) {
  cdrom_cmd_t command;

  command.dfl.opcode = CDROM_LOCK;
  command.raw8[4] = lock?1:0;
  if (cdrom_command(dev,&command,0,0)!=-1) {
    debug("%s lock: lock=%d\n",dev->name,lock);
    dev->locked = lock;
    return 0;
  }
  else return -1;
}

int cdrom_read_capacity(struct cdrom_device *dev) {
  cdrom_cmd_t command;
  struct cdrom_read_capacity_data *cap = cdrom_shmbuf;

  command.dfl.opcode = CDROM_READ_CAPACITY;
  if (cdrom_command(dev,&command,CDROM_R,sizeof(struct cdrom_read_capacity_data))!=-1) {
    dev->block_count = bei(cap->lba);
    dev->block_size = bei(cap->block_size);
    debug("%s read_capacity: lba=0x%x; block_size=0x%x; size=%dkB\n",dev->name,dev->block_count,dev->block_size,(dev->block_count*dev->block_size)/1024);
    return 0;
  }
  else return -1;
}

void *cdrom_read(struct cdrom_device *dev,size_t first_block,size_t block_count) {
  debug("%s read: first_block=0x%x block_count=0x%x\n",dev->name,first_block,block_count);
  cdrom_cmd_t command;

  command.ext.opcode = CDROM_READ;
  command.ext.lba = bei(first_block);
  command.ext.length = bei(block_count);
  return cdrom_command(dev,&command,CDROM_R,block_count*dev->block_size)!=-1?cdrom_shmbuf:NULL;
}

struct cdrom_device *cdrom_device_create(const char *name) {
  struct cdrom_device *dev = malloc(sizeof(struct cdrom_device));
  memset(dev,0,sizeof(struct cdrom_device));

  dev->name = strdup(name);
  asprintf(&(dev->rpc_func),"scsi_request_%s",name);

  return dev;
}

void cdrom_device_destroy(struct cdrom_device *dev) {
  free(dev->name);
  free(dev->rpc_func);
}

int cdrom_buf_init() {
  cdrom_shmid = shmget(IPC_PRIVATE,CDROM_BUFSIZE,0);
  if (cdrom_shmid!=-1) {
    cdrom_shmbuf = shmat(cdrom_shmid,NULL,0);
    if (cdrom_shmbuf!=NULL) return 0;
    else shmctl(cdrom_shmid,IPC_RMID,NULL);
  }
  memset(&cdrom_error,0,sizeof(cdrom_error));
  return -1;
}
