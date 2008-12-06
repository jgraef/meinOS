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

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <llist.h>
#include <errno.h>
#include <stdarg.h>
#include <rpc.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <limits.h>
#include <stdio.h>
#include <path.h>
#include <misc.h>

#include <fcntl.h>       // open,close
#include <unistd.h>      // read,write,seek,truncate
#include <dirent.h>      // *dir
#include <sys/stat.h>    // stat
#include <sys/statvfs.h> // statvfs
#include <utime.h>       // utime

#include <sys/socket.h>  // Socket stuff

#define getnew_fh() (lastfh++)
#define filebyfh(fh) llist_get(filelist,lidxbyfh(fh))

#define SHMMEM_SIZE PAGE_SIZE

struct fslist_item {
  pid_t pid;
  id_t id;
  path_t *mountpoint;
  char *mountpoint_str;
};

struct filelist_item {
  struct fslist_item *fs;
  int fs_fh;
  struct dirent dir_cur;
  mode_t mode;
  int oflag;
  int fh;
  char *path;
  int shmid;
  void *shmbuf;
  enum { FL_FILE,FL_DIR,FL_PIPE,FL_SOCK } type;
  /// @todo Check for right file types (e.g. in read()/write()) and set them in open()/opendir()/socket()
};

static llist_t filelist;
static llist_t fslist;
static int lastfh;
static struct {
  char *str;
  size_t strlen;
  path_t *path;
} workdir;
static mode_t creation_mask;
struct process_data *_process_data;

static int _open_unnamed_pipe(int fh,int shmid);

/**
 * Initializes File I/O
 */
void _fs_init() {
  filelist = llist_create();
  fslist = llist_create();
  lastfh = 3;
  creation_mask = 0777;

  // Init current workdir
  char *path = getenv("PATH");
  if (path==NULL) path = "/";
  memset(&workdir,0,sizeof(workdir));
  chdir(path);

  // Create stdin, stdout, stderr
  _open_unnamed_pipe(STDIN_FILENO,_process_data->shmid_stdin);
  _open_unnamed_pipe(STDOUT_FILENO,_process_data->shmid_stdout);
  _open_unnamed_pipe(STDERR_FILENO,_process_data->shmid_stderr);
}

/**
 * Match mountpoint
 *  @param file Path to file
 *  @param parent If FS of parent directory of file is needed
 *  @return FS List item
 */
static struct fslist_item *mp_match(char *file,int parent) {
  size_t i,curcmp;
  size_t maxcmp = 0;
  struct fslist_item *fs;
  struct fslist_item *maxfs = NULL;
  path_t *path = path_parse(file);

  if (parent) path_parent(path);

  for (i=0;(fs = llist_get(fslist,i));i++) {
    curcmp = path_compare(path,fs->mountpoint);
    if (curcmp>maxcmp) {
      maxcmp = curcmp;
      maxfs = fs;
    }
  }

  if (maxfs!=NULL) memmove(file,file+strlen(maxfs->mountpoint_str),strlen(file)-strlen(maxfs->mountpoint_str)+1);
  if (file[0]==0) strcpy(file,"/");

  return maxfs;
}

/**
 * Gets FSID and PID by path and cuts path to relative path for FS
 *  @param path Path to file
 *  @param parent If FS of parent directory of file is needed
 *  @return FSID
 */
static struct fslist_item *fsbypath(char *path,int parent) {
  struct fslist_item *fs = mp_match(path,parent);
  if (fs==NULL) {
    fs = malloc(sizeof(struct fslist_item));
    char *mountpoint = strdup(path);
    fs->id = rpc_call("vfs_getfsid",2,path,parent);
    if (fs->id<0) {
      free(mountpoint);
      errno = -fs->id;
      return NULL;
    }
    mountpoint[strlen(mountpoint)-strlen(path)] = 0;
    fs->mountpoint = path_parse(mountpoint);
    fs->mountpoint_str = path_output(fs->mountpoint,NULL);
    free(mountpoint);
    fs->pid = rpc_call("vfs_getpid",0,fs->id);
    if (fs->pid<0) {
      errno = -fs->pid;
      return NULL;
    }
    llist_push(fslist,fs);
  }

  return fs;
}

/**
 * Gets list index by filehandle
 *  @param fh filehandle
 *  @return List index
 */
