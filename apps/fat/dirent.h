/*
 * fat - A FAT* CDI driver
 *
 * Copyright (C) 2008 Janosch Gräf
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _DIRENT_H_
#define _DIRENT_H_

#include <stdint.h>

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
  uint16_t res0[5];

  /// Creation date
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
  } __attribute__ ((packed)) date;

  /// First cluster
  uint16_t first_cluster;

  /// File size
  uint32_t file_size;
} __attribute__ ((packed));

#endif
