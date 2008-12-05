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

#include <fuse.h>
#include <fuse_lowlevel.h>
#include <fuse_common.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include <llist.h>
#include <rpc.h>
#include <errno.h>
#include <proc.h>
#include <sys/shm.h>

static llist_t fslist;
static int fuse_inited = 0;
uint64_t fs_file_lastfh;

struct fuse_readdir {
  const char *name;
  off_t next;
  const struct stat *stbuf;
};

static int fuse_fill_dir(void *vdbuf,const char *name,const struct stat *stbuf,off_t off) {
  llist_t dir = (llist_t)vdbuf;
  struct fuse_readdir *new = malloc(sizeof(struct fuse_readdir));
  new->name = name;
  new->next = off;
  new->stbuf = stbuf;
  llist_insert(dir,llist_size(dir),new);
  return 0;
}

#define fs_func_create(func,synopsis,bufsize) _fs_func_create(__STRING(func),func,synopsis,bufsize,pid)
static int _fs_func_create(char *name,void *func,char *synopsis,size_t bufsize,pid_t pid) {
  char *rpc;
  asprintf(&rpc,"%s_%x",name,pid);
  int ret = rpc_func_create(rpc,func,synopsis,bufsize);
  free(rpc);
  return ret;
}

static struct fuse *fuse_find(int fsid) {
  size_t i;
  struct fuse *fuse;
  for (i=0;(fuse = llist_get(fslist,i));i++) {
    if (fuse->chan->fsid==fsid) return fuse;
  }
  return NULL;
}

static struct fuse_file *fuse_file_find(struct fuse *fuse,int fh) {
  size_t i;
  struct fuse_file *file;
  for (i=0;(file = llist_get(fuse->files,i));i++) {
    /// @todo Must not happen
    if (file->file_info==NULL) continue;
    if (file->file_info->fh==fh) return file;
  }
  return NULL;
}

void fuse_set_context(struct fuse *fuse) {
  fuse->context->pid = rpc_curpid;
  fuse->context->uid = getuidbypid(rpc_curpid);
  fuse->context->gid = getgidbypid(rpc_curpid);
  fuse_current_context = fuse->context;
}

// Wrapper functions

static int fs_open(int fsid,int oflag,int shmid) {
  fprintf(stderr,"fuse: fs_open(%d,%d,%d)\n",fsid,oflag,shmid);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    if (fuse->fs->op->open!=NULL) {
      struct fuse_file *file = malloc(sizeof(struct fuse_file));
      file->shmid = shmid;
      file->shmbuf = shmat(shmid,NULL,0);
      file->filename = strdup(file->shmbuf);
      file->off = 0;
      file->type = REG;
      struct fuse_file_info *file_info = malloc(sizeof(struct fuse_file_info));
      file_info->flags = oflag;
      file_info->fh = fs_file_lastfh++;
      file->file_info = file_info;

      int ret = fuse->fs->op->open(file->filename,file_info);
      if (ret>=0) {
        llist_push(fuse->files,file);
        return file_info->fh;
      }
      else {
        shmdt(file->shmbuf);
        free(file->file_info);
        free(file);
        return ret;
      }
    }
    else return -ENOSYS;
  }
  return -EBADF;
}

static int fs_close(int fsid,int fh) {
  fprintf(stderr,"fuse: fs_close(%d,%d)\n",fsid,fh);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    struct fuse_file *file = fuse_file_find(fuse,fh);
    if (file!=NULL) {
      if (fuse->fs->op->release!=NULL) {
        int ret = fuse->fs->op->release(file->filename,file->file_info);
        if (ret>=0) {
          shmdt(file->shmbuf);
          free(file->file_info);
          free(file);
          return 0;
        }
        else return ret;
      }
      else return -ENOSYS;
    }
    else return -EBADF;
  }
  else return -EBADF;
}

static ssize_t fs_read(int fsid,int fh,size_t count) {
  fprintf(stderr,"fuse: fs_read(%d,%d,%d)\n",fsid,fh,count);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    struct fuse_file *file = fuse_file_find(fuse,fh);
    if (file!=NULL) {
      if (fuse->fs->op->read!=NULL) {
        ssize_t res = fuse->fs->op->read(file->filename,file->shmbuf,count,file->off,file->file_info);
        if (res>0) file->off += res;
        return res;
      }
      else return -ENOSYS;
    }
    else return -EBADF;
  }
  else return -EBADF;
}

