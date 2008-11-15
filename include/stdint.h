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

#ifndef _STDINT_H_
#define _STDINT_H_

// Normal
typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

// Least
typedef int8_t              int_least8_t;
typedef int16_t             int_least16_t;
typedef int32_t             int_least32_t;
typedef int64_t             int_least64_t;
typedef uint8_t             uint_least8_t;
typedef uint16_t            uint_least16_t;
typedef uint32_t            uint_least32_t;
typedef uint64_t            uint_least64_t;

// Fast
typedef int32_t             int_fast8_t;
typedef int32_t             int_fast16_t;
typedef int32_t             int_fast32_t;
typedef int64_t             int_fast64_t;
typedef uint32_t            uint_fast8_t;
typedef uint32_t            uint_fast16_t;
typedef uint32_t            uint_fast32_t;
typedef uint64_t            uint_fast64_t;

// Pointer
typedef int32_t             intptr_t;
typedef uint32_t            uintptr_t;
typedef uint32_t            ptrdiff_t;

// Greatest
typedef int64_t             intmax_t;
typedef uint64_t            uintmax_t;


// Normal
#define INT8_MIN          -128
#define INT8_MAX          127
#define UINT8_MAX         255
#define INT16_MIN         -32768
#define INT16_MAX         32767
#define UINT16_MAX        65535
#define INT32_MIN         -2147483648
#define INT32_MAX         2147483647
#define UINT32_MAX        4294967295
#define INT64_MIN         0
#define INT64_MAX         ((uint64_t)-1)
#define UINT64_MAX        ((uint64_t)-1)

// Least
#define INT_LEAST8_MIN    -127
#define INT_LEAST8_MAX    127
#define UINT_LEAST8_MAX   255
#define INT_LEAST16_MIN   -32767
#define INT_LEAST16_MAX   32767
#define UINT_LEAST16_MAX  65535
#define INT_LEAST32_MIN   -2147483647
#define INT_LEAST32_MAX   2147483647
#define UINT_LEAST32_MAX  4294967295
#define INT_LEAST64_MIN   0
#define INT_LEAST64_MAX   ((uint64_t)-1)
#define UINT_LEAST64_MAX  ((uint64_t)-1)

// Fast
#define INT_FAST8_MIN     -127
#define INT_FAST8_MAX     127
#define UINT_FAST8_MAX    255
#define INT_FAST16_MIN    -32767
#define INT_FAST16_MAX    32767
#define UINT_FAST16_MAX   65535
#define INT_FAST32_MIN    -2147483647
#define INT_FAST32_MAX    2147483647
#define UINT_FAST32_MAX   4294967295
#define INT_FAST64_MIN    0
#define INT_FAST64_MAX    ((uint64_t)-1)
#define UINT_FAST64_MAX   ((uint64_t)-1)

// Pointer
#define INTPTR_MIN        -2147483648
#define INTPTR_MAX        2147483647
#define UINTPTR_MAX       -1

// Greatest
#define INTMAX_MIN        0
#define INTMAX_MAX        -1
#define UINTMAX_MAX       -1

#endif
