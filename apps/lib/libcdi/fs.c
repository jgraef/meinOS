#include <cdi.h>
#include <cdi/lists.h>
#include <llist.h> // for llist_find()
#include <cdi/fs.h>
#include <errno.h>
#include <sys/shm.h>
#include <rpc.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/cdefs.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <utime.h>
#include <unistd.h>
#include <path.h>

#define CDI_FS_STREAM(filesystem,resource) { .fs = (filesystem), .res = (resource), .error = 0 }

struct cdi_fs_file {
  struct cdi_fs_res *res;
  int fh;
  enum {
    FILE_REG,
    FILE_DIR,
  } type;
  uint64_t pos;
  void *shmbuf;
  cdi_list_t dirlist;
};

/**
 * Mounts FS driver
 *  @param driver FS driver
 *  @param fs_name Filesystem name
 *  @param mountpoint Mountpoint
 *  @param readonly If FS should be mounted readonly
 *  @return Success?
 */
static int cdi_fs_mount(struct cdi_fs_driver *driver,const char *fs_name,const char *mountpoint,const char *dev,int readonly) {
  fprintf(stderr,"cdi_fs_mount(0x%x,%s,%s,%s,%d)\n",driver,fs_name,mountpoint,dev,readonly);
  int fsid = rpc_call("vfs_regfs",0,fs_name,mountpoint);
  if (fsid!=-1) {
    struct cdi_fs_filesystem *filesystem = malloc(sizeof(struct cdi_fs_filesystem));
    filesystem->driver = driver;
    filesystem->error = 0;
    filesystem->read_only = readonly;
    filesystem->fsid = fsid;
    filesystem->last_fh = 0;
    filesystem->files = cdi_list_create();
    filesystem->mountpoint = strdup(mountpoint);
    if (dev!=NULL) filesystem->data_fh = open(dev,O_RDWR);
    else filesystem->data_fh = 0;

    if (filesystem->data_fh!=-1) {
      filesystem->data_dev = dev;
      if (driver->fs_init(filesystem)) {
        cdi_list_push(driver->filesystems,filesystem);
        cdi_list_push(cdi_filesystems,filesystem);
        return 0;
      }
    }
    else {
      cdi_list_destroy(filesystem->files);
      close(filesystem->data_fh);
      free((void*)(filesystem->mountpoint));
      free(filesystem);
    }
  }
  return -EIO;
}

static int cdi_fs_mount_rpc(char *fs_name,const char *mountpoint,const char *dev,int readonly) {
  fprintf(stderr,"cdi_fs_mount_rpc(%s,%s,%s,%d)\n",fs_name,mountpoint,dev,readonly);
  size_t i;
  struct cdi_fs_driver *driver;
  if (!*dev) dev = NULL;
  for (i=0;(driver = cdi_list_get(cdi_drivers,i));i++) {
    if (strcmp(driver->drv.name,fs_name)==0) return cdi_fs_mount(driver,fs_name,mountpoint,dev,readonly);
  }
  return -EINVAL;
}

static int cdi_fs_unmount(struct cdi_fs_filesystem *filesystem) {
  fprintf(stderr,"cdi_fs_unmount(0x%x)\n",filesystem);
  rpc_call("vfs_unregfs",0,filesystem->fsid);
  return filesystem->driver->fs_destroy(filesystem)?0:-1;
}

static int cdi_fs_unmount_rpc(char *fs_name,char *mountpoint) {
  fprintf(stderr,"cdi_fs_unmount_rpc(%s)\n",fs_name);
  size_t i,j;
  struct cdi_fs_driver *driver;
  struct cdi_fs_filesystem *filesystem;
  for (i=0;(driver = cdi_list_get(cdi_drivers,i));i++) {
    if (strcmp(driver->drv.name,fs_name)==0) {
      for (j=0;(filesystem = cdi_list_get(driver->filesystems,j));j++) {
        if (strcmp(filesystem->mountpoint,mountpoint)==0) return cdi_fs_unmount(filesystem);
      }
    }
  }
  return -1;
}