static int lidxbyfh(int fh) {
  struct filelist_item *file;
  int i;
  for (i=0;(file = llist_get(filelist,i));i++) {
    if (file->fh==fh) return i;
  }
  return -1;
}

/**
 * Gets absolute path by relative path
 *  @param relative Relative or absolute path
 *  @return Absolute path (Can be passed to free())
 */
static char *getabsolutepath(const char *relative) {
  path_t *path = path_parse(relative);
  if (!path->root) {
    path_t *newpath = path_cat(workdir.path,path);
    path_destroy(path);
    path = newpath;
  }
  path_reject_dots(path);
  return path_output(path,NULL);
}

/**
 * Creates an unnamed pipe
 *  @param read Whether creator is reader
 *  @return Filehandle
 */
int _create_unnamed_pipe(int read,int *_shmid) {
  int shmid = shmget(IPC_PRIVATE,SHMMEM_SIZE,0);
  if (shmid!=-1) {
    size_t *shmbuf = shmat(shmid,NULL,0);
    if (shmbuf!=NULL) {
      struct filelist_item *new = malloc(sizeof(struct filelist_item));
      memset(new,0,sizeof(struct filelist_item));
      new->fh = getnew_fh();
      new->oflag = read?O_RDONLY:O_WRONLY;
      new->shmid = shmid;
      new->shmbuf = shmbuf;
      shmbuf[0] = read; // Direction
      shmbuf[1] = 0; // Reserved for error
      shmbuf[2] = 0; // Reading position
      shmbuf[3] = 0; // Writing position
      llist_push(filelist,new);
      if (_shmid!=NULL) *_shmid = shmid;
      return new->fh;
    }
    else shmctl(shmid,IPC_RMID,NULL);
  }
  return -1;
}

/**
 * Destroys an unnamed pipe
 *  @param fh Filehandle of unnamed pipe
 *  @return Success?
 */
int _destroy_unnamed_pipe(int fh) {
  struct filelist_item *file = filebyfh(fh);

  if (file && file->fs==NULL) {
    shmdt(file->shmbuf);
    shmctl(file->shmid,IPC_RMID,NULL);
    llist_remove(filelist,lidxbyfh(fh));
    free(file);
    return 0;
  }
  else errno = EBADF;
  return -1;
}

/**
 * Opens an unnamed pipe
 *  @param fh Use this filehandle (-1 no specified filehandle)
 *  @param shmid SHM object the unnamed pipe uses
 *  @note If you specify the filehandle, be sure that it isn't used already
 *  @return Filehandle
 */
static int _open_unnamed_pipe(int fh,int shmid) {
  size_t *shmbuf = shmat(shmid,NULL,0);
  if (shmbuf!=NULL) {
    if (fh==-1) fh = getnew_fh();
    struct filelist_item *new = malloc(sizeof(struct filelist_item));
    memset(new,0,sizeof(struct filelist_item));
    new->fh = fh;
    new->oflag = shmbuf[0]?O_WRONLY:O_RDONLY;
    new->shmid = shmid;
    new->shmbuf = shmbuf;
    llist_push(filelist,new);
    return fh;
  }
  else return -1;
}

/**
 * Destroys an unnamed pipe
 *  @param fh Filehandle of unnamed pipe
 *  @return Success?
 */
static int _close_unnamed_pipe(int fh) {
  struct filelist_item *file = filebyfh(fh);

  if (file && file->fs==NULL) {
    shmdt(file->shmbuf);
    llist_remove(filelist,lidxbyfh(fh));
    free(file);
    return 0;
  }
  else errno = EBADF;
  return -1;
}

/**
 * Reads an unnamed pipe
 *  @param fh Filehandle
 *  @param data Data buffer
 *  @param size How many bytes to read
 *  @return How many bytes read
 */
static size_t _read_unnamed_pipe(int fh,void *data,size_t size) {
  struct filelist_item *file = filebyfh(fh);
  ssize_t size_read = -1;

  if (file && file->fs==NULL && file->oflag==O_RDONLY) {
    size_t *pos_read = file->shmbuf+2*sizeof(size_t);
    size_t *pos_write = file->shmbuf+3*sizeof(size_t);
    void *buffer = file->shmbuf+4*sizeof(size_t);
    size_t size_free = *pos_write>=*pos_read?*pos_write-*pos_read:*pos_write+SHMMEM_SIZE-4*sizeof(int)-*pos_read;

    if (size>size_free) size = size_free;
    if (*pos_write>=*pos_read) memcpy(data,buffer,size);
    else {
      size_t part1 = SHMMEM_SIZE-4*sizeof(int)-*pos_read;
      size_t part2 = *pos_write;
      memcpy(data,buffer+*pos_read,part1);
      memcpy(data+part1,buffer,part2);
    }
    *pos_read = (*pos_read+size)%(SHMMEM_SIZE-4*sizeof(int));

    size_read = size;
  }
  else errno = EBADF;
  return size_read;
}

