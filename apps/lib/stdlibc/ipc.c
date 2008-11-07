#include <sys/types.h>
#include <sys/ipc.h>

key_t ftok(const char *file,int x) {
  x &= 0xFF;
  key_t val = 0;
  while (*file) val = *file++;
  val *= x;
  val = val<0?-val:val;
  if (val==IPC_PRIVATE) val++;
  return val;
}
