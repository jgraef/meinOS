#ifndef _SYS_UTSNAME_H_
#define _SYS_UTSNAME_H_

/// @todo Don't know if it's okay to use pointers instead of arrays
struct utsname {
  char *sysname;
  char *nodename;
  char *release;
  char *version;
  char *machine;
};

int uname(struct utsname *utsbuf);

#endif
