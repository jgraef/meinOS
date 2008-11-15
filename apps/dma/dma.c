/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/types.h>
#include <ioport.h>
#include <rpc.h>
#include <string.h>

int dma_ports_page[]   = {0x87, 0x83, 0x81, 0x82, 0x8f, 0x8b, 0x89, 0x8a};
int dma_ports_addr[]   = {0x00, 0x02, 0x04, 0x06, 0xc0, 0xc4, 0xc8, 0xcc};
int dma_ports_count[]  = {0x01, 0x03, 0x05, 0x07, 0xc2, 0xc6, 0xca, 0xce};
int dma_ports_mask[]   = {0x0a, 0x0a, 0x0a, 0x0a, 0xd4, 0xd4, 0xd4, 0xd4};
int dma_ports_mode[]   = {0x0b, 0x0b, 0x0b, 0x0b, 0xd6, 0xd6, 0xd6, 0xd6};
int dma_ports_clear[]  = {0x0c, 0x0c, 0x0c, 0x0c, 0xd8, 0xd8, 0xd8, 0xd8};

struct {
  int used;
  pid_t owner;
} dma_channels[8];

void dma_init() {
  memset(dma_channels,0,sizeof(dma_channels));
}

int dma_start(int channel,void *buffer,size_t length,int mode) {
  if (dma_channels[channel].used!=0) return -1;
  dma_channels[channel].used = 1;
  dma_channels[channel].owner = rpc_curpid;

  mode = mode | (channel % 4);

  // mask channel
  outb(dma_ports_mask[channel], channel | 0x4);

  // stop all running transmitions
  outb(dma_ports_clear[channel], 0x0);

  // send mode
  outb(dma_ports_mode[channel], mode);

  // send address
  outb(dma_ports_addr[channel], ((uint32_t)(buffer) & 0xff));
  outb(dma_ports_addr[channel], (((uint32_t)(buffer) >> 8) & 0xff));
  outb(dma_ports_page[channel], (((uint32_t)(buffer) >> 16) & 0xff));

  outb(dma_ports_clear[channel], 0x0);

  // send length
  length--;
  outb(dma_ports_count[channel], (uint8_t)(length & 0xff));
  outb(dma_ports_count[channel], (uint8_t)((length >> 8) & 0xff));

  // run
  outb(dma_ports_mask[channel], channel);

  return 0;
}

int dma_stop(int channel) {
  if (dma_channels[channel].owner==rpc_curpid) {
    dma_channels[channel].used = 0;
    return 0;
  }
  else return -1;
}

int dma_isready(int channel) {
  return !dma_channels[channel].used;
}

int main(int argc,char *argv[]) {
  dma_init();
  rpc_func(dma_start,"iiii",sizeof(int)*4);
  rpc_func(dma_stop,"i",sizeof(int));
  rpc_func(dma_isready,"i",sizeof(int));
  rpc_mainloop(-1);

  return 0;
}
