#include <sys/types.h>
#include <ioport.h>
#include <syscall.h>

int ioport_init() {
  iopb_general = malloc(IOPORT_MAXNUM/sizeof(*iopb_general));
  if (syscall_create(SYSCALL_IO_REG,ioport_reg,1)==-1) return -1;
  if (syscall_create(SYSCALL_IO_UNREG,ioport_unreg,1)==-1) return -1;
  return 0;
}

int ioport_reg(unsigned int port) {
  /*if (port>=IOPORT_MAXNUM) return -ENOENT;
  if (proc_current->iopb==NULL) return -EPERM;
  if (IOPORT_ISREG(iopb_general,port)) return -EEXIST;

  IOPORT_REG(iopb_general,port);
  IOPORT_REG(proc_current->iopb,port);*/
  return 0;
}

int ioport_unreg(unsigned int port) {
  /*if (port>=IOPORT_MAXNUM) return -ENOENT;
  if (proc_current->iopb==NULL) return -EPERM;
  if (!IOPORT_ISREG(proc_current->iopb,port)) return -EPERM;

  IOPORT_UNREG(iopb_general,port);
  IOPORT_UNREG(proc_current->iopb,port);*/
  return 0;
}
