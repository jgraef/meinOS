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

#include <stddef.h>

#include "cdi/fs.h"
#include "cdi/lists.h"

#include "fat_cdi.h"
#include "dirent.h"
#include "cluster.h"
#ifdef FAT_LOWER_FILENAMES
  #include <ctype.h>
#endif

static int check_filename(char *name) {
  size_t i,j;
  char illegal[] = {0x22,0x2A,0x2B,0x2C,0x2E,0x2F,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,0x5B,0x5C,0x5D,0x7C};

  if (name[0]==' ') return 0;
  for (i=0;i<11;i++) {
    if (name[i]<' ') return 0;
    for (j=0;j<sizeof(illegal);j++) {
      if (name[i]==illegal[j]) return 0;
    }
  }
  return 1;
}

static char *parse_name(char *name,char *ext) {
  size_t i;
  size_t name_len = 0;
  size_t ext_len = 0;

  for (i=0;i<8;i++) {
    if (name[i]!=' ') name_len = i+1;
  }
  for (i=0;i<3;i++) {
    if (ext[i]!=' ') ext_len = i+1;
  }

  int dot = ext_len>0?1:0;
  char *filename = malloc(name_len+ext_len+dot+1);
  memcpy(filename,name,name_len);
  if (dot) {
    filename[name_len] = '.';
    memcpy(filename+name_len+1,ext,ext_len);
  }
  filename[name_len+ext_len+dot] = 0;

#ifdef FAT_LOWER_FILENAMES
  for (i=0;i<filename[i];i++) filename[i] = tolower(filename[i]);
#endif

  return filename;
}

/**
 * Converts UCS-2 to UTF-8
 *  @param dest Destination for utf-8 data
 *  @param src Source of UCS-2 data
 *  @param n Num of elements (16bit-elements)
 *  @todo Implement
 */
static void ucs2toutf8(uint8_t *dest,uint16_t *src,size_t n) {
  size_t i;

  for (i=0;i<n;i++) dest[i] = src[i]&0xFF;
}

static struct fat_fs_res *fat_dirent_load(struct fat_fs_res *parent,struct fat_dirent *dirent) {
  struct fat_fs_filesystem *fat_fs = parent->fs->opaque;
  static cdi_list_t long_dirents = NULL;

  if (dirent->attr.hidden && dirent->attr.system && dirent->attr.volume && dirent->attr.readonly && ((struct fat_dirent_long*)dirent)->type==0) {
    if (long_dirents==NULL) long_dirents = cdi_list_create();
    cdi_list_push(long_dirents,memcpy(malloc(sizeof(struct fat_dirent)),dirent,sizeof(struct fat_dirent)));
  }
  else if (!dirent->attr.volume && !dirent->attr.system && !dirent->attr.hidden) {
    if (dirent->filename[0]!='.') {
      char *name;
      if (long_dirents==NULL) {
        if (!check_filename((char*)dirent->filename)) return NULL;
        name = parse_name((char*)dirent->filename,(char*)dirent->filename_ext);
      }
      else {
        struct fat_dirent_long *long_dirent;
        /// @todo Measure correctly how many bytes needed (UTF-8)
        name = malloc(cdi_list_size(long_dirents)*13+1);
        name[cdi_list_size(long_dirents)*13] = 0;
        size_t cur = 0;
        while ((long_dirent = cdi_list_pop(long_dirents))) {
          ucs2toutf8((uint8_t*)name+cur,long_dirent->name1,5);
          ucs2toutf8((uint8_t*)name+cur+5,long_dirent->name2,6);
          ucs2toutf8((uint8_t*)name+cur+11,long_dirent->name3,2);
          cur += 13;
          free(long_dirent);
        }
        cdi_list_destroy(long_dirents);
        long_dirents = NULL;
      }
      cdi_fs_res_class_t class = dirent->attr.dir?CDI_FS_CLASS_DIR:CDI_FS_CLASS_FILE;
      cdi_fs_res_type_t type = 0;
      struct fat_fs_res *res = fat_fs_res_create(name,parent,class,type);
      res->readonly = dirent->attr.readonly;
      res->filesize = dirent->file_size;
      if (fat_fs->type==FAT32) res->clusters = fat_clchain_create(parent->fs,fat32_first_cluster(dirent));
      else res->clusters = fat_clchain_create(parent->fs,dirent->first_cluster);
      free(name);
      return res;
    }
  }

  return NULL;
}

cdi_list_t fat12_16_rootdir_load(struct cdi_fs_filesystem *fs) {
  cdi_list_t dirlist = cdi_list_create();
  size_t i;

  for (i=0;;i++) {
    struct fat_dirent dirent;

    fat_read(fs,((struct fat_fs_res*)fs->root_res)->ext_data+i*sizeof(struct fat_dirent),sizeof(struct fat_dirent),&dirent);

    if (dirent.filename[0]==0x05) dirent.filename[0] = 0xE5;
    else if (dirent.filename[0]==0xE5) continue;
    else if (dirent.filename[0]==0x00) break;

    struct fat_fs_res *res = fat_dirent_load((struct fat_fs_res*)fs->root_res,&dirent);
    if (res!=NULL) cdi_list_push(dirlist,res);
  }

  return dirlist;
}

cdi_list_t fat_dir_load(struct fat_fs_res *res) {
  debug("fat_dir_load(0x%x(%s))\n",res,res->res.name);
  struct fat_fs_filesystem *fat_fs = (struct fat_fs_filesystem*)res->fs->opaque;

  if ((struct cdi_fs_res*)res==res->fs->root_res && (fat_fs->type==FAT12 || fat_fs->type==FAT16)) return fat12_16_rootdir_load(res->fs);
  else {
    cdi_list_t dirlist = cdi_list_create();
    size_t i;

    for (i=0;;i++) {
      struct fat_dirent dirent;

      fat_cluster_read(res->fs,res->clusters,i*sizeof(struct fat_dirent),sizeof(struct fat_dirent),&dirent);

      if (dirent.filename[0]==0x05) dirent.filename[0] = 0xE5;
      else if (dirent.filename[0]==0xE5) continue;
      else if (dirent.filename[0]==0x00) break;

      struct fat_fs_res *child = fat_dirent_load(res,&dirent);
      if (child!=NULL) cdi_list_push(dirlist,child);
    }

    return dirlist;
  }
}

cdi_list_t fat_fs_dir_list(struct cdi_fs_stream *stream) {
  struct fat_fs_res *res = (struct fat_fs_res*)stream->res;
  return res->res.children;
}