#define cdi_fs_func(func,synopsis,bufsize) _cdi_fs_func(__STRING(func),func,synopsis,bufsize,pid)
static int _cdi_fs_func(char *name,void *func,char *synopsis,size_t bufsize,pid_t pid) {
  char *rpc;
  asprintf(&rpc,"%s_%x",name,pid);
  int ret = rpc_func_create(rpc,func,synopsis,bufsize);
  free(rpc);
  return ret;
}

/**
 * Find a filesystem
 *  @param fsid FSID
 *  @return Filesystem
 */
static struct cdi_fs_filesystem *cdi_fs_find(int fsid) {
  fprintf(stderr,"cdi_fs_find(%d)\n",fsid);
  size_t i;
  struct cdi_fs_filesystem *filesystem;
  for (i=0;(filesystem = cdi_list_get(cdi_filesystems,i));i++) {
    if (filesystem->fsid==fsid) return filesystem;
  }
  return NULL;
}

static struct cdi_fs_file *cdi_fs_file_find(struct cdi_fs_filesystem *filesystem,int fh) {
  fprintf(stderr,"cdi_fs_file_find(0x%x,%d)\n",filesystem,fh);
  size_t i;
  struct cdi_fs_file *file;
  for (i=0;(file = cdi_list_get(filesystem->files,i));i++) {
    if (file->fh==fh) return file;
  }
  return NULL;
}

/**
 * Converts a CDI FS error number into a Clib errno
 *  @param error CDI FS error number
 *  @return Clib errno
 */
static int cdi_fs_error2errno(cdi_fs_error_t error) {
  if (error==CDI_FS_ERROR_NONE) return 0;
  if (error==CDI_FS_ERROR_IO) return EIO;
  if (error==CDI_FS_ERROR_ONS) return ENOSYS;
  if (error==CDI_FS_ERROR_RNF) return ENOENT;
  if (error==CDI_FS_ERROR_EOF) return EOF;
  if (error==CDI_FS_ERROR_RO) return EROFS;
  if (error==CDI_FS_ERROR_INTERNAL) return -1;
  if (error==CDI_FS_ERROR_NOT_IMPLEMENTED) return ENOTSUP;
  return -1;
}

/**
 * Loads a resource
 *  @param res Resource
 *  @param stream Stream
 */
static inline int cdi_fs_loadres(struct cdi_fs_stream *stream) {
  fprintf(stderr,"cdi_fs_loadres(0x%x)\n",stream);
  return stream->res->loaded?0:(stream->res->res->load(stream)?0:-1);
}

/**
 * Unloads a resource
 *  @param res Resource
 *  @param stream Stream
 */
static inline int cdi_fs_unloadres(struct cdi_fs_stream *stream) {
  fprintf(stderr,"cdi_fs_unloadres(0x%x)\n",stream);
  /// @todo Zum Verringern von IO, aber wieder normal machen!
  return 1; //stream->res->loaded?(stream->res->res->load(stream)?0:-1):0;
}

/**
 * Gets parent CDI FS resource by path
 *  @param filesystem CDI filesystem
 *  @param path Path to resource
 *  @param filename Reference for filename (will point into path)
 *  @return Resource or NULL
 */
static struct cdi_fs_res *cdi_fs_parentres(struct cdi_fs_filesystem *filesystem,char *path_str,char **filename) {
  fprintf(stderr,"cdi_fs_parentres(0x%x,%s)\n",filesystem,path_str);
  struct cdi_fs_res *res = filesystem->root_res;
  struct cdi_fs_res *child = res;
  size_t k;
  ssize_t i,j;
  struct cdi_fs_stream stream = CDI_FS_STREAM(filesystem,res);

  path_t *path = path_parse(path_str);
  for (j=path->num_parts-1;j>=0 && path->parts[j][0]==0;j--);
  for (i=0;i<j;i++) {
    if (path->parts[i][0]==0) continue;
    cdi_fs_loadres(&stream);
    for (k=0;(child = cdi_list_get(res->children,k));k++) {
      if (strcmp(child->name,path->parts[i])==0) {
        res = child;
        stream.res = child;
        break;
      }
    }
    if (child==NULL) break;
  }

  *filename = path_str;
  if (j<0) strcpy(*filename,"/");
  else strcpy(path_str,path->parts[j]);

