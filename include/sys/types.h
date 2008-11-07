#ifndef _SYS_TYPES_H_
#define _SYS_TYPES_H_

#define NULL ((void*)0)

typedef unsigned int   size_t;
typedef signed int     ssize_t;
typedef signed int     blkcnt_t;
typedef ssize_t        blksize_t;
typedef unsigned int   clock_t;
typedef unsigned int   clockid_t;
typedef unsigned int   dev_t;
typedef unsigned int   fsblkcnt_t;
typedef size_t         fsfilcnt_t;
typedef signed int     id_t;
typedef id_t           gid_t;
typedef unsigned int   ino_t;
typedef long           key_t;
typedef unsigned int   mode_t;
typedef unsigned int   nlink_t;
typedef int            off_t;
typedef id_t           pid_t;
typedef signed int     suseconds_t;
typedef unsigned int   time_t;
typedef id_t           uid_t;
typedef unsigned int   useconds_t;

#endif