static ssize_t fs_write(int fsid,int fh,size_t count) {
  fprintf(stderr,"fuse: fs_write(%d,%d,%d)\n",fsid,fh,count);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    struct fuse_file *file = fuse_file_find(fuse,fh);
    if (file!=NULL) {
      if (fuse->fs->op->write!=NULL) {
        ssize_t res = fuse->fs->op->write(file->filename,file->shmbuf,count,file->off,file->file_info);
        if (res>0) file->off += res;
        return res;
      }
      else return -ENOSYS;
    }
    else return -EBADF;
  }
  else return -EBADF;
}

static ssize_t fs_readlink(int fsid,int shmid,size_t bufsize) {
  fprintf(stderr,"fuse: fs_readlink(%d,%d,%d)\n",fsid,shmid,bufsize);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    if (fuse->fs->op->readlink!=NULL) {
      void *buf = shmat(shmid,NULL,0);
      if (buf!=NULL) {
        char *filename = strdup(buf);
        int ret = fuse->fs->op->readlink(filename,buf,bufsize);
        free(filename);
        shmdt(buf);
        return ret;
      }
      else return -errno;
    }
    else return -ENOSYS;
  }
  else return -EBADF;
}

static int fs_symlink(int fsid,char *src,char *dest) {
  fprintf(stderr,"fuse: fs_symlink(%d,%s,%s)\n",fsid,src,dest);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    if (fuse->fs->op->symlink!=NULL) return fuse->fs->op->symlink(src,dest);
    else return -ENOSYS;
  }
  else return -EBADF;
}

static int fs_link(int fsid,char *src,char *dest) {
  fprintf(stderr,"fuse: fs_link(%d,%s,%s)\n",fsid,src,dest);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    if (fuse->fs->op->link!=NULL) return fuse->fs->op->link(src,dest);
    else return -ENOSYS;
  }
  else return -EBADF;
}


static off_t fs_seek(int fsid,int fh,off_t off,int whence) {
  fprintf(stderr,"fuse: fs_seek(%d,%d,%d,%d)\n",fsid,fh,off,whence);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    struct fuse_file *file = fuse_file_find(fuse,fh);
    if (file!=NULL) {
      if (whence==SEEK_SET) file->off = off;
      else if (whence==SEEK_CUR) file->off += off;
      else if (whence==SEEK_END) file->off = 0;
      return file->off;
    }
    else return -EBADF;
  }
  else return -EBADF;
}

static int fs_fstat(int fsid,int fh) {
  fprintf(stderr,"fuse: fs_fstat(%d,%d)\n",fsid,fh);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    struct fuse_file *file = fuse_file_find(fuse,fh);
    if (file!=NULL) {
      if (fuse->fs->op->getattr!=NULL) return fuse->fs->op->getattr(file->filename,file->shmbuf);
      else if (fuse->fs->op->fgetattr!=NULL) return fuse->fs->op->fgetattr(file->filename,file->shmbuf,file->file_info);
      else return -ENOSYS;
    }
    else return -EBADF;
  }
  else return -EBADF;
}

static int fs_unlink(int fsid,char *path) {
  fprintf(stderr,"fuse: fs_unlink(%d,%s)\n",fsid,path);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    if (fuse->fs->op->unlink!=NULL) return fuse->fs->op->unlink(path);
    else return -ENOSYS;
  }
  else return -EBADF;
}

static int fs_rmdir(int fsid,char *path) {
  fprintf(stderr,"fuse: fs_rmdir(%d,%s)\n",fsid,path);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    if (fuse->fs->op->rmdir!=NULL) return fuse->fs->op->rmdir(path);
    else return -ENOSYS;
  }
  else return -EBADF;
}

static int fs_rename(int fsid,char *old,char *new) {
  fprintf(stderr,"fuse: fs_rename(%d,%s,%s)\n",fsid,old,new);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    if (fuse->fs->op->rename!=NULL) return fuse->fs->op->rename(old,new);
    else return -ENOSYS;
  }
  else return -EBADF;
}

static int fs_ftruncate(int fsid,int fh,off_t length) {
  fprintf(stderr,"fuse: fs_truncate(%d,%d,%d)\n",fsid,fh,length);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    struct fuse_file *file = fuse_file_find(fuse,fh);
    if (file!=NULL) {
      if (fuse->fs->op->truncate!=NULL) return fuse->fs->op->truncate(file->filename,length);
      else if (fuse->fs->op->ftruncate!=NULL) return fuse->fs->op->ftruncate(file->filename,length,file->file_info);
      else return -ENOSYS;
    }
    else return -EBADF;
  }
  else return -EBADF;
}