/**
 * Writes an unnamed pipe
 *  @param fh Filehandle
 *  @param data Data buffer
 *  @param size How many bytes to write
 *  @return How many bytes written
 */
static size_t _write_unnamed_pipe(int fh,const void *data,size_t size) {
  struct filelist_item *file = filebyfh(fh);
  ssize_t size_written = -1;

  if (file && file->fs==NULL && file->oflag==O_WRONLY) {
    size_t *pos_read = file->shmbuf+2*sizeof(size_t);
    size_t *pos_write = file->shmbuf+3*sizeof(size_t);
    void *buffer = file->shmbuf+4*sizeof(size_t);
    size_t size_free = *pos_read>*pos_write?*pos_read-*pos_write:*pos_read+SHMMEM_SIZE-4*sizeof(int)-*pos_write;

    if (size>size_free) size = size_free;
    if (*pos_read>=*pos_write) memcpy(buffer,data,size);
    else {
      size_t part1 = SHMMEM_SIZE-4*sizeof(int)-*pos_write;
      size_t part2 = *pos_read;
      memcpy(buffer+*pos_write,data,part1);
      memcpy(buffer,data+part1,part2);
    }
    *pos_write = (*pos_write+size)%(SHMMEM_SIZE-4*sizeof(int));

    size_written = size;
  }
  else errno = EBADF;
  return size_written;
}

/**
 * Gets current workdir
 *  @param buf Buffer to store path in
 *  @param size Size of buffer
 *  @return Current workdir
 */
char *getcwd(char *buf, size_t size) {
  if (size==0) {
    errno = EINVAL;
    return NULL;
  }
  if (workdir.strlen+1>size) {
    errno = ERANGE;
    return NULL;
  }
  return strncpy(buf,workdir.str,size);
}

/**
 * Change current workdir
 *  @param new New workdir
 *  @return 0=success; -1=failure
 */
int chdir(const char *new) {
  new = getabsolutepath((char*)new);
  if (new==NULL) new = "/";

  /*DIR *dir = opendir(new);
  if (dir!=NULL) {
    closedir(dir);*/
    free(workdir.str);
    path_destroy(workdir.path);
    workdir.path = path_parse(new);
    path_reject_dots(workdir.path);
    workdir.str = path_output(workdir.path,NULL);
    workdir.strlen = strlen(workdir.str);
    setenv("PATH",workdir.str,1);
    return 0;
  //}
  //else return -1;
}

/**
 * Change current workdir
 *  @param new Filedescriptor of new workdir
 *  @return 0=success; -1=failure
 */
int fchdir(int fildes) {
  int res;
  struct filelist_item *file = filebyfh(fildes);
  if (file!=NULL) res = chdir(file->path);
  else res = -EBADF;
  errno = res<0?-res:0;
  return res<0?-1:res;
}

/**
 * Creates a new node
 *  @param path Path to new node
 *  @param mode Mode of new node
 *  @param dev DeviceID (not used)
 *  @return 0=success; -1=failure
 */
int mknod(const char *path,mode_t mode,dev_t dev) {
  int res;
  char *cpath = getabsolutepath((char*)path);
  struct fslist_item *fs = fsbypath(cpath,1);
  char *func;

  if (fs!=NULL) {
    asprintf(&func,"fs_mknod_%x",fs->pid);
    res = rpc_call(func,0,fs->id,cpath,mode&(~creation_mask),dev);
    free(func);
  }
  else res = -EINVAL;
  free(cpath);
  errno = res<0?-res:0;
  return res<0?-1:res;
}

/**
 * Checks whether file can be opened with mode
 *  @param path Path to file
 *  @param amode Mode
 *  @return 0=success; -1=failure
 */
