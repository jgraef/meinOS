#include <sys/types.h>
#include <ioport.h>
#include <cmos.h>

/// @todo Do this via RPC

/**
 * Reads value from CMOS
 *  @param offset Offset in CMOS
 *  @return Read Value
 */
int cmos_read(size_t offset) {
  int val;
  outb(CMOS_PORT_ADDRESS,(inb(CMOS_PORT_ADDRESS)&0x80)|(offset&0x7F));
  val = inb(CMOS_PORT_DATA);
  return val;
}

/**
 * Writes value to CMOS
 *  @param offset Offset in CMOS
 *  @param val Value to write
 */
void cmos_write(size_t offset,int val) {
  outb(CMOS_PORT_ADDRESS,(inb(CMOS_PORT_ADDRESS)&0x80)|(offset&0x7F));
  outb(CMOS_PORT_DATA,val);
}