  return child;
}

/**
 * Gets CDI FS resource by path
 *  @param filesystem CDI filesystem
 *  @param path Path to resource
 *  @return Resource or NULL
 */
static struct cdi_fs_res *cdi_fs_path2res(struct cdi_fs_filesystem *filesystem,char *path_str) {
  fprintf(stderr,"cdi_fs_path2res(0x%x,%s)\n",filesystem,path_str);
  struct cdi_fs_res *res = filesystem->root_res;
  struct cdi_fs_res *child = res;
  size_t i,k;
  struct cdi_fs_stream stream = CDI_FS_STREAM(filesystem,res);

  path_t *path = path_parse(path_str);
  for (i=0;i<path->num_parts;i++) {
    if (path->parts[i][0]==0) continue;
    cdi_fs_loadres(&stream);
    for (k=0;(child = cdi_list_get(res->children,k));k++) {
      if (strcmp(child->name,path->parts[i])==0) {
        res = child;
        stream.res = child;
        break;
      }
    }
    if (child==NULL) break;
  }

  return child;
}

static cdi_fs_res_class_t cdi_fs_mode2class(mode_t mode,cdi_fs_res_type_t *special) {
  if (S_ISREG(mode)) return CDI_FS_CLASS_FILE;
  if (S_ISDIR(mode)) return CDI_FS_CLASS_DIR;
  if (S_ISLNK(mode)) return CDI_FS_CLASS_LINK;
  if (S_ISCHR(mode)) {
    *special = CDI_FS_CHAR;
    return CDI_FS_CLASS_SPECIAL;
  }
  if (S_ISBLK(mode)) {
    *special = CDI_FS_BLOCK;
    return CDI_FS_CLASS_SPECIAL;
  }
  if (S_ISFIFO(mode)) {
    *special = CDI_FS_FIFO;
    return CDI_FS_CLASS_SPECIAL;
  }
  if (S_ISSOCK(mode)) {
    *special = CDI_FS_SOCKET;
    return CDI_FS_CLASS_SPECIAL;
  }
  return CDI_FS_CLASS_FILE;
}

static mode_t cdi_fs_class2mode(struct cdi_fs_res *res) {
  mode_t mode = 0;
  if (res->file!=NULL) mode |= S_IFREG;
  if (res->dir!=NULL) mode |= S_IFDIR;
  if (res->link!=NULL) mode |= S_IFLNK;
  if (res->special!=NULL) {
    if (res->type==CDI_FS_CHAR) mode |= S_IFCHR;
    else if (res->type==CDI_FS_BLOCK) mode |= S_IFBLK;
    else if (res->type==CDI_FS_FIFO) mode |= S_IFIFO;
    else if (res->type==CDI_FS_SOCKET) mode |= S_IFSOCK;
  }
  return mode;
}

static size_t cdi_fs_filesize(struct cdi_fs_filesystem *filesystem,struct cdi_fs_res *res) {
  if (res->res->meta_read!=NULL) {
    struct cdi_fs_stream stream = CDI_FS_STREAM(filesystem,res);
    return res->res->meta_read(&stream,CDI_FS_META_SIZE);
  }
  else return 0;
}

// Wrapper functions ////

static int fs_open(int fsid,int oflag,int shmid) {
  fprintf(stderr,"fs_open(%d,%d,%d)\n",fsid,oflag,shmid);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    void *shmbuf = shmat(shmid,NULL,0);
    if (shmbuf!=NULL) {
      struct cdi_fs_res *res = cdi_fs_path2res(filesystem,shmbuf);
      if (res!=NULL) {
        if (res->file!=NULL) {
          struct cdi_fs_stream stream = CDI_FS_STREAM(filesystem,res);
          if (cdi_fs_loadres(&stream)==0) {
            struct cdi_fs_file *file = malloc(sizeof(struct cdi_fs_file));
            file->res = res;
            file->fh = filesystem->last_fh++;
            file->type = FILE_REG;
            file->pos = (oflag&O_APPEND)?cdi_fs_filesize(filesystem,res):0;
            file->shmbuf = shmbuf;
            cdi_list_push(filesystem->files,file);
            return file->fh;
          }
          else {
            shmdt(shmbuf);
            return -cdi_fs_error2errno(stream.error);
          }
        }
        else {
          shmdt(shmbuf);
          return -ENOENT;
        }
      }
      else {
        shmdt(shmbuf);
        return -ENOENT;
      }
    }
    else return -errno;
  }
  else return -EINVAL;
}

