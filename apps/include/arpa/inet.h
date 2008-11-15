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
