#ifndef _SYS_CDEFS_H_
#define _SYS_CDEFS_H_

#define __CONCAT1(x,y)  x ## y
#define __CONCAT(x,y)   __CONCAT1(x,y)
#define __STRING(x)     #x              /* stringify without expanding x */
#define __XSTRING(x)    __STRING(x)     /* expand x, then stringify */

#define __const         const           /* define reserved names to standard */
#define __signed        signed
#define __volatile      volatile

#endif