static int fs_close(int fsid,int fh) {
  fprintf(stderr,"fs_close(%d,%d)\n",fsid,fh);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    struct cdi_fs_file *file = cdi_fs_file_find(filesystem,fh);
    if (file!=NULL) {
      if (file->type==FILE_REG) {
        struct cdi_fs_stream stream = CDI_FS_STREAM(filesystem,file->res);
        if (file->res->res->unload(&stream)) {
          shmdt(file->shmbuf);
          cdi_list_remove(filesystem->files,llist_find((llist_t)filesystem->files,file));
          free(file);
          return 0;
        }
        else return -ENOSYS;
      }
      else return -EBADF;
    }
    else return -EBADF;
  }
  else return -EINVAL;
}

static ssize_t fs_read(int fsid,int fh,size_t count) {
  fprintf(stderr,"fs_read(%d,%d,%d)\n",fsid,fh,count);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    struct cdi_fs_file *file = cdi_fs_file_find(filesystem,fh);
    if (file!=NULL) {
      if (file->res->file!=NULL && file->type==FILE_REG) {
        if (file->res->file->read!=NULL) {
          struct cdi_fs_stream stream = CDI_FS_STREAM(filesystem,file->res);
          ssize_t ret = file->res->file->read(&stream,file->pos,count,file->shmbuf);
          if (ret>0) file->pos += ret;
          return ret>=0?ret:-cdi_fs_error2errno(stream.error);
        }
        else return -ENOSYS;
      }
      else return -EBADF;
    }
    else return -EBADF;
  }
  else return -EINVAL;
}

static ssize_t fs_write(int fsid,int fh,size_t count) {
  fprintf(stderr,"fs_write(%d,%d,%d)\n",fsid,fh,count);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    struct cdi_fs_file *file = cdi_fs_file_find(filesystem,fh);
    if (file!=NULL) {
      if (file->res->file!=NULL && file->type==FILE_REG) {
        if (file->res->file->write!=NULL) {
          struct cdi_fs_stream stream = CDI_FS_STREAM(filesystem,file->res);
          ssize_t ret = file->res->file->write(&stream,file->pos,count,file->shmbuf);
          if (ret>0) file->pos += ret;
          return ret>=0?ret:-cdi_fs_error2errno(stream.error);
        }
        else return -ENOSYS;
      }
      else return -EBADF;
    }
    else return -EBADF;
  }
  else return -EINVAL;
}

static off_t fs_seek(int fsid,int fh,off_t off,int whence) {
  fprintf(stderr,"fs_seek(%d,%d,%d,%d)\n",fsid,fh,off,whence);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    struct cdi_fs_file *file = cdi_fs_file_find(filesystem,fh);
    if (file!=NULL) {
      if (file->type==FILE_REG) {
        if (whence==SEEK_SET) file->pos = off;
        else if (whence==SEEK_CUR) file->pos += off;
        else if (whence==SEEK_END) file->pos = cdi_fs_filesize(filesystem,file->res)+off;
        else return -EINVAL;
        return (off_t)file->pos;
      }
      else return -EBADF;
    }
    else return -EBADF;
  }
  else return -EINVAL;
}

