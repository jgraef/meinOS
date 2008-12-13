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

#include <stdlib.h>

#include "cdi/fs.h"

#include "fat_cdi.h"
#include "bootsector.h"

struct fat_bootsector *fat_bootsector_load(struct cdi_fs_filesystem *fs) {
  debug("fat_bootsector_load(0x%x)\n",fs);
  struct fat_bootsector *bootsector = malloc(sizeof(struct fat_bootsector));

  return (cdi_fs_data_read(fs,FAT_BOOTSECTOR_START,sizeof(struct fat_bootsector),bootsector)==sizeof(struct fat_bootsector))?bootsector:NULL;
}

fat_type_t fat_bootsector_type(struct fat_bootsector *bootsector) {
  return FAT12;
}
