#ifndef _DEVFS_H_
#define _DEVFS_H_

#include <sys/types.h>
#include <rpc.h>

typedef struct devfs_dev_S devfs_dev_t;
struct devfs_dev_S {
  char *name;        // Device name
  dev_t id;          // Device ID
  int shmid;         // SHMID
  void *shmbuf;      // SHM buffer
  ssize_t (*func_read)(devfs_dev_t *dev,void *buf,size_t count,off_t offset);
  ssize_t (*func_write)(devfs_dev_t *dev,void *buf,size_t count,off_t offset);
  void *user_data;   // User data
};

#define devfs_curpid rpc_curpid
#define devfs_mainloop() rpc_mainloop(-1)


/**
 * Sets callback function for fileread
 *  @param dev Device
 *  @param func_read Function to fileread handler
 */
static inline void devfs_onread(devfs_dev_t *dev,ssize_t (*func_read)(devfs_dev_t *dev,void *buf,size_t count,off_t offset)) {
  dev->func_read = func_read;
}

/**
 * Sets callback function for filewrite
 *  @param dev Device
 *  @param func_write Function to filewrite handler
 */
static inline void devfs_onwrite(devfs_dev_t *dev,ssize_t (*func_write)(devfs_dev_t *dev,void *buf,size_t count,off_t offset)) {
  dev->func_write = func_write;
}

void devfs_init();
devfs_dev_t *devfs_createdev(const char *name);
int devfs_removedev(devfs_dev_t *dev);

#endif
