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

static struct fat_fs_res *fat_dirent_load(struct fat_fs_res *parent,struct fat_dirent *dirent) {
  char *name = parse_name((char*)dirent->filename,(char*)dirent->filename_ext);

  if (dirent->attr.hidden && dirent->attr.system && dirent->attr.volume && dirent->attr.readonly) {
    /// @todo Long names
  }
  else if (!dirent->attr.volume) {
    if (strcmp(name,".")!=0 && strcmp(name,"..")!=0) {
      cdi_fs_res_class_t class = dirent->attr.dir?CDI_FS_CLASS_DIR:CDI_FS_CLASS_FILE;
      cdi_fs_res_type_t type = 0;
      struct fat_fs_res *res = fat_fs_res_create(name,parent,class,type);
      res->readonly = dirent->attr.readonly;
      res->system = dirent->attr.system;
      res->filesize = dirent->file_size;
      res->clusters = fat_clchain_create(parent->fs,dirent->first_cluster);
      free(name);
      return res;
    }
  }

  free(name);
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
  struct fat_fs_res *child;
  static cdi_list_t dirlist = NULL;
  size_t i;

  if (dirlist==NULL) dirlist = cdi_list_create();
  else {
    while ((child = cdi_list_pop(dirlist)));
  }

  for (i=0;(child = cdi_list_get(res->res.children,i));i++) {
    if (!child->system) cdi_list_push(dirlist,child);
  }

  return dirlist;
}
