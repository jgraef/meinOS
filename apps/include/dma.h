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
