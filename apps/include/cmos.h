#ifndef _LIBMEINOS_CMOS_H_
#define _LIBMEINOS_CMOS_H_

#include <sys/types.h>

#define CMOS_PORT_ADDRESS 0x70
#define CMOS_PORT_DATA    0x71

#define CMOS_OFFSET_SECOND       0x00
#define CMOS_OFFSET_ALARMSECOND  0x01
#define CMOS_OFFSET_MINUTE       0x02
#define CMOS_OFFSET_ALARMINUTE   0x03
#define CMOS_OFFSET_HOUR         0x04
#define CMOS_OFFSET_ALARMHOUR    0x05
#define CMOS_OFFSET_DAYOFWEEK    0x06
#define CMOS_OFFSET_DAYOFMONTH   0x07
#define CMOS_OFFSET_MONTH        0x08
#define CMOS_OFFSET_YEAR         0x09
#define CMOS_OFFSET_SRA          0x0A
#define CMOS_OFFSET_SRB          0x0B
#define CMOS_OFFSET_SRC          0x0C
#define CMOS_OFFSET_SRD          0x0D
#define CMOS_OFFSET_POST         0x0E
#define CMOS_OFFSET_SHUTDOWN     0x0F
#define CMOS_OFFSET_FDTYPES      0x10
#define CMOS_OFFSET_HDTYPES      0x12
#define CMOS_OFFSET_DEVICEBYTE   0x14
#define CMOS_OFFSET_BASEMEMLOW   0x15
#define CMOS_OFFSET_BASEMEMHIGH  0x16
#define CMOS_OFFSET_EXTMEMLOW    0x17
#define CMOS_OFFSET_EXTMEMHIGH   0x18
#define CMOS_OFFSET_CHECKSUMHIGH 0x2E
#define CMOS_OFFSET_CHECKSUMLOW  0x2F
#define CMOS_OFFSET_CENTURY      0x32

#define cmos_bcd2bin(bcd)    (((bcd>>4)&0x0F)*10+(bcd&0x0F))
#define cmos_bin2bcd(bin)    (((bin/10)<<8)+(bin%10))

#define cmos_getsecond()     cmos_bcd2bin(cmos_read(CMOS_OFFSET_SECOND))
#define cmos_getminute()     cmos_bcd2bin(cmos_read(CMOS_OFFSET_MINUTE))
#define cmos_gethour()       cmos_bcd2bin(cmos_read(CMOS_OFFSET_HOUR))
#define cmos_getdayofweek()  cmos_bcd2bin(cmos_read(CMOS_OFFSET_DAYOFWEEK))
#define cmos_getday()        cmos_bcd2bin(cmos_read(CMOS_OFFSET_DAYOFMONTH))
#define cmos_getmonth()      cmos_bcd2bin(cmos_read(CMOS_OFFSET_MONTH))
#define cmos_getyear()       (cmos_bcd2bin(cmos_read(CMOS_OFFSET_CENTURY))*100+cmos_bcd2bin(cmos_read(CMOS_OFFSET_YEAR)))

#define cmos_setsecond(v)    cmos_write(CMOS_OFFSET_SECOND,cmos_bin2bcd(v))
#define cmos_setminute(v)    cmos_write(CMOS_OFFSET_MINUTE,cmos_bin2bcd(v))
#define cmos_sethour(v)      cmos_write(CMOS_OFFSET_HOUR,cmos_bin2bcd(v))
#define cmos_setdayofweek(v) cmos_write(CMOS_OFFSET_DAYOFWEEK,cmos_bin2bcd(v))
#define cmos_setday(v)       cmos_write(CMOS_OFFSET_DAYOFMONTH,cmos_bin2bcd(v))
#define cmos_setmonth(v)     cmos_write(CMOS_OFFSET_MONTH,cmos_bin2bcd(v))
#define cmos_setyear(v)      do { cmos_write(CMOS_OFFSET_YEAR,cmos_bin2bcd(v&0xFF)); cmos_write(CMOS_OFFSET_CENTURY,cmos_bin2bcd((v>>8)&0xFF)); } while(0);

#define cmos_getbasemem()    ((cmos_read(CMOS_OFFSET_BASEMEMHIGH)<<8)|cmos_read(CMOS_OFFSET_BASEMEMLOW))
#define cmos_getextmem()     ((cmos_read(CMOS_OFFSET_EXTMEMHIGH)<<8)|cmos_read(CMOS_OFFSET_EXTMEMLOW))
#define cmos_getchecksum()   ((cmos_read(CMOS_OFFSET_CHECKSUMHIGH)<<8)|cmos_read(CMOS_OFFSET_CHECKSUMLOW))

int cmos_read(size_t offset);
void cmos_write(size_t offset,int val);

#endif
