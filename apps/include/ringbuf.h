#ifndef _RINGBUF_H_
#define _RINGBUF_H_

#include <sys/types.h>

typedef struct {
  void *buf;
  size_t size;
  size_t rpos;
  size_t wpos;
} ringbuf_t;

ringbuf_t *ringbuf_create(size_t size);
void ringbuf_destroy(ringbuf_t *ringbuf);
size_t ringbuf_read(ringbuf_t *ringbuf,void *buf,size_t size);
size_t ringbuf_write(ringbuf_t *ringbuf,const void *buf,size_t size);

#endif /* _RINGBUF_H_ */
