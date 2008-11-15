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
