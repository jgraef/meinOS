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
