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

#ifndef _CDI_DMA_H_
#define _CDI_DMA_H_

#include <sys/types.h>
#include <dma.h>
#include <string.h>
#include <stdint.h>

// Geraet   => Speicher
#define CDI_DMA_MODE_READ           DMA_MODE_READ
// Speicher => Geraet
#define CDI_DMA_MODE_WRITE          DMA_MODE_WRITE
#define CDI_DMA_MODE_ON_DEMAND      DMA_MODE_ON_DEMAND
#define CDI_DMA_MODE_SINGLE         DMA_MODE_SINGLE
#define CDI_DMA_MODE_BLOCK          DMA_MODE_BLOCK

struct cdi_dma_handle {
  uint8_t channel;
  size_t length;
  uint8_t mode;
  void *buffer;
  void *dmabuf; ///< @note meinOS specific
};

int cdi_dma_read(struct cdi_dma_handle *handle);
int cdi_dma_write(struct cdi_dma_handle *handle);
int cdi_dma_close(struct cdi_dma_handle *handle);

int cdi_dma_open(struct cdi_dma_handle *handle,uint8_t channel,uint8_t mode,size_t length,void* buffer);

#endif
