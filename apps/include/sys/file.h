#ifndef _SYS_FILE_H_
#define _SYS_FILE_H_

#include <fcntl.h>
#include <unistd.h>

#define L_SET  SEEK_SET
#define L_INCR SEEK_CUR
#define L_XTND SEEK_END

#define LOCK_SH 1
#define LOCK_EX 2
#define LOCK_NB 4
#define LOCK_UN 8

#endif