static int fs_opendir(int fsid,int shmid) {
  fprintf(stderr,"fuse: fs_opendir(%d,%d)\n",fsid,shmid);
  int ret;
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    struct fuse_file *file = malloc(sizeof(struct fuse_file));
    file->shmid = shmid;
    file->shmbuf = shmat(shmid,NULL,0);
    file->filename = strdup(file->shmbuf);
    file->off = 0;
    file->type = DIR;
    file->dir = llist_create();
    struct fuse_file_info *file_info = malloc(sizeof(struct fuse_file_info));
    file_info->fh = fs_file_lastfh++;
    file->file_info = file_info;

    if (fuse->fs->op->opendir!=NULL) ret = fuse->fs->op->opendir(file->filename,file_info);
    else ret = 0;

    if (ret==0) {
      llist_push(fuse->files,file);
      return file_info->fh;
    }
    else {
      shmdt(file->shmbuf);
      free(file->file_info);
      free(file);
      return ret;
    }
  }
  return -EBADF;
}

static int fs_readdir(int fsid,int dh) {
  fprintf(stderr,"fuse: fs_readdir(%d,%d)\n",fsid,dh);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    struct fuse_file *file = fuse_file_find(fuse,dh);
    if (file!=NULL) {
      int ret = 0;
      if (llist_size(file->dir)==0) {
        if (fuse->fs->op->readdir!=NULL) {
          struct fuse_readdir *entry;
          while ((entry = llist_pop(file->dir))) free(entry);
          ret = fuse->fs->op->readdir(file->filename,file->dir,fuse_fill_dir,file->off,file->file_info);
        }
        else return -ENOSYS;
      }
      if (ret>=0) {
        if (file->off>=llist_size(file->dir)) ret = -ENOENT;
        else {
          struct fuse_readdir *entry = llist_get(file->dir,file->off);
          strcpy(file->shmbuf,entry->name);
          file->off++;
        }
      }
      return ret;
    }
    else return -EBADF;
  }
  else return -EBADF;
}

static int fs_closedir(int fsid,int dh) {
  fprintf(stderr,"fuse: fs_closedir(%d,%d)\n",fsid,dh);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    struct fuse_file *file = fuse_file_find(fuse,dh);
    if (file!=NULL) {
      int ret;
      if (fuse->fs->op->releasedir!=NULL) ret = fuse->fs->op->releasedir(file->filename,file->file_info);
      else ret = 0;
      if (ret>=0) {
        struct fuse_readdir *entry;
        while ((entry = llist_pop(file->dir))) free(entry);
        llist_destroy(file->dir);
        shmdt(file->shmbuf);
        free(file->file_info);
        free(file);
        return 0;
      }
      else return ret;
    }
    else return -EBADF;
  }
  else return -EBADF;
}

static off_t fs_seekdir(int fsid,int dh,off_t off) {
  fprintf(stderr,"fuse: fs_seekdir(%d,%d,%d)\n",fsid,dh,off);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    struct fuse_file *file = fuse_file_find(fuse,dh);
    if (file!=NULL) {
      if (off>0) file->off = off;
      return file->off;
    }
    else return -EBADF;
  }
  else return -EBADF;
}

static int fs_statvfs(int fsid,int shmid) {
  fprintf(stderr,"fuse: fs_statvfs(%d,%d)\n",fsid,shmid);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    if (fuse->fs->op->statfs!=NULL) {
      void *shmbuf = shmat(shmid,NULL,0);
      if (shmbuf!=NULL) {
        char *path = strdup(shmbuf);
        int ret = fuse->fs->op->statfs(path,shmbuf);
        free(path);
        shmdt(shmbuf);
        return ret;
      }
      else return -errno;
    }
    else return -ENOSYS;
  }
  else return -EBADF;
}

static int fs_mknod(int fsid,char *path,mode_t mode,dev_t dev) {
  fprintf(stderr,"fuse: fs_mknod(%d,%s,%o,%d)\n",fsid,path,mode,dev);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    if (S_ISDIR(mode)) {
      if (fuse->fs->op->mkdir!=NULL) return fuse->fs->op->mknod(path,mode,dev);
      else return -ENOSYS;
    }
    else {
      if (fuse->fs->op->mknod!=NULL) return fuse->fs->op->mknod(path,mode,dev);
      else return -ENOSYS;
    }
  }
  else return -EBADF;
}