static int fs_fstat(int fsid,int fh) {
  fprintf(stderr,"fs_fstat(%d,%d)\n",fsid,fh);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    struct cdi_fs_file *file = cdi_fs_file_find(filesystem,fh);
    if (file!=NULL) {
      struct cdi_fs_stream stream = CDI_FS_STREAM(filesystem,file->res);
      struct stat *stbuf = file->shmbuf;
      memset(stbuf,0,sizeof(struct stat));
      if (file->res->special!=NULL) file->res->special->dev_read(&stream,&(stbuf->st_rdev));
      stbuf->st_size = file->res->res->meta_read(&stream,CDI_FS_META_SIZE);
      stbuf->st_atime = file->res->res->meta_read(&stream,CDI_FS_META_ACCESSTIME);
      stbuf->st_mtime = file->res->res->meta_read(&stream,CDI_FS_META_CHANGETIME);
      stbuf->st_ctime = file->res->res->meta_read(&stream,CDI_FS_META_CREATETIME);
      stbuf->st_blksize = file->res->res->meta_read(&stream,CDI_FS_META_BLOCKSZ);
      stbuf->st_blocks = file->res->res->meta_read(&stream,CDI_FS_META_USEDBLOCKS);
      stbuf->st_mode = cdi_fs_class2mode(file->res);
      return 0;
    }
    else return -EBADF;
  }
  else return -EINVAL;
}

static int fs_unlink(int fsid,char *path) {
  fprintf(stderr,"fs_unlink(%d,%s)\n",fsid,path);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    struct cdi_fs_res *res = cdi_fs_path2res(filesystem,path);
    if (res!=NULL) {
      struct cdi_fs_stream stream = CDI_FS_STREAM(filesystem,res);
      //if (!llist_empty(res->children)) return -ENOTEMPTY;
      if (!(res->file!=NULL?res->res->remove_class(&stream,CDI_FS_CLASS_FILE):1)) return -cdi_fs_error2errno(stream.error);
      if (!(res->dir!=NULL?res->res->remove_class(&stream,CDI_FS_CLASS_DIR):1)) return -cdi_fs_error2errno(stream.error);
      if (!(res->link!=NULL?res->res->remove_class(&stream,CDI_FS_CLASS_LINK):1)) return -cdi_fs_error2errno(stream.error);
      if (!(res->special!=NULL?res->res->remove_class(&stream,CDI_FS_CLASS_SPECIAL):1)) return -cdi_fs_error2errno(stream.error);
      return res->res->remove(&stream)?0:-cdi_fs_error2errno(stream.error);
    }
    else return -ENOENT;
  }
  else return -EINVAL;
}

static int fs_rmdir(int fsid,char *path) {
  fprintf(stderr,"fs_rmdir(%d,%s)\n",fsid,path);
  return fs_unlink(fsid,path);
}

static int fs_ftruncate(int fsid,int fh,off_t newsize) {
  fprintf(stderr,"fs_ftruncate(%d,%d,%d)\n",fsid,fh,newsize);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    struct cdi_fs_file *file = cdi_fs_file_find(filesystem,fh);
    if (file!=NULL) {
      if (file->res->file!=NULL && file->type==FILE_REG) {
        struct cdi_fs_stream stream = CDI_FS_STREAM(filesystem,file->res);
        return file->res->file->truncate(&stream,newsize)?0:-cdi_fs_error2errno(stream.error);
      }
      else return -ENOENT;
    }
    else return -EBADF;
  }
  else return -EINVAL;
}

static int fs_opendir(int fsid,int shmid) {
  fprintf(stderr,"fs_opendir(%d,%d)\n",fsid,shmid);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    void *shmbuf = shmat(shmid,NULL,0);
    if (shmbuf!=NULL) {
      struct cdi_fs_res *res = cdi_fs_path2res(filesystem,shmbuf);
      if (res!=NULL) {
        struct cdi_fs_stream stream = CDI_FS_STREAM(filesystem,res);
        if (cdi_fs_loadres(&stream)==0) {
          if (res->dir!=NULL) {
            struct cdi_fs_file *file = malloc(sizeof(struct cdi_fs_file));
            file->res = res;
            file->fh = filesystem->last_fh++;
            file->type = FILE_DIR;
            file->pos = 0;
            file->shmbuf = shmbuf;
            file->dirlist = NULL;
            cdi_list_push(filesystem->files,file);
            return file->fh;
          }
          else {
            shmdt(shmbuf);
            return -ENOTDIR;
          }
        }
        else {
          shmdt(shmbuf);
          return -cdi_fs_error2errno(stream.error);
        }
      }
      else {
        shmdt(shmbuf);
        return -ENOENT;
      }
    }
    else return -errno;
  }
  else return -EINVAL;
}

