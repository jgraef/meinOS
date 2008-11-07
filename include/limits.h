#ifndef _LIMITS_H_
#define _LIMITS_H_

// only userspace
#define ATEXIT_MAX     (-1)
#define LOGIN_NAME_MAX 256
#define HOST_NAME_MAX  64
#define LOGIN_NAME_MAX 256

// both
#define NAME_MAX       256
#define PATH_MAX       1024

#define SCHAR_MIN      (-128)
#define SCHAR_MAX      127
#define UCHAR_MAX      255
#define CHAR_MAX       SCHAR_MAX
#define CHAR_MIN       SCHAR_MIN
#define CHAR_BIT       8

#define SHRT_MIN       (-32768)
#define SHRT_MAX       32767
#define USHRT_MAX      65535

#define LONG_MAX       0x7FFFFFFF
#define LONG_MIN       (-0x80000000)

#define INT_MAX        0x7FFFFFFF
#define INT_MIN        (-0x80000000)

#define ULONG_MAX      0xFFFFFFFF
#define UINT_MAX       0xFFFFFFFF

#define PAGESIZE       4096
#define PAGE_SIZE      PAGESIZE

// stdio.h
#define FILENAME_MAX   PATH_MAX
#define FOPEN_MAX      (-1)

// sys/uio.h
#define IOV_MAX        4096

#endif
