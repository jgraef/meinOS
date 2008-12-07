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

#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <stdint.h>
#include <devfs.h>

#define CDROM_REQUEST_SENSE 0x03
#define CDROM_INQUIRY       0x12
#define CDROM_START_STOP    0x1B
#define CDROM_LOCK          0x1E
#define CDROM_READ_CAPACITY 0x25
#define CDROM_READ          0xA8

#define CDROM_BUFSIZE DEVFS_BUFSIZE

#define CDROM_R 0
#define CDROM_W 1

#define CDROM_TYPE 0x05

struct cdrom_device {
  char *name;
  char *rpc_func;
  uint16_t vendor_id;
  uint16_t product_id;
  int started;
  int loaded;
  int locked;
  size_t block_count;
  size_t block_size;
  devfs_dev_t *devfs;
};

typedef union {
  struct {
    uint8_t opcode;
    uint8_t res0;
    uint32_t lba;
    uint8_t res1;
    uint16_t length;
    uint8_t res2[3];
  } __attribute__ ((packed)) dfl;
  struct {
    uint8_t opcode;
    uint8_t res0;
    uint32_t lba;
    uint32_t length;
    uint16_t res1;
  } __attribute__ ((packed)) ext;
  uint8_t raw8[12];
  uint16_t raw16[6];
  uint32_t raw32[3];
} cdrom_cmd_t;

struct cdrom_inquiry_data {
  unsigned type:5;
  unsigned :10;
  unsigned rmb:1;
  unsigned ansi_version:3;
  unsigned ecma_version:3;
  unsigned iso_version:2;
  unsigned response_type:4;
  unsigned atapi_version:4;
  uint8_t additional_length;
  unsigned :24;
  uint16_t vendor_id;
  uint16_t product_id;
  uint16_t product_rev_level;
} __attribute__ ((packed));

struct cdrom_read_capacity_data {
  uint32_t lba;
  uint32_t block_size;
} __attribute__ ((packed));

struct cdrom_request_sense_data {
  unsigned error_code:7;
  unsigned valid:1;
  uint8_t segment_number;
  unsigned sense_key:4;
  unsigned res0:1;
  unsigned ili:1;
  unsigned res1:2;
  uint16_t information;
  uint8_t additionaL_sense_length;
  uint16_t command_specific_information;
  uint8_t additional_sense_code;
  uint8_t additional_sense_code_qualifier;
  uint8_t field_replacable_unit_code;
  unsigned sense_key_specific:15;
  unsigned sense_key_valid:1;
  uint8_t additional_sense_bytes[0];
} __attribute__ ((packed));

struct cdrom_request_sense_data *cdrom_request_sense(struct cdrom_device *dev);
int cdrom_inquiry(struct cdrom_device *dev);
int cdrom_start(struct cdrom_device *dev,int start,int load);
int cdrom_lock(struct cdrom_device *dev,int lock);
int cdrom_read_capacity(struct cdrom_device *dev);
void *cdrom_read(struct cdrom_device *dev,size_t first_block,size_t block_count);
struct cdrom_device *cdrom_device_create(const char *name);
void cdrom_device_destroy(struct cdrom_device *dev);
int cdrom_buf_init();

static inline int cdrom_load(struct cdrom_device *dev,int load) {
  return cdrom_start(dev,dev->started,load);
}

static inline uint32_t bei(uint32_t v) {
  uint32_t r;
  swab(&v,&r,4);
  return r;
}

static inline uint16_t bes(uint16_t v) {
  uint16_t r;
  swab(&v,&r,2);
  return r;
}

#endif