static int fs_readdir(int fsid,int dh) {
  fprintf(stderr,"fs_readdir(%d,%d)\n",fsid,dh);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    struct cdi_fs_file *file = cdi_fs_file_find(filesystem,dh);
    if (file!=NULL) {
      if (file->type==FILE_DIR) {
        if (file->pos<2) {
          strcpy(file->shmbuf,file->pos++==0?".":"..");
          return 0;
        }
        else {
          if (file->dirlist==NULL) {
            struct cdi_fs_stream stream = CDI_FS_STREAM(filesystem,file->res);
            cdi_fs_loadres(&stream);
            if (file->res->dir!=NULL) file->dirlist = file->res->dir->list(&stream);
            else return -ENOTDIR;
          }
          struct cdi_fs_res *child = cdi_list_get(file->dirlist,(file->pos++)-2);
          if (child!=NULL) {
            strcpy(file->shmbuf,child->name);
            return 0;
          }
          else return -ENOENT;
        }
      }
      else return -ENOTDIR;
    }
    else return -EBADF;
  }
  else return -EINVAL;
}

static int fs_closedir(int fsid,int dh) {
  fprintf(stderr,"fs_closedir(%d,%d)\n",fsid,dh);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    struct cdi_fs_file *file = cdi_fs_file_find(filesystem,dh);
    if (file!=NULL) {
      if (file->type==FILE_DIR) {
        shmdt(file->shmbuf);
        cdi_list_remove(filesystem->files,llist_find((llist_t)filesystem->files,file));
        free(file);
        return 0;
      }
      else return -ENOTDIR;
    }
    else return -EBADF;
  }
  else return -EINVAL;
}

static off_t fs_seekdir(int fsid,int dh,off_t off) {
  fprintf(stderr,"fs_seekdir(%d,%d,%d)\n",fsid,dh,off);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    struct cdi_fs_file *file = cdi_fs_file_find(filesystem,dh);
    if (file!=NULL) {
      if (file->type==FILE_DIR) {
        if (off!=-1) file->pos = off;
        return file->pos;
      }
      else return -ENOTDIR;
    }
    else return -EBADF;
  }
  else return -EINVAL;
}

static int fs_statvfs(int fsid,int shmid) {
  fprintf(stderr,"fs_statvfs(%d,%d)\n",fsid,shmid);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    struct statvfs *stbuf = shmat(shmid,NULL,0);
    if (stbuf!=NULL) {
      memset(stbuf,0,sizeof(struct statvfs));
      /// @todo
      shmdt(stbuf);
      return 0;
    }
    else return -errno;
  }
  else return -EINVAL;
}

static int fs_mknod(int fsid,char *path,mode_t mode,dev_t dev) {
  fprintf(stderr,"fs_mknod(%d,%s,%d,%d)\n",fsid,path,mode,dev);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    if (cdi_fs_path2res(filesystem,path)==NULL) {
      char *filename;
      struct cdi_fs_res *res = cdi_fs_parentres(filesystem,path,&filename);
      if (res!=NULL) {
        if (res->dir!=NULL) {
          struct cdi_fs_stream stream = CDI_FS_STREAM(filesystem,NULL);
          if (!res->dir->create_child(&stream,filename,res)) return -cdi_fs_error2errno(stream.error);
          if (cdi_fs_loadres(&stream)==-1) return -cdi_fs_error2errno(stream.error);
          stream.res->type = 0;
          if (!stream.res->res->assign_class(&stream,cdi_fs_mode2class(mode,&(stream.res->type)))) return -cdi_fs_error2errno(stream.error);
          /// @todo ACL
          return cdi_fs_unloadres(&stream)==0?0:-cdi_fs_error2errno(stream.error);
        }
        else return -ENOTDIR;
      }
      else return -ENOENT;
    }
    else return -EEXIST;
  }
  else return -EINVAL;
}

