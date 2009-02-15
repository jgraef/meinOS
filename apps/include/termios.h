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

#ifndef _TERMIOS_H_
#define _TERMIOS_H_

#define NCCS 32

// Control characters
#define VEOF   4
#define VEOL   11
#define VERASE 2
#define VINTR  0
#define VKILL  3
#define VMIN   6
#define VQUIT  1
#define VSTART 8
#define VSTOP  9
#define VSUSP  10
#define VTIME  5

// Input modes
#define IGNBRK 1
#define BRKINT 2
#define IGNPAR 4
#define PARMRK 8
#define INPCK  16
#define ISTRIP 32
#define INLCR  64
#define IGNCR  128
#define ICRNL  256
#define IXON   1024
#define IXANY  2048
#define IXOFF  4096

// Output modes
#define OPOST  1
#define ONLCR  4
#define OCRNL  8
#define ONOCR  16
#define ONLRET 32
#define OFILL  64
#define NLDLY  256
#define NL0    0
#define NL1    256
#define CRDLY  1536
#define CR0    0
#define CR1    512
#define CR2    1024
#define CR3    1536
#define TABDLY 6144
#define TAB0   0
#define TAB1   2048
#define TAB2   4096
#define TAB3   6144
#define BSDLY  8192
#define BS0    0
#define BS1    8192
#define VTDLY  16384
#define VT0    0
#define VT1    16384
#define FFDLY  32768
#define FF0    0
#define FF1    32768

// Baudrates
#define B0     0
#define B50    1
#define B75    2
#define B110   3
#define B134   4
#define B150   5
#define B200   6
#define B300   7
#define B600   8
#define B1200  9
#define B1800  10
#define B2400  11
#define B4800  12
#define B9600  13
#define B19200 14
#define B38400 15

// Control modes
#define CSIZE  48
#define CS5    0
#define CS6    16
#define CS7    32
#define CS8    48
#define CSTOPB 64
#define CREAD  128
#define PARENB 256
#define PARODD 512
#define HUPCL  1024
#define CLOCAL 2048

// Local modes
#define ECHO   8
#define ECHOE  16
#define ECHOK  32
#define ECHONL 64
#define ICANON 2
#define IEXTEN 32768
#define ISIG   1
#define NOFLSH 128
#define TOSTOP 256

// Attribute selection
#define TCSANOW   1
#define TCSADRAIN 2
#define TCSAFLUSH 4

// Line control
#define TCIFLUSH  1
#define TCOFLUSH  2
#define TCIOFLUSH 3
#define TCIOFF    4
#define TCION     8
#define TCOOFF    16
#define TCOON     32

typedef unsigned char cc_t;
typedef unsigned int speed_t;
typedef unsigned int tcflag_t;

struct termios {
  tcflag_t c_iflag;
  tcflag_t c_oflag;
  tcflag_t c_cflag;
  tcflag_t c_lflag;
  speed_t  c_ispeed;
  speed_t  c_ospeed;
  cc_t     c_cc[NCCS];
};

static __inline__ speed_t cfgetispeed(const struct termios *termios) {
  return termios->c_ispeed;
}

static __inline__ int cfsetispeed(struct termios *termios,speed_t speed) {
  termios->c_ispeed = speed;
  return 0;
}

static __inline__ speed_t cfgetospeed(const struct termios *termios) {
  return termios->c_ospeed;
}

static __inline__ int cfsetospeed(struct termios *termios,speed_t speed) {
  termios->c_ospeed = speed;
  return 0;
}

int tcgetattr(int fildes,struct termios *termios_p);
int tcsetattr(int fildes,int optional_actions,const struct termios *termios_p);

#endif /* _TERMIOS_H_ */
