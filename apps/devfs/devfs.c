#include <sys/types.h>
#include <sys/shm.h>
#include <rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <llist.h>
#include <fuse.h>

struct devlist_item {
  int id;
  char *name;
  pid_t owner;
  enum { DT_CHAR,DT_BLOCK } type;
  void *shmbuf;
};

llist_t devlist;
int nextdevid;

#define getnew_devid() (nextdevid++)

/**
 * Gets ListID by DevID
 *  @param devid DevID
 *  @return ListID
 */
int getlid_devid(int devid) {
  struct devlist_item *dev;
  int i;
  for (i=0;(dev = llist_get(devlist,i));i++) {
    if (dev->id==devid) return i;
  }
  return -1;
}

/**
 * Gets device by name
 *  @param name Device name
 *  @return Device
 */
struct devlist_item *getdev_name(char *name) {
  struct devlist_item *dev;
  int i;

  for (i=0;(dev = llist_get(devlist,i));i++) {
    if (strcmp(dev->name,name)==0) return dev;
  }
  return NULL;
}

/**
 * Opens a file
 *  @param path Path to file
 *  @param fi File Info
 *  @return Filehandle
 */
int devfs_open(const char *path,struct fuse_file_info *fi) {
  struct devlist_item *dev = getdev_name((char*)path+1);
  if (dev!=NULL) return 0;
  else return -ENOENT;
}

/**
 * Closes a file
 *  @param path Path to file
 *  @param fi File Info
 *  @return Success?
 */
int devfs_close(const char *path,struct fuse_file_info *fi) {
  return 0;
}

/**
 * Reads from file
 *  @param path Path to file
 *  @param buf Buffer to store read data in
 *  @param count How many bytes to read
 *  @param offset Offset to start at
 *  @param fi File info
 *  @return How many bytes read (-Errorcode)
 */
int devfs_read(const char *path,char *buf,size_t count,off_t offset,struct fuse_file_info *fi) {
  struct devlist_item *dev = getdev_name((char*)path+1);
  char *func;
  if (dev!=NULL) {
    asprintf(&func,"devfs_read_%x",dev->owner);
    int ret = rpc_call(func,0,dev->id,count,offset);
    memcpy(buf,dev->shmbuf,count);
    free(func);
    return ret;
  }
  else return -ENOENT;
}

/**
 * Writes to file
 *  @param path Path to file
 *  @param buf Buffer to get data from
 *  @param count How many bytes to write
 *  @param offset Offset to start at
 *  @param fi File info
 *  @return How many bytes written (-Errorcode)
 */
int devfs_write(const char *path,const char *buf,size_t count,off_t offset,struct fuse_file_info *fi) {
  struct devlist_item *dev = getdev_name((char*)path+1);
  char *func;
  if (dev!=NULL) {
    asprintf(&func,"devfs_write_%x",dev->owner);
    memcpy(dev->shmbuf,buf,count);
    int ret = rpc_call(func,0,dev->id,count,offset);
    free(func);
    return ret;
  }
  else return -ENOENT;
}

/**
 * Reads from dir
 *  @param path Path to dir
 *  @param buf Buffer for dir entries
 *  @param filler Filler function
 *  @param off Dir offset
 *  @param fi File info
 *  @return 0=success (-Errorcode)
 */
int devfs_readdir(const char *path,void *buf,fuse_fill_dir_t filler,off_t off,struct fuse_file_info *fi) {
  if (strcmp(path,"/")==0) {
    size_t i;
    struct devlist_item *dev;
    filler(buf,".",NULL,0);
    filler(buf,"..",NULL,0);
    for (i=0;(dev = llist_get(devlist,i));i++) filler(buf,dev->name,NULL,0);
    return 0;
  }
  else return -ENOENT;
}

/**
 * Gets informations about files
 *  @param path Path to file
 *  @param stbuf Buffer to store informations in
 *  @return 0=success (-Errorcode)
 */
int devfs_getattr(const char *path,struct stat *stbuf) {
  struct devlist_item *dev;
  memset(stbuf,0,sizeof(struct stat));
  if (strcmp(path,"/") == 0) {
    stbuf->st_mode = S_IFDIR|0755;
    stbuf->st_nlink = 2;
  }
  else if ((dev = getdev_name((char*)path+1))!=NULL) {
    stbuf->st_mode = (dev->type==DT_BLOCK?S_IFBLK:S_IFCHR)|0744;
    stbuf->st_nlink = 1;
    stbuf->st_size = 0;
  }
  else return -ENOENT;
  return 0;
}

/**
 * Creates a new device
 *  @param name Device name
 *  @return DevID
 */
int devfs_createdev(char *name,int shmid) {
  int devid = getnew_devid();
  if (devid>=0 && getdev_name(name)==NULL) {
    struct devlist_item *new = malloc(sizeof(struct devlist_item));
    new->id = devid;
    new->name = strdup(name);
    new->owner = rpc_curpid;
    new->shmbuf = shmat(shmid,NULL,0);
    llist_push(devlist,new);
    return devid;
  }
  else {
    fprintf(stderr,"devfs: Could not create device: %s\n",name);
    return -1;
  }
}

/**
 * Removes a device
 *  @param id DevID
 *  @return 0=success; -1=failure
 */
int devfs_removedev(int id) {
  struct devlist_item *dev = llist_get(devlist,getlid_devid(id));
  if (dev!=NULL && dev->owner==rpc_curpid) {
    llist_remove(devlist,getlid_devid(id));
    shmdt(dev->shmbuf);
    free(dev->name);
    free(dev);
    return 0;
  }
  return -1;
}

/**
 * Initializes DevFS
 *  @param argc Number of arguments
 *  @param argv Values of arguments
 *  @return Return value
 */
int main(int argc,char *argv[]) {
  devlist = llist_create();
  nextdevid = 1;

  rpc_func(devfs_createdev,"$i",NAME_MAX+sizeof(int));
  rpc_func(devfs_removedev,"i",sizeof(int));

  struct fuse_operations devfs_oper = {
    .open = devfs_open,
    .release = devfs_close,
    .read = devfs_read,
    .write = devfs_write,
    .readdir = devfs_readdir,
    .getattr = devfs_getattr
  };

  // fake argc/argv
  int fake_argc = 2;
  char *fake_argv[2] = { "devfs","/dev" };
  fuse_main(fake_argc,fake_argv,&devfs_oper,NULL);

  return 0;
}