static ssize_t fs_readlink(int fsid,int shmid,size_t bufsize) {
  fprintf(stderr,"fs_readlink(%d,%d,%d)\n",fsid,shmid,bufsize);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    void *shmbuf = shmat(shmid,NULL,0);
    if (shmbuf!=NULL) {
      struct cdi_fs_res *res = cdi_fs_path2res(filesystem,shmbuf);
      if (res!=NULL) {
        if (res->link!=NULL) {
          struct cdi_fs_stream stream = CDI_FS_STREAM(filesystem,res);
          const char *link = res->link->read_link(&stream);
          size_t len = 0;
          if (link!=NULL) {
            len = strlen(link);
            strcpy(shmbuf,link);
          }
          shmdt(shmbuf);
          return len;
        }
        else {
          shmdt(shmbuf);
          return -ENOLINK;
        }
      }
      else {
        shmdt(shmbuf);
        return -ENOENT;
      }
    }
    else return -errno;
  }
  else return -EINVAL;
}

static ssize_t fs_symlink(int fsid,char *src,char *dest) {
  fprintf(stderr,"fs_symlink(%d,%s,%s)\n",fsid,src,dest);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    char *filename;
    struct cdi_fs_res *res = cdi_fs_parentres(filesystem,src,&filename);
    if (res!=NULL) {
      if (res->dir!=NULL) {
        struct cdi_fs_stream stream = CDI_FS_STREAM(filesystem,NULL);
        if (!res->dir->create_child(&stream,filename,res)) return -cdi_fs_error2errno(stream.error);
        if (cdi_fs_loadres(&stream)==-1) return -cdi_fs_error2errno(stream.error);
        stream.res->type = 0;
        if (!stream.res->res->assign_class(&stream,CDI_FS_CLASS_LINK)) return -cdi_fs_error2errno(stream.error);
        /// @todo ACL
        if (!stream.res->link->write_link(&stream,dest)) return -cdi_fs_error2errno(stream.error);
        return cdi_fs_unloadres(&stream)==0?0:-cdi_fs_error2errno(stream.error);
      }
      else return -ENOTDIR;
    }
    else return -ENOENT;
  }
  else return -EINVAL;
}

static int fs_dup(int fsid,int fh,int shmid) {
  fprintf(stderr,"fs_dup(%d,%d,%d)\n",fsid,fh,shmid);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    struct cdi_fs_file *file = cdi_fs_file_find(filesystem,fh);
    if (file!=NULL) {
      void *shmbuf = shmat(shmid,NULL,0);
      if (shmbuf!=NULL) {
        struct cdi_fs_file *new = malloc(sizeof(struct cdi_fs_file));
        new->fh = filesystem->last_fh++;
        new->shmbuf = shmbuf;
        return new->fh;
      }
      else return -errno;
    }
    else return -EBADF;
  }
  else return -EINVAL;
}

static int fs_utime(int fsid,int shmid) {
  fprintf(stderr,"fs_utime(%d,%d)\n",fsid,shmid);
  struct cdi_fs_filesystem *filesystem = cdi_fs_find(fsid);
  if (filesystem!=NULL) {
    void *shmbuf = shmat(shmid,NULL,0);
    if (shmbuf!=NULL) {
      struct cdi_fs_res *res = cdi_fs_path2res(filesystem,shmbuf);
      if (res!=NULL) {
        struct utimbuf *times = shmbuf+strlen(shmbuf)+1;
        struct cdi_fs_stream stream = CDI_FS_STREAM(filesystem,res);
        res->res->meta_write(&stream,CDI_FS_META_ACCESSTIME,times->actime);
        res->res->meta_write(&stream,CDI_FS_META_CHANGETIME,times->modtime);
        shmdt(shmbuf);
        return 0;
      }
      else {
        shmdt(shmbuf);
        return -ENOENT;
      }
    }
    else return -errno;
  }
  else return -EINVAL;
}

/**
 * Initializes CDI FS
 *  @return Success?
 */
