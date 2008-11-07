#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321

// x86 (Little Endian)
#define __BYTE_ORDER       __LITTLE_ENDIAN
#define __FLOAT_WORD_ORDER __BYTE_ORDER
#define __WORDSIZE         32

#endif