static int fs_dup(int fsid,int fh,int shmid) {
  fprintf(stderr,"fuse: fs_dup(%d,%d,%d)\n",fsid,fh,shmid);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    struct fuse_file *file = fuse_file_find(fuse,fh);
    if (file!=NULL) {
      struct fuse_file *new = malloc(sizeof(struct fuse_file));
      new->file_info = malloc(sizeof(struct fuse_file_info));
      memcpy(new->file_info,file->file_info,sizeof(struct fuse_file_info));
      new->file_info->fh = fs_file_lastfh++;
      new->shmid = shmid;
      new->shmbuf = shmat(shmid,NULL,0);
      new->filename = strdup(file->filename);
      new->off = file->off;
      new->type = file->type;
      if (file->dir==NULL) new->dir = NULL;
      else new->dir = llist_create();
      return new->file_info->fh;
    }
    else return -EBADF;
  }
  else return -EBADF;
}

static int fs_chown(int fsid,char *path,uid_t uid,gid_t gid) {
  fprintf(stderr,"fuse: fs_chown(%d,%s,%d,%d)\n",fsid,path,uid,gid);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    if (fuse->fs->op->chown!=NULL) return fuse->fs->op->chown(path,uid,gid);
    else return -ENOSYS;
  }
  else return -EBADF;
}

static int fs_chmod(int fsid,char *path,mode_t mode) {
  fprintf(stderr,"fuse: fs_chmod(%d,%s,%o)\n",fsid,path,mode);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    if (fuse->fs->op->chmod!=NULL) return fuse->fs->op->chmod(path,mode);
    else return -ENOSYS;
  }
  else return -EBADF;
}

static int fs_access(int fsid,char *path,int amode) {
  fprintf(stderr,"fuse: fs_access(%d,%s,%d)\n",fsid,path,amode);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    if (fuse->fs->op->access!=NULL) return fuse->fs->op->access(path,amode);
    else return -ENOSYS;
  }
  else return -EBADF;
}

static int fs_utime(int fsid,int shmid) {
  fprintf(stderr,"fuse: fs_utime(%d,%d)\n",fsid,shmid);
  struct fuse *fuse = fuse_find(fsid);
  if (fuse!=NULL) {
    if (fuse->fs->op->utime!=NULL) {
      void *shmbuf = shmat(shmid,NULL,0);
      if (shmbuf!=NULL) {
        char *path = shmbuf;
        struct utimbuf *times = shmbuf+strlen(shmbuf)+1;
        int ret = fuse->fs->op->utime(path,times);
        shmdt(shmbuf);
        return ret;
      }
      else return -errno;
    }
    else return -ENOSYS;
  }
  else return -EBADF;
}

static void fuse_init() {
  pid_t pid = getpid();
  fslist = llist_create();
  fs_file_lastfh = 0;
  fs_func_create(fs_open,"iii",sizeof(int)*3);
  fs_func_create(fs_close,"ii",sizeof(int)*2);
  fs_func_create(fs_read,"iii",sizeof(int)*3);
  fs_func_create(fs_write,"iii",sizeof(int)*3);
  fs_func_create(fs_seek,"iiii",sizeof(int)*4);
  fs_func_create(fs_fstat,"ii",sizeof(int)*2);
  fs_func_create(fs_unlink,"i$",sizeof(int)+PATH_MAX);
  fs_func_create(fs_rmdir,"i$",sizeof(int)+PATH_MAX);
  fs_func_create(fs_rename,"i$$",sizeof(int)+PATH_MAX*2);
  fs_func_create(fs_ftruncate,"iii",sizeof(int)*3);
  fs_func_create(fs_opendir,"ii",sizeof(int)*2);
  fs_func_create(fs_readdir,"ii",sizeof(int)*2);
  fs_func_create(fs_closedir,"ii",sizeof(int)*2);
  fs_func_create(fs_seekdir,"iii",sizeof(int)*3);
  fs_func_create(fs_statvfs,"ii",sizeof(int));
  fs_func_create(fs_readlink,"iii",sizeof(int)*3);
  fs_func_create(fs_symlink,"i$$",sizeof(int)+PATH_MAX*2);
  fs_func_create(fs_link,"i$$",sizeof(int)+PATH_MAX*2);
  fs_func_create(fs_mknod,"i$ii",sizeof(int)*3+PATH_MAX);
  fs_func_create(fs_dup,"iii",sizeof(int)*3);
  fs_func_create(fs_chown,"i$ii",sizeof(int)*3+PATH_MAX);
  fs_func_create(fs_chmod,"i$i",sizeof(int)*2+PATH_MAX);
  fs_func_create(fs_access,"i$i",sizeof(int)*2+PATH_MAX);
  fs_func_create(fs_utime,"ii",sizeof(int)*2);
  fuse_inited = 1;
}

void _fuse_add(struct fuse *fuse) {
  if (!fuse_inited) fuse_init();
  llist_push(fslist,fuse);
  if (fuse->fs->op->init!=NULL) fuse->context->private_data = fuse->fs->op->init(fuse->conn);
}