int access(const char *path,int amode) {
  int res;
  char *cpath = getabsolutepath((char*)path);
  struct fslist_item *fs = fsbypath(cpath,0);
  char *func;

  if (fs!=NULL) {
    asprintf(&func,"fs_access_%x",fs->pid);
    res = rpc_call(func,0,fs->id,cpath,amode);
    free(func);
  }
  else res = -EINVAL;
  free(cpath);
  errno = res<0?-res:0;
  return res<0?-1:res;
}

/**
 * Opens a file
 *  @param path Path to file
 *  @param oflag Mode
 */
int open(const char *path,int oflag,...) {
  va_list args;
  int fh = 0;
  void *shmbuf;
  char *func;
  char *cpath = getabsolutepath(path);
  struct fslist_item *fs = fsbypath(cpath,0);
  if (fs==NULL) fh = -errno;

  va_start(args,oflag);

  if (fh==0) {
    if (oflag&O_CREAT) {
      oflag &= ~O_CREAT;
      int res = mknod(path,va_arg(args,mode_t)|S_IFREG,0);
      if (res==-1 && errno!=EEXIST) fh = -errno;
    }
    int shmid = shmget(IPC_PRIVATE,SHMMEM_SIZE,0);
    shmbuf = shmat(shmid,NULL,0);
    if (shmbuf!=NULL) {
      strncpy(shmbuf,cpath,SHMMEM_SIZE);
      asprintf(&func,"fs_open_%x",fs->pid);
      fh = rpc_call(func,0,fs->id,oflag,shmid);
      free(func);
      if (fh>=0) {
        struct filelist_item *new = malloc(sizeof(struct filelist_item));
        new->fs_fh = fh;
        new->fs = fs;
        new->oflag = oflag;
        new->mode = S_IFREG;
        new->fh = getnew_fh();
        new->path = strdup(cpath);
        new->shmid = shmid;
        new->shmbuf = shmbuf;
        fh = new->fh;
        llist_push(filelist,new);
      }
    }
  }
  va_end(args);
  free(cpath);
  errno = fh<0?-fh:0;
  return fh<0?-1:fh;
}

/**
 * Closes all filehandles
 */
void _close_all_filehandles() {
  struct filelist_item *file;
  while ((file = llist_pop(filelist))!=NULL) {
    if (S_ISREG(file->mode)) close(file->fh);
    else if (S_ISDIR(file->mode)) closedir(file);
  }

  // close stdin, stdout and stderr
  _destroy_unnamed_pipe(STDIN_FILENO);
  _destroy_unnamed_pipe(STDOUT_FILENO);
  _destroy_unnamed_pipe(STDERR_FILENO);

}

/**
 * Closes a file
 *  @param fildes File descriptor
 *  @return 0=success; -1=failure
 */
int close(int fildes) {
  int res;
  char *func;
  struct filelist_item *file = filebyfh(fildes);
  if (file->fs==NULL) return _close_unnamed_pipe(fildes);
  if (file) {
    asprintf(&func,"fs_close_%x",file->fs->pid);
    res = rpc_call(func,0,file->fs->id,file->fs_fh);
    free(func);
    if (res==0) {
      shmdt(file->shmbuf);
      shmctl(file->shmid,IPC_RMID,NULL);
      llist_remove(filelist,lidxbyfh(fildes));
      free(file->path);
      free(file);
    }
  }
  else res = -EBADF;
  errno = res<0?-res:0;
  return res<0?-1:res;
  return -1;
}

/**
 * Duplicates a file descriptor
 *  @param fildes File descriptor
 *  @param fildes2 New file descriptor
 *  @return New file desciptor
 */
int dup2(int fildes,int fildes2) {
  int res = -1;
  char *func;
  struct filelist_item *file = filebyfh(fildes);
  if (file) {
    asprintf(&func,"fs_close_%x",file->fs->pid);
    res = rpc_call(func,0,file->fs->id,file->fs_fh);
    free(func);
    if (res>=0) {
      struct filelist_item *new = malloc(sizeof(struct filelist_item));
      memcpy(new,file,sizeof(struct filelist_item));
      if (fildes2==0) new->fh = getnew_fh();
      else {
        close(fildes2);
        new->fh = fildes2;
      }
      new->fs_fh = res;
      new->shmid = shmget(IPC_PRIVATE,SHMMEM_SIZE,0);
      new->shmbuf = shmat(new->shmid,NULL,0);
      llist_push(filelist,new);
    }
  }
  else errno = EBADF;
  return res;
}

/**
 * Reads from a file
 *  @param fildes File descriptor
 *  @param buf Buffer to store read data in
 *  @param count How many bytes to read
 *  @return How many bytes read
 */
