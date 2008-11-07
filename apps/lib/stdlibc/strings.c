#include <strings.h>
#include <stddef.h>

int strcasecmp(const char *str1,const char *str2) {
  size_t i;
  for (i=0;(str1[i]!=0 && str2[i]!=0);i++) {
    if (str1[i]!=str2[i]) return str1[i]-str2[i];
  }
  return 0;
}

int strncasecmp(const char *str1,const char *str2,size_t n) {
  size_t i;
  for (i=0;(str1[i]!=0 && str2[i]!=0 && i<n);i++) {
    if (str1[i]!=str2[i]) return str1[i]-str2[i];
  }
  return 0;
}

int ffs(int x) {
  size_t i;
  for (i=0;i<sizeof(int)*8;i++) {
    if (x&(1<<i)) return i+1;
  }
  return 0;
}
