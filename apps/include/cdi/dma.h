/*
 * Copyright (c) 2009 Janosch Gräf
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
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
