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
void startbeep(u16 freq) {
  freq = 1193180/freq;
  outb(0x43,0xB6);
  outb(0x42,(u8)freq);
  outb(0x42,(u8)(freq>>8));
  outb(0x61,inb(0x61)|3);
}

/**
 * Prints usage
 *  @param cmd Command used to run origran
 */
void usage(char *cmd) {
  printf("%s [options]\n",cmd)
  printf("  -f Frequency in milliseconds\n");
  printf("  -d Duration\n");
}

/**
 * Main function
 *  @param argc Number of Parameters
 *  @param argv Parameters
 */
int main(int argc,char *argv[]) {
  int freq = 470;
  int dur = 250;

  for (i=1;i<argc;i++) {
    if (strcmp(argv[i],"-f")==0) {
      i++;
      if (i==argc) usage(argv[0]);
      freq = atoi(argv[i]);
    }
    if (strcmp(argv[i],"-d")==0) {
      i++;
      if (i==argc) usage(argv[0]);
      dur = atoi(argv[i]);
    }
  }

  ioport_reg(0x42);
  ioport_reg(0x43);
  ioport_reg(0x61);
  startbeep((u16)freq);
  usleep(dur*1000);
  stopbeep();
}
