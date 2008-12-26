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

#include <stdint.h>
#include <ioport.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Turns bell off
 */
void stopbeep() {
  outb(0x61,inb(0x61)&~3);
}

/**
 * Rings the bell
 *  @param freq Frequency to use
 */
void startbeep(uint16_t freq) {
  freq = 1193180/freq;
  outb(0x43,0xB6);
  outb(0x42,(uint8_t)freq);
  outb(0x42,(uint8_t)(freq>>8));
  outb(0x61,inb(0x61)|3);
}

/**
 * Prints usage
 *  @param cmd Command used to run origran
 */
static void usage(char *prog,int ret) {
  FILE *stream = ret==0?stdout:stderr;
  fprintf(stream,"Usage: %s [OPTION]...\n",prog);
  fprintf(stream,"\t-f FREQUENCY Frequency in milliseconds\n");
  fprintf(stream,"\t-d DURATION  Duration\n");
  exit(ret);
}

/**
 * Main function
 *  @param argc Number of Parameters
 *  @param argv Parameters
 */
int main(int argc,char *argv[]) {
  int c;
  int freq = 470;
  int dur = 250;

    while ((c = getopt(argc,argv,":fdhv"))!=-1) {
    switch(c) {
      case 'f':
        freq = atoi(optarg);
        break;
      case 'd':
        dur = atoi(optarg);
        break;
      case 'h':
        usage(argv[0],0);
        break;
      case 'v':
        printf("beep v0.1\n(c) 2008 Janosch Graef\n");
        return 0;
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  ioport_reg(0x42);
  ioport_reg(0x43);
  ioport_reg(0x61);
  startbeep((uint16_t)freq);
  usleep(dur*1000);
  stopbeep();

  return 0;
}
