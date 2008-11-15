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

#include <sys/types.h>
#include <rpc.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <llist.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <path.h>
#include <limits.h>

struct fslist_item {
  int fsid;
  char *name;
  char *mountpoint;
  path_t *mp_path;
  pid_t pid;
};

llist_t fslist;
struct fslist_item *root_fs = NULL;
int nextfsid;

/**
 * Gets a new unique FSID
 *  @return New FSID
 */
/*int getnew_fsid() {
  int dub,i,fsid;
  struct fslist_item *fs;

  do {
    fsid = rand();
    dub = 0;
    for (i=0;(fs = llist_get(fslist,i));i++) {
      if (fs->fsid==fsid) {
        dub = 1;
        break;
      }
    }
  } while (dub);
  return fsid;
}*/
#define getnew_fsid() (nextfsid++)

/**
 * Gets ListID by FSID
 *  @param fsid FSID
 *  @return ListID
 */
int getlid_fsid(int fsid) {
  int i;
  struct fslist_item *fs;
  for (i=0;(fs = llist_get(fslist,i));i++) {
    if (fs->fsid==fsid) return i;
  }
  return -1;
}

/**
 * Registers a new FS
 *  @param name Name of new FS
 *  @param mountpoint of new FS
 *  @return New FSID
 */
int vfs_regfs(char *name,char *mountpoint) {
  struct fslist_item *new = malloc(sizeof(struct fslist_item));
  new->fsid = getnew_fsid();
  if (mountpoint[strlen(mountpoint)-1]=='/') mountpoint[strlen(mountpoint)-1] = 0;
  new->name = strdup(name);
  new->mountpoint = strdup(mountpoint);
  new->mp_path = path_parse(new->mountpoint);
  if (new->mountpoint[strlen(new->mountpoint)-1]=='/' && strlen(new->mountpoint)>1) new->mountpoint[strlen(new->mountpoint)-1] = 0;
  new->pid = rpc_curpid;
  llist_push(fslist,new);
  fprintf(stderr,"vfs: New FS: %s %s (id=%d; pid=%d%s)\n",name,mountpoint,new->fsid,new->pid,mountpoint[0]==0?", rootfs":"");
  if (mountpoint[0]==0) root_fs = new;
  return new->fsid;
}

/**
 * Removes a FS
 *  @param fsid FSID of FS to remove
 *  @return 0 = success; -1 = failure
 */
int vfs_remfs(int fsid) {
  struct fslist_item *rem = llist_get(fslist,getlid_fsid(fsid));
  if (rem->pid==rpc_curpid) {
    llist_remove(fslist,getlid_fsid(fsid));
    fprintf(stderr,"vfs: Removed FS: %s (%d)\n",rem->name,rem->fsid);
    path_destroy(rem->mp_path);
    free(rem->name);
    free(rem->mountpoint);
    free(rem);
    return 0;
  }
  return -1;
}

/**
 * Gets FSID by Path
 *  @param file Path to file
 */
int vfs_getfsid(char *file,int parent) {
  size_t i,curcmp;
  size_t maxcmp = 0;
  struct fslist_item *fs;
  struct fslist_item *maxfs = root_fs;
  path_t *path = path_parse(file);

  if (parent) path_parent(path);

  for (i=0;(fs = llist_get(fslist,i));i++) {
    curcmp = path_compare(path,fs->mp_path);
    if (curcmp>maxcmp) {
      maxcmp = curcmp;
      maxfs = fs;
    }
  }

  if (maxfs!=NULL) {
    fprintf(stderr,"vfs: %s is on FS %s (%d)\n",file,maxfs->name,maxfs->fsid);
    memmove(file,file+strlen(maxfs->mountpoint),strlen(file)-strlen(maxfs->mountpoint)+1);
    return maxfs->fsid;
  }
  else {
    fprintf(stderr,"vfs: Could not find FS for %s\n",file);
    return -ENOENT;
  }
}

/**
 * Gets PID of a FS
 *  @param fsid FSID
 *  @return PID
 */
pid_t vfs_getpid(int fsid) {
  struct fslist_item *fs = llist_get(fslist,getlid_fsid(fsid));
  if (fs!=NULL) return fs->pid;
  else return -EINVAL;
}

/**
 * Deinitializes VFS
 */
void deinit() {
  struct fslist_item *fs;
  int i;
  for (i=0;(fs = llist_get(fslist,i));i++) kill(fs->pid,SIGTERM);
}

/**
 * Initializes VFS
 *  @param argc Number of arguments
 *  @param argv Values og arguments
 *  @return Return value
 */
int main(int argc,char *argv[]) {
  atexit(deinit);
  fslist = llist_create();
  nextfsid = 1;
  rpc_func(vfs_regfs,"$$",NAME_MAX+PATH_MAX);
  rpc_func(vfs_remfs,"i",sizeof(int));
  rpc_func(vfs_getfsid,"$i",PATH_MAX+sizeof(int));
  rpc_func(vfs_getpid,"i",sizeof(int));
  rpc_mainloop(-1);

  return 0;
}
