#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/socket.h>

const struct in6_addr in6addr_any = { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } };
const struct in6_addr in6addr_loopback = { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 } };

int inet_aton(const char *str, struct in_addr *addr) {
  union {
    uint8_t b[4];
    uint32_t l;
  } a;

  if (sscanf(str,"%hhu.%hhu.%hhu.%hhu",&a.b[0],&a.b[1],&a.b[2],&a.b[3])==4) {
    addr->s_addr = a.l;	/* Always in network byte order */
    return 1;
  }
  else return 0;
}

in_addr_t inet_addr(const char *str) {
  struct in_addr a;
  int rv = inet_aton(str,&a);
  return rv?INADDR_NONE:a.s_addr;
}

char *inet_ntoa(struct in_addr addr) {
  static char name[INET_ADDRSTRLEN];
  const uint8_t *cp = (const uint8_t*)&addr.s_addr;
  sprintf(name,"%u.%u.%u.%u",cp[0],cp[1],cp[2],cp[3]);
  return name;
}

const char *inet_ntop(int af,const void *cp,char *buf,socklen_t len) {
  size_t xlen;

  switch (af) {
    case AF_INET: {
      const uint8_t *bp = (const uint8_t*)&((const struct in_addr*)cp)->s_addr;
      xlen = snprintf(buf,len,"%u.%u.%u.%u",bp[0],bp[1],bp[2],bp[3]);
      break;
    }
    case AF_INET6: {
      struct in6_addr *s = (struct in6_addr *)cp;
      uint16_t *addr16 = (uint16_t*)&(s->s6_addr);
      xlen = snprintf(buf,len,"%x:%x:%x:%x:%x:%x:%x:%x",ntohs(addr16[0]),ntohs(addr16[1]),ntohs(addr16[2]),ntohs(addr16[3]),ntohs(addr16[4]),ntohs(addr16[5]),ntohs(addr16[6]),ntohs(addr16[7]));
      memcpy(s->s6_addr,addr16,sizeof(addr16));
      break;
    }
    default: {
      errno = EAFNOSUPPORT;
      return NULL;
    }
  }

  if (xlen>len) {
    errno = ENOSPC;
    return NULL;
  }

  return buf;
}

static inline int hexval(int ch) {
  if (ch>='0' && ch<='9') return ch-'0';
  else if (ch>='A' && ch<='F') return ch-'A'+10;
  else if (ch>='a' && ch<='f') return ch-'a'+10;
  else return -1;
}

int inet_pton(int af,const char *src,void *dst) {
  switch (af) {
    case AF_INET: {
      return inet_aton(src, (struct in_addr *)dst);
    }
    case AF_INET6: {
      struct in6_addr *d = (struct in6_addr *)dst;
      int colons = 0;
      int dcolons = 0;
      int i;
      const char *p;

      /* A double colon will increment colons by 2,
         dcolons by 1 */
      for (p=dst;*p;p++) {
        if (p[0] == ':') {
          colons++;
          if (p[1] == ':') dcolons++;
        }
        else if (!isxdigit(*p)) return 0; /* Invalid address */
      }

      if (colons>7 || dcolons>1 || (!dcolons && colons!=7)) return 0; /* Invalid address */

      memset(d, 0, sizeof(struct in6_addr));

      i = 0;
      for (p=dst;*p;p++) {
        if (*p==':') {
          if (p[1]==':') i += (8-colons);
          else i++;
        }
        else {
          uint16_t *addr16 = (uint16_t*)&(d->s6_addr);
          addr16[i] = htons((ntohs(addr16[i])<<4)+hexval(*p));
        }
      }
      return 1;
    }
    default: {
      errno = EAFNOSUPPORT;
      return -1;
    }
  }
}