static ssize_t _read(struct filelist_item *file,void *buf,size_t count) {
  ssize_t res;
  char *func;
  asprintf(&func,"fs_read_%x",file->fs->pid);
  res = rpc_call(func,0,file->fs->id,file->fs_fh,count);
  free(func);
  if (res>0) memcpy(buf,file->shmbuf,res);
  return res;
}
ssize_t read(int fildes,void *buf,size_t count) {
  struct filelist_item *file = filebyfh(fildes);
  if (file->fs==NULL) return _read_unnamed_pipe(fildes,buf,count);
  else if (file!=NULL) {
    ssize_t count_rem = count;
    size_t off = 0;

    while (count_rem>0) {
      size_t count_cur = count_rem;
      if (count_cur>SHMMEM_SIZE) count_cur = SHMMEM_SIZE;
      count_cur = _read(file,buf+off,count_cur);
      if (count_cur==-1) return -1;
      count_rem -= count_cur;
      if (count_cur==0) break;
      off += count_cur;
    }

    return count-count_rem;
  }
  else {
    errno = EBADF;
    return -1;
  }
}

/**
 * Writes to a file
 *  @param fildes File descriptor
 *  @param buf Data to write to file
 *  @param count How many bytes to write
 *  @return How many bytes written
 */
static ssize_t _write(struct filelist_item *file,const void *buf,size_t count) {
  char *func;
  ssize_t res;
  memcpy(file->shmbuf,buf,count);
  asprintf(&func,"fs_write_%x",file->fs->pid);
  res = rpc_call(func,0,file->fs->id,file->fs_fh,count);
  free(func);
  return res;
}
ssize_t write(int fildes,const void *buf,size_t count) {
  struct filelist_item *file = filebyfh(fildes);
  if (file->fs==NULL) return _write_unnamed_pipe(fildes,buf,count);
  else if (file) {
    ssize_t count_rem = count;
    size_t off = 0;
    while (count_rem>0) {
      size_t count_cur = count_rem;
      if (count_cur>SHMMEM_SIZE) count_cur = SHMMEM_SIZE;
      count_cur = _write(file,buf+off,count_cur);
      if (count_cur==-1) return -1;
      count_rem -= count_cur;
      if (count_cur==0) break;
      off += count_cur;
    }
    return count-count_rem;
  }
  else {
    errno = EBADF;
    return -1;
  }
}

/**
 * Seeks in a file
 *  @param fildes File descriptor
 *  @param offset Offset
 *  @param whence SEEK_SET, SEEK_CUR, SEEK_END
 *  @return New position
 */
off_t lseek(int fildes,off_t offset,int whence) {
  int res;
  char *func;
  struct filelist_item *file = filebyfh(fildes);
  if (file) {
    asprintf(&func,"fs_seek_%x",file->fs->pid);
    res = rpc_call(func,0,file->fs->id,file->fs_fh,offset,whence);
    free(func);
  }
  else res = -EBADF;
  errno = res<0?-res:0;
  return res<0?-1:res;
}

/**
 * Removes a file
 *  @param path Path to file
 *  @return 0=success; -1=failure
 */
int unlink(const char *path) {
  int res;
  char *cpath = getabsolutepath((char*)path);
  struct fslist_item *fs = fsbypath(cpath,1);
  char *func;

  if (fs!=NULL) {
    asprintf(&func,"fs_unlink_%x",fs->pid);
    res = rpc_call(func,0,fs->id,cpath);
    free(func);
  }
  else res = -EINVAL;
  free(cpath);
  errno = res<0?-res:0;
  return res<0?-1:res;
}

/**
 * Removes a directory
 *  @param path Path to directory
 *  @return 0=success; -1=failure
 */
int rmdir(const char *path) {
  int res;
  char *cpath = getabsolutepath((char*)path);
  struct fslist_item *fs = fsbypath(cpath,1);
  char *func;

  if (fs!=NULL) {
    asprintf(&func,"fs_rmdir_%x",fs->pid);
    res = rpc_call(func,0,fs->id,cpath);
    free(func);
  }
  else res = -EINVAL;
  free(cpath);
  errno = res<0?-res:0;
  return res<0?-1:res;
}

/**
 * Renames a file
 *  @param path Path to directory
 *  @return 0=success; -1=failure
 */
