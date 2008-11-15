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
#include <stdint.h>
#include <dma.h>
#include <cdi/dma.h>

int cdi_dma_open(struct cdi_dma_handle* handle,uint8_t channel,uint8_t mode,size_t length,void* buffer) {
  if (dma_isready(channel) && length<=DMA_MAXCOUNT) {
    handle->dmabuf = dma_alloc(length);
    if (handle->dmabuf!=NULL) {
      handle->channel = channel;
      handle->mode = mode;
      handle->length = length;
      handle->buffer = buffer;
      return dma_start(channel,handle->dmabuf,length,mode);
    }
  }
  return -1;
}

int cdi_dma_close(struct cdi_dma_handle *handle) {
  if (dma_stop(handle->channel)==-1) return -1;
  return dma_free(handle->dmabuf,handle->length);
}

int cdi_dma_read(struct cdi_dma_handle *handle) {
  memcpy(handle->buffer,handle->dmabuf,handle->length);
  return 0;
}

int cdi_dma_write(struct cdi_dma_handle *handle) {
  memcpy(handle->dmabuf,handle->buffer,handle->length);
  return 0;
}
