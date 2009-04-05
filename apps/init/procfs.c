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
#include <sys/stat.h>
#include <fuse.h>
#include <llist.h>
#include <stdlib.h>
#include <path.h>
#include <proc.h>
#include <stdio.h>

#include "init.h"

#define PROCFS_NAME       "procfs"
#define PROCFS_MOUNTPOINT "/proc"

typedef struct procfs_path_S procfs_path_t;

typedef struct {
  pid_t pid;
  char *exe;
  char *name;
  uid_t uid;
  gid_t gid;
} procfs_proc_t;

typedef enum {
  PROCFS_FILE,
  PROCFS_DIR,
  PROCFS_SYMLINK
} procfs_ftype_t;

typedef struct {
  const char *name;
  procfs_ftype_t ftype;
  struct {
    int (*cb_read)(procfs_path_t *path,char *buf,size_t size,off_t offset);
    int (*cb_write)(procfs_path_t *path,const char *buf,size_t size,off_t offset);
  } file;
  struct {
    int (*cb_readdir)(procfs_path_t *path,void *buf,fuse_fill_dir_t filler,off_t offset);
  } dir;
  struct {
  } symlink;
} procfs_file_t;

struct procfs_path_S {
  pid_t pid;
  procfs_proc_t *proc;
  procfs_file_t *file;
  const char *subfile;
  int isdir;
};

static int procfs_loginuid_read(procfs_path_t *path,char *buf,size_t size,off_t offset);
static int procfs_mem_read(procfs_path_t *path,char *buf,size_t size,off_t offset);
static int procfs_mem_write(procfs_path_t *path,const char *buf,size_t size,off_t offset);
static procfs_file_t procfs_files[] = {
  { .name = "cwd", .ftype = PROCFS_SYMLINK },
  { .name = "exe", .ftype = PROCFS_SYMLINK },
  { .name = "loginuid", .ftype = PROCFS_FILE, .file = { .cb_read = procfs_loginuid_read } },
  { .name = "mem", .ftype = PROCFS_FILE, .file = { .cb_read = procfs_mem_read, .cb_write = procfs_mem_write } },
  { .name = NULL }
};

static llist_t procfs_processes;

static int procfs_loginuid_read(procfs_path_t *path,char *buf,size_t size,off_t offset) {
  return 0;
}

static int procfs_mem_read(procfs_path_t *path,char *buf,size_t size,off_t offset) {
  return 0;
}

static int procfs_mem_write(procfs_path_t *path,const char *buf,size_t size,off_t offset) {
  return 0;
}

static procfs_proc_t *procfs_proc_find(pid_t pid) {
  size_t i;
  procfs_proc_t *proc;
  for (i=0;(proc = llist_get(procfs_processes,i));i++) {
    if (proc->pid==pid) return proc;
  }
  return NULL;
}

int procfs_proc_new(pid_t pid,const char *name,const char *exe) {
  procfs_proc_t *proc = malloc(sizeof(procfs_proc_t));
  proc->pid = pid;
  proc->uid = getuidbypid(pid);
  proc->gid = getgidbypid(pid);
  if (name==NULL) proc->name = getname(pid);
  else proc->name = strdup(name);
  if (exe==NULL) proc->exe = NULL;
  else proc->exe = strdup(exe);
  llist_push(procfs_processes,proc);
  return 0;
}

int procfs_proc_update(pid_t pid,const char *name,const char *exe) {
  procfs_proc_t *proc = procfs_proc_find(pid);
  if (proc!=NULL) {
    if (name!=NULL) {
      free(proc->name);
      proc->name = strdup(name);
    }
    if (exe!=NULL) {
      free(proc->exe);
      proc->exe = strdup(exe);
    }
    return 0;
  }
  else return -1;
}

static void procfs_parse_path(const char *strpath,procfs_path_t *path) {
  path_t *tpath = path_parse(strpath);
  path_reject_dots(tpath);

  memset(path,0,sizeof(procfs_path_t));
  path->isdir = 1;
  if (tpath->num_parts>0) {
    path->pid = strtoul(tpath->parts[0],NULL,10);
    path->proc = procfs_proc_find(path->pid);
    //path->isdir = 1;
  }
  if (tpath->num_parts>1) {
    size_t i;
    for (i=0;procfs_files[i].name!=NULL;i++) {
      if (strcmp(tpath->parts[1],procfs_files[i].name)==0) {
        path->file = procfs_files+i;
        path->isdir = (path->file->ftype==PROCFS_DIR);
      }
    }
  }
  if (tpath->num_parts>2) {
    static char subfile_buf[256];
    strncpy(subfile_buf,tpath->parts[2],256);
    path->subfile = subfile_buf;
    path->isdir = 0;
  }

  path_destroy(tpath);
}