int rename(const char *old, const char *new) {
  int res;
  char *cold = getabsolutepath((char*)old);
  char *cnew = getabsolutepath((char*)new);
  struct fslist_item *fs = fsbypath(cold,1);
  struct fslist_item *fsnew = fsbypath(cnew,1);
  char *func;

  if (fs!=NULL && fs==fsnew) {
    asprintf(&func,"fs_rename_%x",fs->pid);
    res = rpc_call(func,0,fs->id,old,new);
    free(func);
  }
  else res = -EINVAL;
  errno = res<0?-res:0;
  return res<0?-1:res;
}

/**
 * Truncates a file
 *  @param path Path
 *  @param length New length
 *  @return 0=success; -1=failure
 */
int truncate(const char *path,off_t length) {
  int fh;
  int res = -1;
  if ((fh = open(path,O_WRONLY))!=-1) {
    if (ftruncate(fh,length)!=-1) res = 0;
    close(fh);
  }
  return res;
}

/**
 * Truncates a file
 *  @param fildes File descriptor
 *  @param length New length
 *  @return 0=success; -1=failure
 */
int ftruncate(int fildes,off_t length) {
  int res;
  char *func;
  struct filelist_item *file = filebyfh(fildes);
  if (file) {
    asprintf(&func,"fs_ftruncate_%x",file->fs->pid);
    res = rpc_call(func,0,file->fs->id,file->fs_fh,length);
    free(func);
  }
  else res = -EBADF;
  errno = res<0?-res:0;
  return res<0?-1:res;
  return -1;
}

/**
 * Opens a directory
 *  @param path Path to directory
 *  @return Pointer to dir handle
 */
DIR *opendir(const char *path) {
  int dh,res;
  void *shmbuf;
  char *func;
  char *cpath = getabsolutepath(path);
  struct fslist_item *fs = fsbypath(cpath,0);

  struct filelist_item *new;
  if (fs!=NULL) {
    int shmid = shmget(IPC_PRIVATE,SHMMEM_SIZE,0);
    shmbuf = shmat(shmid,NULL,0);
    if (shmbuf!=NULL) {
      strncpy(shmbuf,cpath,SHMMEM_SIZE);
      asprintf(&func,"fs_opendir_%x",fs->pid);
      dh = rpc_call(func,0,fs->id,shmid);
      free(func);
      if (dh>=0) {
        new = malloc(sizeof(struct filelist_item));
        new->fs_fh = dh;
        new->fs = fs;
        new->oflag = 0;
        new->mode = S_IFDIR;
        new->fh = 0; // Dir handles don't need IDs
        new->path = strdup(cpath);
        new->shmid = shmid;
        new->shmbuf = shmbuf;
        new->dir_cur.d_name = NULL;
        llist_push(filelist,new);
        res = 0;
      }
      else res = dh;
    }
    else res = -errno;
  }
  else res = -ENOENT;
  free(cpath);
  errno = res<0?-res:0;
  return res<0?NULL:((DIR*)new);
}

/**
 * Closes a directory
 *  @param file Dirhandle
 *  @return Errorcode
 */
int closedir(DIR *file) {
  int res;
  char *func;
  if (file) {
    asprintf(&func,"fs_closedir_%x",file->fs->pid);
    res = rpc_call(func,0,file->fs->id,file->fs_fh);
    free(func);
    if (res==0) {
      shmdt(file->shmbuf);
      shmctl(file->shmid,IPC_RMID,NULL);
      free(file->path);
      free(file);
      llist_remove(filelist,llist_find(filelist,file));
    }
  }
  else res = -EBADF;
  errno = res<0?-res:0;
  return res<0?-1:res;
}

/**
 * Reads an entry from an opened dir
 *  @param dh Dirhandle
 *  @return Direntry
 */
struct dirent *readdir(DIR *file) {
  int res;
  char *func;
  if (file!=NULL) {
    asprintf(&func,"fs_readdir_%x",file->fs->pid);
    res = rpc_call(func,0,file->fs->id,file->fs_fh);
    free(func);
    if (res>=0) {
      if (file->dir_cur.d_name!=NULL) free(file->dir_cur.d_name);
      file->dir_cur.d_name = strdup(file->shmbuf);
    }
  }
  else res = -EBADF;
  errno = res<0 && res!=-ENOENT?-res:0;
  return res<0?NULL:&(file->dir_cur);
}

/**
 * Seeks in dir
 *  @param dh Dirhandle
 *  @param loc Location to seek to
 */
