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
#include <sys/shm.h>
#include <limits.h>
#include <path.h>
#include <time.h>
#include <stdio.h>
#include <pack.h>

#include <fcntl.h>       // open,close
#include <unistd.h>      // read,write,seek,truncate
#include <dirent.h>      // *dir
#include <sys/stat.h>    // stat
#include <sys/statvfs.h> // statvfs
#include <utime.h>       // utime
#include <termios.h>     // terminal stuff

#include <sys/socket.h>  // Socket stuff

#define getnew_fh() (nextfh++)

#define SHMMEM_SIZE PAGE_SIZE

#define FLAG_ISSET(x,f) ((x)&(f))
#define FLAG_SET(x,f)   do { (x) |= (f); } while (0)
#define FLAG_UNSET(x,f) do { (x) &= ~(f); } while (0)

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
  int fh;
  char *path;
  int shmid;
  void *shmbuf;
  /// @todo Check for right file types (e.g. in read()/write()) and set them in open()/opendir()/socket()
  enum { FL_FILE,FL_DIR,FL_SOCK } type;
  int fdfl;
  int stfl;
  int isatty;
};

static llist_t filelist;
static llist_t fslist;
static int nextfh;
static struct {
  char *str;
  size_t strlen;
  path_t *path;
} workdir;
static mode_t creation_mask;
struct process_data *_process_data;

/**
 * Initializes File I/O
 */
void _fs_init() {
  filelist = llist_create();
  fslist = llist_create();
  creation_mask = 0777;
  nextfh = 3;

  // Init current workdir
  memset(&workdir,0,sizeof(workdir));
  chdir("/");
}

struct fslist_item *fsbyid(id_t fsid) {
  struct fslist_item *fs;
  size_t i;
  for (i=0;(fs = llist_get(fslist,i));i++) {
    if (fs->id==fsid) return fs;
  }
  return NULL;
}

/**
 * Pack filehandles
 *  @param buf Buffer
 *  @return If buf is NULL return needed size
 */
size_t _fs_pack_filehandles(pack_t buf) {
  struct filelist_item *file;
  struct fslist_item *fs;
  size_t i;

  if (buf==NULL) {
    // Prepare filesystems
    size_t size = sizeof(size_t)+llist_size(fslist)*sizeof(int)*2; // id, pid
    for (i=0;(fs = llist_get(fslist,i));i++) size += strlen(fs->mountpoint_str)+1; // mountpoint

    // Prepare files
    size += sizeof(size_t)+llist_size(filelist)*sizeof(int)*8; // fh, fs_fh, stfl, fdfl, shmid, type, mode
    for (i=0;(file = llist_get(filelist,i));i++) size += strlen(file->path)+1; // path

    return size;
  }
  else {
    // Pack filesystems
    packi(buf,llist_size(fslist));
    for (i=0;(fs = llist_get(fslist,i));i++) {
      packi(buf,fs->id);
      packi(buf,fs->pid);
      packstr(buf,fs->mountpoint_str);
    }

    // Pack files
    packi(buf,llist_size(filelist));
    for (i=0;(file = llist_get(filelist,i));i++) {
      if (file->type!=FL_DIR && !FLAG_ISSET(file->fdfl,FD_CLOEXEC)) {
        packi(buf,file->fh);
        packi(buf,file->fs_fh);
        packi(buf,file->shmid);
        packi(buf,file->type);
        packi(buf,file->mode);
        packi(buf,file->stfl);
        packi(buf,file->fdfl);
        packi(buf,file->fs->id);
        packstr(buf,file->path);
      }
    }

    return 0;
  }
}

/**
 * Unpack filehandles
 *  @param buf Buffer
 *  @note This function should be called before any other file function (except _fs_init())
 */
