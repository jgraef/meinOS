#ifndef _PACK_H_
#define _PACK_H_

#include <stdint.h>
#include <malloc.h>

typedef struct pack_S {
  void* cur;
  void* buf;
}* pack_t;

#define pack_destroy(pack) free(pack)
#define pack_sizeof(pack)  ((pack)->cur-(pack)->buf)
#define pack_malloc(size)  pack_create(malloc(size))
#define pack_free(pack)    do { free(pack->buf); pack_destroy(pack); } while(0)
#define pack_data(pack)    ((pack)->buf)
#define pack_reset(pack)   ((pack)->cur = (pack)->buf)

pack_t pack_create(void *buf);

// Pack

#define packg(pack,type,val) do { *((type*)((pack)->cur)) = (type)(val); (pack)->cur += sizeof(type); } while(0)
#define pack8(pack,val)      packg(pack,uint8_t,val)
#define pack16(pack,val)     packg(pack,uint16_t,val)
#define pack32(pack,val)     packg(pack,uint32_t,val)
#define pack64(pack,val)     packg(pack,uint64_t,val)
#define packc(pack,val)      packg(pack,char,val)
#define packs(pack,val)      packg(pack,short,val)
#define packi(pack,val)      packg(pack,int,val)
#define packl(pack,val)      packg(pack,long,val)
#define packobj(pack,obj)    packdata(pack,&(obj),sizeof(obj))

void packstr(pack_t pack,const char *str);
void packnstr(pack_t pack,const char *str,size_t maxlen);
void packdata(pack_t pack,void *data,size_t size);

// Unpack

#define unpackg(pack,type,val) do { *(val) = *((type*)((pack)->cur)); (pack)->cur += sizeof(type); } while(0)
#define unpack8(pack,val)      unpackg(pack,uint8_t,val)
#define unpack16(pack,val)     unpackg(pack,uint16_t,val)
#define unpack32(pack,val)     unpackg(pack,uint32_t,val)
#define unpack64(pack,val)     unpackg(pack,uint64_t,val)
#define unpackc(pack,val)      unpackg(pack,char,val)
#define unpacks(pack,val)      unpackg(pack,short,val)
#define unpacki(pack,val)      unpackg(pack,int,val)
#define unpackl(pack,val)      unpackg(pack,long,val)
#define unpackobj(pack,obj)    unpackdata(pack,&(obj));

void unpackstr(pack_t pack,char **str);
void unpackdata(pack_t pack,void *data);

#endif