void seekdir(DIR *file,long loc) {
  int res;
  char *func;
  if (file) {
    asprintf(&func,"fs_seekdir_%x",file->fs->pid);
    res = rpc_call(func,0,file->fs->id,file->fs_fh,loc);
    free(func);
  }
  else res = -EBADF;
  errno = res<0?-res:0;
}

/**
 * Gets location in dir
 *  @param dh Dirhandle
 *  @return Location in dir
 */
long telldir(DIR *file) {
  int res;
  char *func;
  if (file) {
    asprintf(&func,"fs_seekdir_%x",file->fs->pid);
    res = rpc_call(func,0,file->fs->id,file->fs_fh,-1);
    free(func);
  }
  else res = -EBADF;
  errno = res<0?-res:0;
  return res<0?-1:res;
}

/**
 * Gets informations about a file
 *  @param path Path to file
 *  @param buf Pointer to stat structure
 *  @return 0=success; -1=failure
 */
int stat(const char *path,struct stat *buf) {
  int fh;
  int ret = -1;
  if ((fh = open(path,O_RDONLY))!=-1) {
    ret = fstat(fh,buf);
    close(fh);
  }
  else {
    struct filelist_item *dir = opendir(path);
    if (dir!=NULL) {
      ret = fstat(dir->fh,buf);
      closedir(dir);
    }
  }
  return ret;
}

/**
 * Gets informations about a filedescriptor
 *  @param fildes Filedescriptor
 *  @param buf Pointer to stat structure
 *  @return 0=success; -1=failure
 */
int fstat(int fildes,struct stat *buf) {
  int res;
  char *func;
  struct filelist_item *file = filebyfh(fildes);
  if (file) {
    asprintf(&func,"fs_fstat_%x",file->fs->pid);
    memset(buf,0,sizeof(struct stat));
    res = rpc_call(func,0,file->fs->id,file->fs_fh);
    memcpy(buf,file->shmbuf,sizeof(struct stat));
    free(func);
  }
  else res = -EBADF;
  errno = res<0?-res:0;
  return res<0?-1:res;
}

/**
 * Gets informations about a filesystem
 *  @param path File included in FS of that information is wanted
 *  @param buf Pointer to statvfs structure

 */
int statvfs(const char *path,struct statvfs *buf) {
  int res = 0;
  void *shmbuf;
  char *func;
  char *cpath = getabsolutepath(path);
  struct fslist_item *fs = fsbypath(cpath,0);

  if (fs!=NULL) {
    int shmid = shmget(IPC_PRIVATE,SHMMEM_SIZE,0);
    shmbuf = shmat(shmid,NULL,0);
    if (shmbuf!=NULL) {
      asprintf(&func,"fs_statvfs_%x",fs->pid);
      memset(buf,0,sizeof(struct statvfs));
      strncpy(shmbuf,cpath,SHMMEM_SIZE);
      res = rpc_call(func,0,fs->id,shmid);
      memcpy(buf,shmbuf,sizeof(struct statvfs));
      free(func);
      shmdt(shmbuf);
      shmctl(shmid,IPC_RMID,NULL);
    }
    else res = -errno;
  }
  else res = -EBADF;
  free(cpath);
  errno = res<0?-res:0;
  return res<0?-1:res;
}

/**
 * Reads link
 *  @param path Path to link
 *  @param buf Buffer for path
 *  @param bufsize Size of buffer
 *  @return Bytes written to buffer
 */
ssize_t readlink(const char *path,char *buf,size_t bufsize) {
  int res = 0;
  void *shmbuf;
  char *func;
  char *cpath = getabsolutepath(path);
  struct fslist_item *fs = fsbypath(cpath,0);

  if (fs!=NULL) {
    int shmid = shmget(IPC_PRIVATE,SHMMEM_SIZE,0);
    shmbuf = shmat(shmid,NULL,0);
    if (shmbuf!=NULL) {
      asprintf(&func,"fs_readlink_%x",fs->pid);
      strncpy(shmbuf,cpath,SHMMEM_SIZE);
      res = rpc_call(func,0,fs->id,shmid,bufsize);
      strncpy(buf,shmbuf,bufsize);
      free(func);
      shmdt(shmbuf);
      shmctl(shmid,IPC_RMID,NULL);
    }
    else res = -errno;
  }
  else res = -EBADF;
  free(cpath);
  errno = res<0?-res:0;
  return res<0?-1:res;
}