static int procfs_open(const char *strpath,struct fuse_file_info *fi) {
  procfs_path_t path;

  if (path.pid!=0 && path.proc!=NULL) return -ENOENT;
  else if (path.isdir) return -EISDIR;
  else return 0;
}

static int procfs_close(const char *path,struct fuse_file_info *fi) {
  return 0;
}

static int procfs_read(const char *strpath,char *buf,size_t count,off_t offset,struct fuse_file_info *fi) {
  procfs_path_t path;
  procfs_parse_path(strpath,&path);

  if (path.pid!=0 && path.proc!=NULL) return -ENOENT;
  else if (path.isdir) return -EISDIR;
  else {
    if (path.file->file.cb_read!=NULL) return path.file->file.cb_read(&path,buf,count,offset);
    else return -ENOSYS;
  }
}

static int procfs_write(const char *strpath,const char *buf,size_t count,off_t offset,struct fuse_file_info *fi) {
  procfs_path_t path;
  procfs_parse_path(strpath,&path);

  if (path.pid!=0 && path.proc!=NULL) return -ENOENT;
  else if (path.isdir) return -EISDIR;
  else {
    if (path.file->file.cb_write!=NULL) return path.file->file.cb_write(&path,buf,count,offset);
    else return -ENOSYS;
  }
}

static int procfs_readdir(const char *strpath,void *buf,fuse_fill_dir_t filler,off_t offset,struct fuse_file_info *fi) {
  procfs_path_t path;
  procfs_parse_path(strpath,&path);

  if (!path.isdir) return -ENOTDIR;
  else if (path.pid==0) {
    // list all processes
    size_t i;
    procfs_proc_t *proc;
    struct stat stbuf = {
      /// @todo correct permissions
      .st_mode = S_IFDIR|0777,
    };
    for (i=0;(proc = llist_get(procfs_processes,i));i++) {
      char str_pid[16];
      snprintf(str_pid,16,"%d",proc->pid);
      stbuf.st_uid = proc->uid;
      stbuf.st_gid = proc->gid;
      filler(buf,str_pid,&stbuf,0);
    }
    return 0;
  }
  else if (path.proc!=NULL) return -ENOENT;
  else {
    if (path.file->dir.cb_readdir!=NULL) return path.file->dir.cb_readdir(&path,buf,filler,offset);
    else return -ENOSYS;
  }
}

static int procfs_getattr(const char *strpath,struct stat *stbuf) {
  procfs_path_t path;
  procfs_parse_path(strpath,&path);

  memset(stbuf,0,sizeof(struct stat));

  if (path.file!=NULL) {
    if (path.file->ftype==PROCFS_DIR) stbuf->st_mode = S_IFDIR;
    else if (path.file->ftype==PROCFS_SYMLINK) stbuf->st_mode = S_IFLNK;
    else stbuf->st_mode = S_IFREG;
  }
  else stbuf->st_mode = S_IFDIR;

  /// @todo correct permissions
  stbuf->st_mode |= 0777;

  return 0;
}

int procfs_init(pid_t *grub_modules) {
  size_t i;
  pid_t pid;

  procfs_processes = llist_create();

  // put init itself in process list
  procfs_proc_new(1,"init",NULL);

  // put GRUB modules in process list
  for (i=0;(pid = grub_modules[i])!=0;i++) {
    procfs_proc_new(pid,NULL,NULL);
  }

  return 0;
}

int procfs_run(void) {
  // start FUSE
  struct fuse_operations devfs_oper = {
    .open = procfs_open,
    .release = procfs_close,
    .read = procfs_read,
    .write = procfs_write,
    .readdir = procfs_readdir,
    .getattr = procfs_getattr
  };

  // fake argc/argv
  int fake_argc = 2;
  char *fake_argv[2] = { PROCFS_NAME,PROCFS_MOUNTPOINT };
  fuse_main(fake_argc,fake_argv,&devfs_oper,NULL);

  return 0;
}
