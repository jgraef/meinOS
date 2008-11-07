#include <sys/types.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <pack.h>

pack_t pack_create(void *buf) {
  pack_t pack = malloc(sizeof(struct pack_S));
  pack->buf = buf;
  pack->cur = buf;
  return pack;
}

// Pack

void packstr(pack_t pack,const char *str) {
  strcpy(pack->cur,str);
  pack->cur += strlen(str)+1;
}

void packnstr(pack_t pack,const char *str,size_t maxlen) {
  strncpy(pack->cur,str,maxlen);
  pack->cur += strlen(pack->cur)+1;
}

void packdata(pack_t pack,void *data,size_t size) {
  packg(pack,size_t,size);
  memcpy(pack->cur,data,size);
  pack->cur += size;
}

// Unpack

void unpackstr(pack_t pack,char **str) {
  *str = pack->cur;
  pack->cur += strlen(pack->cur)+1;
}

void unpackdata(pack_t pack,void *data) {
  size_t size;
  unpackg(pack,size_t,&size);
  memcpy(data,pack->cur,size);
  pack->cur += size;
}
