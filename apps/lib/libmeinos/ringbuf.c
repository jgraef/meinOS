#include <ringbuf.h>
#include <stdlib.h>
#include <sys/types.h>

ringbuf_t *ringbuf_create(size_t size) {
  ringbuf_t *ringbuf = malloc(sizeof(ringbuf_t));
  ringbuf->buf = malloc(size);
  ringbuf->size = size;
  ringbuf->wpos = 0;
  ringbuf->rpos = 0;
  return ringbuf;
}

void ringbuf_destroy(ringbuf_t *ringbuf) {
  free(ringbuf->buf);
  free(ringbuf);
}

size_t ringbuf_read(ringbuf_t *ringbuf,void *buf,size_t size) {
  // calculate free size
  size_t size_free = ringbuf->wpos>=ringbuf->rpos?ringbuf->wpos-ringbuf->rpos:ringbuf->wpos+ringbuf->size-ringbuf->rpos;

  // check if buffer is big enough
  if (size>size_free) size = size_free;
  if (size==0) return 0;

  // if isn't wrapped around at end just copy
  /// @todo Check this line
  if (ringbuf->rpos+size<ringbuf->size) memcpy(buf,ringbuf->buf+ringbuf->rpos,size);
  // else you have to do 2 copies
  else {
    size_t part1 = ringbuf->size-ringbuf->rpos;
    size_t part2 = size-part1;
    memcpy(buf,ringbuf->buf+ringbuf->rpos,part1);
    memcpy(buf+part1,ringbuf->buf,part2);
  }

  // update read position
  ringbuf->rpos = (ringbuf->rpos+size)%ringbuf->size;

  return size;
}

size_t ringbuf_write(ringbuf_t *ringbuf,const void *buf,size_t size) {
  // calculate free size
  size_t size_free = ringbuf->rpos>ringbuf->wpos?ringbuf->rpos-ringbuf->wpos:ringbuf->rpos+ringbuf->size-ringbuf->rpos;

  // check if buffer is big enough
  if (size>size_free) size = size_free;
  if (size==0) return 0;

  // if isn't be wrapped around at end just copy
  /// @todo Check this line
  if (ringbuf->rpos+size<ringbuf->size) memcpy(ringbuf->buf+ringbuf->wpos,buf,size);
  // else you have to do 2 copies
  else {
    size_t part1 = ringbuf->size-ringbuf->wpos;
    size_t part2 = size-part1;
    memcpy(ringbuf->buf+ringbuf->wpos,buf,part1);
    memcpy(ringbuf->buf,buf+part1,part2);
  }

  // update read position
  ringbuf->wpos = (ringbuf->wpos+size)%ringbuf->size;

  return size;
}
