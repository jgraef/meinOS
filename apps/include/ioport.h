#ifndef _LIBMEINOS_IOPORT_H_
#define _LIBMEINOS_IOPORT_H_

#include <stdint.h>
#include <syscall.h>
#include <errno.h>

static inline int ioport_reg(uint16_t port) {
  int ret = syscall_call(SYSCALL_IO_REG,1,port);
  errno = ret<0?-ret:0;
  return ret<0?-1:ret;
}

static inline int ioport_unreg(uint16_t port) {
  int ret = syscall_call(SYSCALL_IO_UNREG,1,port);
  errno = ret<0?-ret:0;
  return ret<0?-1:ret;
}

static inline uint8_t inb(uint16_t _port) {
  uint8_t _result;
  __asm__("inb %1, %0":"=a"(_result):"Nd"(_port));
  return _result;
}

static inline uint16_t inw(uint16_t _port) {
  uint16_t _result;
  __asm__("inw %1, %0":"=a"(_result):"Nd"(_port));
  return _result;
}

static inline uint32_t inl(uint16_t _port) {
  uint32_t _result;
  __asm__("inl %1, %0":"=a"(_result):"Nd"(_port));
  return _result;
}

static inline void outw(uint16_t _port,uint16_t _data) {
  __asm__("outw %0, %1"::"a"(_data),"Nd"(_port));
}

static inline void outb(uint16_t _port,uint8_t _data) {
  __asm__("outb %0, %1"::"a"(_data),"Nd"(_port));
}

static inline void outl(uint16_t _port,uint32_t _data) {
  __asm__("outl %0, %1"::"a"(_data),"Nd" (_port));
}

#endif

