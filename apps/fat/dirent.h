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

#ifndef _DIRENT_H_
#define _DIRENT_H_

#include <stdint.h>

#define fat32_first_cluster(de) ((((uint32_t)(de)->first_cluster_high)<<16)|(de)->first_cluster)

struct fat_dirent {
  /// Filename
  uint8_t filename[8];

  /// Filename extension
  uint8_t filename_ext[3];

  /// File attributes
  struct {
    /// Read only
    unsigned readonly:1;

    /// File is hidden
    unsigned hidden:1;

    /// System file
    unsigned system:1;

    /// Volume label
    unsigned volume:1;

    /// Directory
    unsigned dir:1;

    /// Archiv
    unsigned archiv:1;

    unsigned :2;
  } __attribute__ ((packed)) attr;

  /// Reserved
  uint8_t res0;

  /// Creation date
  struct {
    /// Decisecond
    uint8_t deci_second;

    /// Hour
    unsigned hour:5;

    /// Minute
    unsigned minute:6;

    /// 2 Seconds
    unsigned two_secs:5;

    /// Year (since 1980)
    unsigned year:7;

    /// Month
    unsigned month:4;

    /// Day of month
    unsigned day:5;
  } __attribute__ ((packed)) create_date;

  /// Last access date
  struct {
    /// Year (since 1980)
    unsigned year:7;

    /// Month
    unsigned month:4;

    /// Day of month
    unsigned day:5;
  } __attribute__ ((packed)) access_date;

  /// Higher 16 bits of first cluster (for FAT32)
  uint16_t first_cluster_high;

  /// Last write date
  struct {
    /// Hour
    unsigned hour:5;

    /// Minute
    unsigned minute:6;

    /// 2 Seconds
    unsigned two_secs:5;

    /// Year (since 1980)
    unsigned year:7;

    /// Month
    unsigned month:4;

    /// Day of month
    unsigned day:5;
  } __attribute__ ((packed)) write_date;

  /// First cluster
  uint16_t first_cluster;

  /// File size
  uint32_t file_size;
} __attribute__ ((packed));

struct fat_dirent_long {
  /// Order
  uint8_t order;

  /// Name (part 1)
  uint16_t name1[5];

  /// File attributes
  struct {
    /// Read only
    unsigned readonly:1;

    /// File is hidden
    unsigned hidden:1;

    /// System file
    unsigned system:1;

    /// Volume label
    unsigned volume:1;

    /// Directory
    unsigned dir:1;

    /// Archiv
    unsigned archiv:1;

    unsigned :2;
  } __attribute__ ((packed)) attr;

  /// Type of log directory entry (should be 0)
  uint8_t type;

  /// Checksum of name
  uint8_t checksum;

  /// Name (part 2)
  uint16_t name2[6];

  /// Zero
  uint16_t zero;

  /// Name (part 3)
  uint16_t name3[2];
} __attribute__ ((packed));

#endif