/**
 * Creates a symoblic link
 *  @param src Source file
 *  @param dest Destination (name of symlink)
 *  @return 0=success; -1=failure
 */
int symlink(const char *dest,const char *src) {
  int res;
  char *csrc = getabsolutepath((char*)src);
  char *cdest = getabsolutepath((char*)dest);
  struct fslist_item *fs = fsbypath(csrc,1);
  char *func;

  if (fs!=NULL) {
    asprintf(&func,"fs_symlink_%x",fs->pid);
    res = rpc_call(func,0,fs->id,csrc,cdest);
    free(func);
  }
  else res = -EINVAL;
  errno = res<0?-res:0;
  return res<0?-1:res;
}

/**
 * Creates a hardlink
 *  @param src Source file
 *  @param dest Destination (name of hardlink)
 *  @return 0=success; -1=failure
 */
int link(const char *src,const char *dest) {
  int res;
  char *csrc = getabsolutepath((char*)src);
  char *cdest = getabsolutepath((char*)dest);
  struct fslist_item *fs = fsbypath(csrc,1);
  struct fslist_item *fsdest = fsbypath(cdest,0);
  char *func;

  if (fs!=NULL && fs==fsdest) {
    asprintf(&func,"fs_link_%x",fs->pid);
    res = rpc_call(func,0,fs->id,src,dest);
    free(func);
  }
  else res = -EINVAL;
  errno = res<0?-res:0;
  return res<0?-1:res;
}

int chown(const char *path,uid_t uid,gid_t gid) {
  int res;
  char *cpath = getabsolutepath((char*)path);
  struct fslist_item *fs = fsbypath(cpath,0);
  char *func;

  if (fs!=NULL) {
    asprintf(&func,"fs_chown_%x",fs->pid);
    res = rpc_call(func,0,fs->id,cpath,uid,gid);
    free(func);
  }
  else res = -EINVAL;
  free(cpath);
  errno = res<0?-res:0;
  return res<0?-1:res;
}

int fchown(int fildes,uid_t uid,gid_t gid) {
  int res;
  struct filelist_item *file = filebyfh(fildes);
  if (file) res = chown(file->path,uid,gid);
  else res = -EBADF;
  errno = res<0?-res:0;
  return res<0?-1:res;
}

int chmod(const char *path,mode_t mode) {
  int res;
  char *cpath = getabsolutepath((char*)path);
  struct fslist_item *fs = fsbypath(cpath,0);
  char *func;

  if (fs!=NULL) {
    asprintf(&func,"fs_chmod_%x",fs->pid);
    res = rpc_call(func,0,fs->id,cpath,mode);
    free(func);
  }
  else res = -EINVAL;
  free(cpath);
  errno = res<0?-res:0;
  return res<0?-1:res;
}

int fchmod(int fildes,mode_t mode) {
  int res;
  struct filelist_item *file = filebyfh(fildes);
  if (file) {
    res = chmod(file->path,mode);
    if (res==0) file->mode = mode;
  }
  else res = -EBADF;
  errno = res<0?-res:0;
  return res<0?-1:res;
}

int utime(const char *path,const struct utimbuf *times) {
  int res = 0;
  void *shmbuf;
  char *func;
  char *cpath = getabsolutepath(path);
  struct fslist_item *fs = fsbypath(cpath,0);

  if (fs!=NULL) {
    int shmid = shmget(IPC_PRIVATE,SHMMEM_SIZE,0);
    shmbuf = shmat(shmid,NULL,0);
    if (shmbuf!=NULL) {
      asprintf(&func,"fs_utime_%x",fs->pid);
      strncpy(shmbuf,path,SHMMEM_SIZE);
      memcpy(shmbuf+strlen(shmbuf)+1,times,sizeof(struct utimbuf));
      res = rpc_call(func,0,fs->id,shmid);
      free(func);
      shmdt(shmbuf);
      shmctl(shmid,IPC_RMID,NULL);
    }
    else res = -errno;
  }
  else res = -EBADF;
  free(cpath);
  errno = res<0?-res:0;
  return res<0?-1:res;
}

mode_t umask(mode_t cmask) {
  mode_t ret = creation_mask;
  creation_mask = cmask&0777;
  return ret;
}

///////// SOCKET STUFF /////////////////////

int socket(int domain,int type, int protocol) {
  return -1;
}
