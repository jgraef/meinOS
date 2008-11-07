#include <sys/utsname.h>
#include <limits.h>
#include <stdio.h>

/// @todo WARNING! Fixed values, change them by hand!
static char *sysname = "meinOS";
static char *release = "0.1";
static char *version = "0.1";
/// @todo Maybe measure that (CPUID?)
static char *machine = "i686";
static char *hostfil = "/etc/hostname";

static char *get_nodename() {
  static char hostname[HOST_NAME_MAX];
  FILE *fd = fopen(hostfil,"r");
  if (fd!=NULL) {
    fgets(hostname,HOST_NAME_MAX,fd);
    fclose(fd);
    return hostname;
  }
  else return NULL;
}

int uname(struct utsname *utsbuf) {
  char *nodename = get_nodename();
  if (nodename==NULL) return -1;

  utsbuf->nodename = nodename;
  utsbuf->sysname = sysname;
  utsbuf->release = release;
  utsbuf->version = version;
  utsbuf->machine = machine;

  return 0;
}
