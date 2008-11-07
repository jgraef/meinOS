#ifndef _ARPA_INET_H_
#define _ARPA_INET_H_

#include <netinet/in.h>
#include <stdint.h>
#include <unistd.h>

static inline uint32_t htonl(uint32_t v) {
  uint32_t r;
  swab(&v,&r,4);
  return r;
}

static inline uint16_t htons(uint16_t v) {
  uint16_t r;
  swab(&v,&r,2);
  return r;
}

static inline uint32_t ntohl(uint32_t v) {
  uint32_t r;
  swab(&v,&r,4);
  return r;
}

static inline uint16_t ntohs(uint16_t v) {
  uint16_t r;
  swab(&v,&r,2);
  return r;
}

in_addr_t inet_addr(const char *str);
char *inet_ntoa(struct in_addr addr);
const char *inet_ntop(int af,const void *cp,char *buf,socklen_t len);
int inet_pton(int af,const char *src,void *dst);

#endif