void _fs_unpack_filehandles(pack_t buf) {
  struct filelist_item *file;
  struct fslist_item *fs;
  size_t i,files_num,fs_num;
  char *tmp;
  id_t id;

  // Unpack filesystems
  unpacki(buf,&fs_num);
  for (i=0;i<fs_num;i++) {
    fs = malloc(sizeof(struct fslist_item));
    unpacki(buf,&(fs->id));
    unpacki(buf,&(fs->pid));
    unpackstr(buf,&tmp);
    fs->mountpoint = path_parse(tmp);
    fs->mountpoint_str = strdup(tmp);
    llist_push(fslist,fs);
  }

  // Unpack files
  unpacki(buf,&files_num);
  for (i=0;i<files_num;i++) {
    file = malloc(sizeof(struct filelist_item));
    unpacki(buf,&(file->fh));
    unpacki(buf,&(file->fs_fh));
    unpacki(buf,&(file->shmid));
    unpacki(buf,&(file->type));
    unpacki(buf,&(file->mode));
    unpacki(buf,&(file->stfl));
    unpacki(buf,&(file->fdfl));
    unpacki(buf,&id);
    unpackstr(buf,&(file->path));
    file->shmbuf = shmat(file->shmid,NULL,0);
    file->fs = fsbyid(id);
    llist_push(filelist,file);
  }
}

/**
 * Forks filehandles for child
 *  @param pid PID of Child
 *  @todo Implement
 */
