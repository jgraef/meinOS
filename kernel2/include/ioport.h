#include <stdint.h>

#define IOPORT_ISREG(iopb,port) ((*(((int*)iopb)+(port/sizeof(int))))&(1<<(port%sizeof(int))))
#define IOPORT_REG(iopb,port)   (*(((int*)iopb)+(port/sizeof(int))) |= 1<<(port%sizeof(int)))
#define IOPORT_UNREG(iopb,port) (*(((int*)iopb)+(port/sizeof(int))) &= ~(1<<(port%sizeof(int))))

#define IOPORT_MAXNUM 0x10000

int *iopb_general;

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

int ioport_init();
int ioport_reg(unsigned int port);
int ioport_unreg(unsigned int port);
