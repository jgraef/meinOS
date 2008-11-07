#ifndef _DMA_H_
#define _DMA_H_

#include <sys/types.h>
#include <rpc.h>
#include <misc.h>
#include <limits.h>
#include <syscall.h>

#define DMA_PAGE_SIZE PAGE_SIZE
#define DMA_MAXCOUNT  65536

// Device => Memory
#define DMA_MODE_READ           (1 << 2)
// Memory => Device
#define DMA_MODE_WRITE          (2 << 2)
#define DMA_MODE_ON_DEMAND      (0 << 6)
#define DMA_MODE_SINGLE         (1 << 6)
#define DMA_MODE_BLOCK          (2 << 6)

/**
 * Allocates DMA memory
 *  @param size Size of DMA memory
 *  @return DMA memory
 */
static inline void *dma_alloc(size_t size) {
  return (void*)syscall_call(SYSCALL_MEM_DMA_ALLOC,1,((size-1)/DMA_PAGE_SIZE+1)*DMA_PAGE_SIZE);
}

/**
 * Frees DMA memory
 *  @param buffer DMA memory
 *  @param size Size of DMA memory
 *  @return Success?
 */
static inline int dma_free(void *buffer,size_t size) {
  return syscall_call(SYSCALL_MEM_DMA_FREE,2,buffer,((size-1)/DMA_PAGE_SIZE+1)*DMA_PAGE_SIZE);
}

/**
 * Checks if DMA channel is ready to use
 *  @param channel DMA channel
 *  @return If channel is ready to use
 */
static inline int dma_isready(channel) {
  return rpc_call("dma_isready",0,channel);
}

/**
 * Start a DMA transfer
 *  @param channel DMA channel
 *  @param buf Buffer to read/write data from/to
 *  @param count How many bytes to transfer
 *  @param mode Transfer mode
 *  @return Success?
 */
static inline int dma_start(int channel,void *buf,size_t count,int mode) {
  return rpc_call("dma_start",0,channel,mem_getphysaddr(buf),count,mode);
}

/**
 * Stops a DMA transfer
 *  @param channel DMA channel
 *  @return Success?
 */
static inline int dma_stop(int channel) {
  return rpc_call("dma_stop",0,channel);
}

#endif
