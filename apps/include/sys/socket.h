#ifndef _SYS_SOCKET_H_
#define _SYS_SOCKET_H_

#include <stdint.h>
#include <sys/uio.h>

/// @todo CMSG features
/*
#define SCM_RIGHTS 1
#define CMSG_DATA(cmsg) ((unsigned char*)((cmsg)+1))
#define CMSG_NXTHDR(mhdr,cmsg) ((mhdr)->msg_control>=(cmsg) && (mhdr)->msg_control<(cmsg)+(cmsg)->cmsg_len)
*/

#define SOCK_DGRAM     1
#define SOCK_RAW       2
#define SOCK_SEQPACKET 3
#define SOCK_STREAM    4

#define SOL_SOCKET 1

#define SO_ACCEPTCONN 0x00000000
#define SO_BROADCAST  0x00000001
#define SO_DEBUG      0x00000002
#define SO_DONTROUTE  0x00000003
#define SO_ERROR      0x00000004
#define SO_KEEPALIVE  0x00000005
#define SO_LINGER     0x00000006
#define SO_OOBINLINE  0x00000007
#define SO_RCVBUF     0x00000008
#define SO_RCVLOWAT   0x00000009
#define SO_RCVTIMEO   0x0000000A
#define SO_REUSEADDR  0x0000000B
#define SO_SNDBUF     0x0000000C
#define SO_SNDLOWAT   0x0000000D
#define SO_SNDTIMEO   0x0000000E
#define SO_TYPE       0x0000000F

#define SOMAXCONN 1024 /// @todo don't know!?

#define MSG_CTRUNC    1
#define MSG_DONTROUTE 2
#define MSG_EOR       4
#define MSG_OOB       8
#define MSG_PEEK      16
#define MSG_TRUNC     32
#define MSG_WAITALL   64

#define AF_INET   1
#define AF_INET6  2
#define AF_UNIX   3
#define AF_UNSPEC 4

#define SHUT_RD   1
#define SHUT_WR   2
#define SHUT_RDWR (SHUTRD|SHUT_WR)

typedef uint32_t socklen_t;
typedef unsigned int sa_family_t;

struct sockaddr {
  sa_family_t sa_family;
  char sa_data[];
};

struct sockaddr_storage {
  sa_family_t ss_family;
  char ss_data[256]; ///< @todo How large must it be?
};

struct msghdr {
  void *msg_name;
  socklen_t msg_namelen;
  struct iovec msg_iov;
  int msg_iovlen;
  void *msg_control;
  socklen_t msg_controllen;
  int msg_flags;
};

struct cmsghdr {
  socklen_t cmsg_len;
  int cmsg_level;
  int cmsg_type;
};

struct linger {
  int l_onoff;
  int l_linger;
};

#endif