int cdi_fs_init() {
  pid_t pid = getpid();
  cdi_fs_func(fs_open,"iii",sizeof(int)*3);
  cdi_fs_func(fs_close,"ii",sizeof(int)*2);
  cdi_fs_func(fs_read,"iii",sizeof(int)*3);
  cdi_fs_func(fs_write,"iii",sizeof(int)*3);
  cdi_fs_func(fs_seek,"iiii",sizeof(int)*4);
  cdi_fs_func(fs_fstat,"ii",sizeof(int)*2);
  cdi_fs_func(fs_unlink,"i$",sizeof(int)+PATH_MAX);
  cdi_fs_func(fs_rmdir,"i$",sizeof(int)+PATH_MAX);
  //cdi_fs_func(fs_rename,"i$$",sizeof(int)+PATH_MAX*2);
  cdi_fs_func(fs_ftruncate,"iii",sizeof(int)*3);
  cdi_fs_func(fs_opendir,"ii",sizeof(int)*2);
  cdi_fs_func(fs_readdir,"ii",sizeof(int)*2);
  cdi_fs_func(fs_closedir,"ii",sizeof(int)*2);
  cdi_fs_func(fs_seekdir,"iii",sizeof(int)*3);
  cdi_fs_func(fs_statvfs,"ii",sizeof(int));
  cdi_fs_func(fs_readlink,"iii",sizeof(int)*3);
  cdi_fs_func(fs_symlink,"i$$",sizeof(int)+PATH_MAX*2);
  //cdi_fs_func(fs_link,"i$$",sizeof(int)+PATH_MAX*2);
  cdi_fs_func(fs_mknod,"i$ii",sizeof(int)*3+PATH_MAX);
  cdi_fs_func(fs_dup,"iii",sizeof(int)*3);
  /*cdi_fs_func(fs_chown,"i$ii",sizeof(int)*3+PATH_MAX);
  cdi_fs_func(fs_chmod,"i$i",sizeof(int)*2+PATH_MAX);
  cdi_fs_func(fs_access,"i$i",sizeof(int)*2+PATH_MAX);*/
  cdi_fs_func(fs_utime,"ii",sizeof(int)*2);
  return 0;
}

/**
 * Initializes FS driver
 *  @param driver FS driver
 */
void cdi_fs_driver_init(struct cdi_fs_driver* driver) {
  fprintf(stderr,"cdi_fs_driver_init(0x%x)\n",driver);
  cdi_driver_init((struct cdi_driver*)driver);
  driver->filesystems = cdi_list_create();
}

/**
 * Destroys FS driver
 *  @param driver FS driver
 */
void cdi_fs_driver_destroy(struct cdi_fs_driver* driver) {
  fprintf(stderr,"cdi_fs_driver_destroy(0x%x)\n",driver);
  struct cdi_fs_filesystem *filesystem;
  while ((filesystem = cdi_list_pop(driver->filesystems))) driver->fs_destroy(filesystem);
  cdi_list_destroy(driver->filesystems);
  cdi_driver_destroy((struct cdi_driver*)driver);
}

/**
 * Registers FS driver
 *  @param driver FS driver
 */
void cdi_fs_driver_register(struct cdi_fs_driver* driver) {
  fprintf(stderr,"cdi_fs_driver_register(0x%x)\n",driver);
  cdi_driver_register((struct cdi_driver*)driver);
  char *name;
  asprintf(&name,"%s_mount",driver->drv.name);
  rpc_func_create(name,cdi_fs_mount_rpc,"$$$i",NAME_MAX+PATH_MAX+sizeof(int));
  free(name);
  asprintf(&name,"%s_unmount",driver->drv.name);
  rpc_func_create(name,cdi_fs_unmount_rpc,"$$",NAME_MAX);
  free(name);
}

size_t cdi_fs_data_read(struct cdi_fs_filesystem* fs,uint64_t start,size_t size,void* buffer) {
  fprintf(stderr,"cdi_fs_data_read(0x%x,0x%x,0x%x,0x%x)\n",fs,start,size,buffer);
  if (fs->data_dev!=NULL) {
    if (lseek(fs->data_fh,(off_t)start,SEEK_SET)!=-1) return read(fs->data_fh,buffer,size);
  }
  return 0;
}

size_t cdi_fs_data_write(struct cdi_fs_filesystem* fs,uint64_t start,size_t size,const void* buffer) {
  fprintf(stderr,"cdi_fs_data_write(0x%x,0x%x,0x%x,0x%x)\n",fs,start,size,buffer);
  if (fs->data_dev!=NULL) {
    if (lseek(fs->data_fh,(off_t)start,SEEK_SET)!=-1) return write(fs->data_fh,buffer,size);
  }
  return 0;
}