void _fs_fork_filehandles(pid_t pid) {

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
  if (path==NULL || path[0]==0) return NULL;
  struct fslist_item *fs = mp_match(path,parent);
  if (fs==NULL) {
    fs = malloc(sizeof(struct fslist_item));
    char *mountpoint = strdup(path);
    fs->id = rpc_call("vfs_getfsid",RPC_FLAG_RETPARAMS,path,parent);
    if (fs->id<0) {
      free(mountpoint);
      errno = -fs->id;
      return NULL;
    }
    fs->pid = rpc_call("vfs_getpid",0,fs->id);
    if (fs->pid<0) {
      errno = -fs->pid;
      return NULL;
    }
    mountpoint[strlen(mountpoint)-strlen(path)] = 0;
    fs->mountpoint = path_parse(mountpoint);
    fs->mountpoint_str = path_output(fs->mountpoint,NULL);
    free(mountpoint);
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

static struct filelist_item *filebyfh(int fh) {
  if (fh>=0) {
    int idx = lidxbyfh(fh);
    if (idx>=0) return llist_get(filelist,idx);
  }
  return NULL;
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
 * File control
 *  @param fildes File descriptor
 *  @param cmd Command
 *  @param ... Optional parameter
 */
int fcntl(int fildes, int cmd, ...) {
  int ret;
  va_list args;
  struct filelist_item *file = filebyfh(fildes);
  va_start(args,cmd);
  if (file!=NULL) {
    if (cmd==F_SETFD) {
      int arg = va_arg(args,int);
      FLAG_SET(file->fdfl,arg);
      ret = 0;
    }
    else if (cmd==F_SETFL) {
      int arg = va_arg(args,int);
      FLAG_SET(file->stfl,arg);
      ret = 0;
    }
  }
  else {
    errno = EBADF;
    ret = -1;
  }
  va_end(args);
  return ret;
}

/**
 * Creates an anonymous pipe
 *  @param fildes File descriptors for in and out
 *  @return Success?
 */
int pipe(int fildes[2]) {
  char *path;

  // generate path from clock() and pid
  asprintf(&path,"/tmp/anonpipe_%d_%d",clock(),getpid());

  // open pipe
  fildes[0] = open(path,O_RDONLY|O_CREAT,S_IFIFO|0777);
  if (fildes[0]==-1) return -1;
  fildes[1] = open(path,O_WRONLY);
  if (fildes[1]==-1) return -1;

  // set flags
  fcntl(fildes[0],F_SETFD,FD_CLOEXEC);
  fcntl(fildes[0],F_SETFL,O_NONBLOCK);

  return 0;
}

/**
 * Gets current workdir
 *  @param buf Buffer to store path in
 *  @param size Size of buffer
 *  @return Current workdir
 */
char *getcwd(char *buf,size_t size) {
  if (buf==NULL) {
    size = workdir.strlen+1;
    buf = malloc(size);
  }
  else if (size==0) {
    errno = EINVAL;
    return NULL;
  }
  else if (workdir.strlen+1>size) {
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

  DIR *dir = opendir(new);
  if (dir!=NULL) {
    closedir(dir);
    free(workdir.str);
    path_destroy(workdir.path);
    workdir.path = path_parse(new);
    path_reject_dots(workdir.path);
    workdir.str = path_output(workdir.path,NULL);
    workdir.strlen = strlen(workdir.str);
    setenv("PATH",workdir.str,1);
    return 0;
  }
  else return -1;
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

  if (fs!=NULL) res = rpc_call("fs_mknod",RPC_FLAG_SENDTO,fs->pid,fs->id,cpath,mode&(~creation_mask),dev);
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

  if (fs!=NULL) res = rpc_call("fs_access",RPC_FLAG_SENDTO,fs->pid,fs->id,cpath,amode);
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
      fh = rpc_call("fs_open",RPC_FLAG_SENDTO,fs->pid,fs->id,oflag,shmid);
      if (fh>=0) {
        struct filelist_item *new = malloc(sizeof(struct filelist_item));
        memset(new,0,sizeof(struct filelist_item));
        new->fs_fh = fh;
        new->fs = fs;
        new->stfl = oflag;
        new->mode = S_IFREG;
        new->fh = getnew_fh();
        new->path = strdup(cpath);
        new->shmid = shmid;
        new->shmbuf = shmbuf;
        new->type = FL_FILE;
        new->isatty = (strncmp(path,"/dev/tty",8)==0);
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
    if (file->type==FL_FILE) close(file->fh);
    else if (file->type==FL_DIR) closedir(file);
  }
}

/**
 * Closes all filehandles that must be closed on exec*()
 */
void _fs_cloexec() {
  struct filelist_item *file;
  size_t i;
  for (i=0;(file = llist_get(filelist,i));i++) {
    if (file->type==FL_DIR) closedir(file);
    else if (FLAG_ISSET(file->fdfl,FD_CLOEXEC)) close(file->fh);
  }
}

/**
 * Closes a file
 *  @param fildes File descriptor
 *  @return 0=success; -1=failure
 */
int close(int fildes) {
  int res;
  struct filelist_item *file = filebyfh(fildes);
  if (file) {
    res = rpc_call("fs_close",RPC_FLAG_SENDTO,file->fs->pid,file->fs->id,file->fs_fh);
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
  struct filelist_item *file = filebyfh(fildes);
  if (file) {
    res = rpc_call("fs_dup",RPC_FLAG_SENDTO,file->fs->pid,file->fs->id,file->fs_fh);
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
  ssize_t res = rpc_call("fs_read",RPC_FLAG_SENDTO,file->fs->pid,file->fs->id,file->fs_fh,count);
  if (res>0) memcpy(buf,file->shmbuf,res>SHMMEM_SIZE?SHMMEM_SIZE:res);
  return res;
}
ssize_t read(int fildes,void *buf,size_t count) {
  struct filelist_item *file = filebyfh(fildes);
  if (file!=NULL) {
    size_t pos = 0;

    while (pos<count) {
      size_t count_cur = SHMMEM_SIZE;
      if (count-pos<SHMMEM_SIZE) count_cur = count-pos;

      count_cur = _read(file,buf+pos,count_cur);

      if (count_cur==-1) return -1;
      if (count_cur==0) break;

      pos += count_cur;
    }

    return pos;
  }
  else {
    errno = EBADF;
    return -1;
  }
}

/**
 * Reads from a file at a given offset
 *  @param fildes File descriptor
 *  @param buf Buffer to store read data in
 *  @param count How many bytes to read
 *  @param offset Offset to begin reading at
 *  @return How many bytes read
 */
ssize_t pread(int fildes,void *buf,size_t count,off_t offset) {
  int ret;
  off_t oldoff;

  oldoff = lseek(fildes,0,SEEK_CUR);
  lseek(fildes,offset,SEEK_SET);
  ret = read(fildes,buf,count);
  lseek(fildes,oldoff,SEEK_SET);

  return ret;
}

/**
 * Writes to a file
 *  @param fildes File descriptor
 *  @param buf Data to write to file
 *  @param count How many bytes to write
 *  @return How many bytes written
 */
static ssize_t _write(struct filelist_item *file,const void *buf,size_t count) {
  ssize_t res;
  memcpy(file->shmbuf,buf,count);
  res = rpc_call("fs_write",RPC_FLAG_SENDTO,file->fs->pid,file->fs->id,file->fs_fh,count);
  return res;
}
ssize_t write(int fildes,const void *buf,size_t count) {
  struct filelist_item *file = filebyfh(fildes);
  if (file!=NULL) {
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
 * Writes to a file at a given offset
 *  @param fildes File descriptor
 *  @param buf Data to write to file
 *  @param count How many bytes to write
 *  @param offset Offset to begin writing at
 *  @return How many bytes written
 */
ssize_t pwrite(int fildes,const void *buf,size_t count,off_t offset) {
  int ret;
  off_t oldoff;

  oldoff = lseek(fildes,0,SEEK_CUR);
  lseek(fildes,offset,SEEK_SET);
  ret = write(fildes,buf,count);
  lseek(fildes,oldoff,SEEK_SET);

  return ret;
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
  struct filelist_item *file = filebyfh(fildes);
  if (file!=NULL) res = rpc_call("fs_seek",RPC_FLAG_SENDTO,file->fs->pid,file->fs->id,file->fs_fh,offset,whence);
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

  if (fs!=NULL) res = rpc_call("fs_unlink",RPC_FLAG_SENDTO,fs->pid,fs->id,cpath);
  else res = -EINVAL;
  free(cpath);
  errno = res<0?-res:0;
  return res<0?-1:res;
}

/**
 * Removes a directory
 *  @param path Path to directory
 *  @return 0=success; -1=failure
 *  @todo does this need an own RPC function?
 */
int rmdir(const char *path) {
  int res;
  char *cpath = getabsolutepath((char*)path);
  struct fslist_item *fs = fsbypath(cpath,1);

  if (fs!=NULL) res = rpc_call("fs_rmdir",RPC_FLAG_SENDTO,fs->pid,fs->id,cpath);
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

  if (fs!=NULL && fs==fsnew) res = rpc_call("fs_rename",RPC_FLAG_SENDTO,fs->pid,fs->id,old,new);
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
  struct filelist_item *file = filebyfh(fildes);
  if (file) res = rpc_call("fs_ftruncate",RPC_FLAG_SENDTO,file->fs->pid,file->fs->id,file->fs_fh,length);
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
  char *cpath = getabsolutepath(path);
  struct fslist_item *fs = fsbypath(cpath,0);

  struct filelist_item *new;
  if (fs!=NULL) {
    int shmid = shmget(IPC_PRIVATE,SHMMEM_SIZE,0);
    shmbuf = shmat(shmid,NULL,0);
    if (shmbuf!=NULL) {
      strncpy(shmbuf,cpath,SHMMEM_SIZE);
      dh = rpc_call("fs_opendir",RPC_FLAG_SENDTO,fs->pid,fs->id,shmid);
      if (dh>=0) {
        new = malloc(sizeof(struct filelist_item));
        memset(new,0,sizeof(struct filelist_item));
        new->fs_fh = dh;
        new->fs = fs;
        new->mode = S_IFDIR;
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
  if (file) {
    res = rpc_call("fs_closedir",RPC_FLAG_SENDTO,file->fs->pid,file->fs->id,file->fs_fh);
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
  if (file!=NULL) {
    res = rpc_call("fs_readdir",RPC_FLAG_SENDTO,file->fs->pid,file->fs->id,file->fs_fh);
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
  if (file) res = rpc_call("fs_seekdir",RPC_FLAG_SENDTO,file->fs->pid,file->fs->id,file->fs_fh,loc);
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
  if (file) res = rpc_call("fs_seekdir",RPC_FLAG_SENDTO,file->fs->pid,file->fs->id,file->fs_fh,-1);
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
  struct filelist_item *file = filebyfh(fildes);
  if (file) {
    res = rpc_call("fs_fstat",RPC_FLAG_SENDTO,file->fs->pid,file->fs->id,file->fs_fh);
    memcpy(buf,file->shmbuf,sizeof(struct stat));
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
  char *cpath = getabsolutepath(path);
  struct fslist_item *fs = fsbypath(cpath,0);

  if (fs!=NULL) {
    int shmid = shmget(IPC_PRIVATE,SHMMEM_SIZE,0);
    shmbuf = shmat(shmid,NULL,0);
    if (shmbuf!=NULL) {
      memset(buf,0,sizeof(struct statvfs));
      strncpy(shmbuf,cpath,SHMMEM_SIZE);
      res = rpc_call("stat_vfs",RPC_FLAG_SENDTO,fs->pid,fs->id,shmid);
      memcpy(buf,shmbuf,sizeof(struct statvfs));
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
  char *cpath = getabsolutepath(path);
  struct fslist_item *fs = fsbypath(cpath,0);

  if (fs!=NULL) {
    int shmid = shmget(IPC_PRIVATE,SHMMEM_SIZE,0);
    shmbuf = shmat(shmid,NULL,0);
    if (shmbuf!=NULL) {
      strncpy(shmbuf,cpath,SHMMEM_SIZE);
      res = rpc_call("fs_readlink",RPC_FLAG_SENDTO,fs->pid,fs->id,shmid,bufsize);
      strncpy(buf,shmbuf,bufsize);
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

  if (fs!=NULL) res = rpc_call("fs_symlink",RPC_FLAG_SENDTO,fs->pid,fs->id,csrc,cdest);
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

  if (fs!=NULL && fs==fsdest) res = rpc_call("fs_link",RPC_FLAG_SENDTO,fs->pid,fs->id,src,dest);
  else res = -EINVAL;
  errno = res<0?-res:0;
  return res<0?-1:res;
}

int chown(const char *path,uid_t uid,gid_t gid) {
  int res;
  char *cpath = getabsolutepath((char*)path);
  struct fslist_item *fs = fsbypath(cpath,0);

  if (fs!=NULL) res = rpc_call("fs_chown",RPC_FLAG_SENDTO,fs->pid,fs->id,cpath,uid,gid);
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

  if (fs!=NULL) res = rpc_call("fs_chmod",RPC_FLAG_SENDTO,fs->pid,fs->id,cpath,mode);
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
  char *cpath = getabsolutepath(path);
  struct fslist_item *fs = fsbypath(cpath,0);

  if (fs!=NULL) {
    int shmid = shmget(IPC_PRIVATE,SHMMEM_SIZE,0);
    shmbuf = shmat(shmid,NULL,0);
    if (shmbuf!=NULL) {
      strncpy(shmbuf,path,SHMMEM_SIZE);
      memcpy(shmbuf+strlen(shmbuf)+1,times,sizeof(struct utimbuf));
      res = rpc_call("fs_utime",RPC_FLAG_SENDTO,fs->pid,fs->id,shmid);
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

// Terminal stuff

int tcgetattr(int fildes,struct termios *termios_p) {
  int res;
  struct filelist_item *file = filebyfh(fildes);
  if (file) {
    res = rpc_call("fs_tcgetattr",RPC_FLAG_SENDTO,file->fs->pid,file->fs->id,file->fs_fh);
    memcpy(termios_p,file->shmbuf,sizeof(struct termios));
  }
  else res = -EBADF;
  errno = res<0?-res:0;
  return res<0?-1:res;
}

int tcsetattr(int fildes,int optional_actions,const struct termios *termios_p) {
  int res;
  struct filelist_item *file = filebyfh(fildes);
  if (file) {
    memcpy(file->shmbuf,termios_p,sizeof(struct termios));
    res = rpc_call("fs_tcsetattr",RPC_FLAG_SENDTO,file->fs->pid,file->fs->id,file->fs_fh);
  }
  else res = -EBADF;
  errno = res<0?-res:0;
  return res<0?-1:res;
}

char *ttyname(int fildes) {
  struct filelist_item *file = filebyfh(fildes);
  if (file) {
    if (file->isatty) return file->path;
    else errno = ENOTTY;
  }
  else errno = EBADF;
  return NULL;
}

int isatty(int fildes) {
  int res;
  struct filelist_item *file = filebyfh(fildes);
  if (file) return file->isatty;
  else res = -EBADF;
  errno = res<0?-res:0;
  return res<0?-1:res;
}

///////// SOCKET STUFF /////////////////////

int socket(int domain,int type, int protocol) {
  return -1;
}
