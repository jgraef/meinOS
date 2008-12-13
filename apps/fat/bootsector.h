/*
    fat - A FAT* CDI driver
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

#ifndef _BOOTSECTOR_H_
#define _BOOTSECTOR_H_

#include <stdint.h>

#include "fat_cdi.h"

#define FAT_BOOTSECTOR_START 0

#define fat_num_sectors(bs) ((bs)->num_sectors_small!=0?(bs)->num_sectors_small:(bs)->num_sectors)

#define fat_fat_start(bs,n) (((uint64_t)(bs)->reserved_sectors)*(bs)->sector_size+fat_fat_size(bs)*n)
#define fat_fat_sectors(bs) ((uint64_t)((bs)->fat_size_small!=0?(bs)->fat_size_small:(bs)->fat32.fat_size))
#define fat_fat_size(bs)    (fat_fat_sectors(bs)*(bs)->sector_size)

/// FAT bootsector
struct fat_bootsector {
  /// Jump code
  uint8_t jump_code[3];

  /// OEM name
  uint8_t oem_name[8];

  /// Sector size
  uint16_t sector_size;

  /// Sectors per Cluster
  uint8_t cluster_size;

  /// Reserved sectors
  uint16_t reserved_sectors;

  /// FAT copies
  uint8_t num_fats;

  /// Maximum amount of directory entries in root directory
  uint16_t rootdir_max_ent;

  /// Absolute amount of sectors (small)
  uint16_t num_sectors_small;

  /// Media Descriptor Byte
  uint8_t media_desc;

  /// Sectors per FAT (small)
  uint16_t fat_size_small;

  /// Sectors per Track
  uint16_t track_size;

  /// Heads
  uint16_t num_heads;

  /// Sectors between MBR and boot sector
  uint32_t hidden_sectors;

  /// Absolute amount of sectors
  uint32_t num_sectors;

  union {
    struct {
      /// Bios driver number
      uint8_t bios_drive;

      /// Reserved
      uint8_t res0;

      /// Extended boot signature
      uint8_t ext_bootsig;

      /// Filesystem ID
      uint32_t fs_id;

      /// Filesystem Name
      uint8_t fs_name[11];

      /// FAT type
      uint8_t fat_type[8];

      /// Bootloader code
      uint8_t bootloader_code[448];

      /// Boot signature
      uint8_t bootsig[2];
    }  __attribute__ ((packed)) fat12_16;
    struct {
      /// Sectors per FAT
      uint32_t fat_size;

      /// FAT flags
      uint16_t fat_flags;

      /// FAT32 version
      uint16_t fat32_version;

      /// Cluster of root directory
      uint32_t root_cluster;

      /// FS information sector
      uint16_t info_sector;

      /// Sector of bootsector copy
      uint16_t bootsector_copy;

      /// Reserved
      uint32_t res0[3];

      /// Bios driver number
      uint8_t bios_drive;

      /// Reserved
      uint8_t res1;

      /// Extended boot signature
      uint8_t ext_bootsig;

      /// Filesystem ID
      uint32_t fs_id;

      /// Filesystem Name
      uint8_t fs_name[11];

      /// FAT type
      uint8_t fat_type[8];

      /// Bootloader code
      uint8_t bootloader_code[420];

      /// Boot signature
      uint8_t bootsig[2];
    }  __attribute__ ((packed)) fat32;
  };
} __attribute__ ((packed));

struct fat_bootsector *fat_bootsector_load(struct cdi_fs_filesystem *fs);

#endif
