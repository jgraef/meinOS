#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc<2) {
    fprintf(stderr,"Usage: %s seconds\n",argv[0]);
    return 1;
  }

  int seconds = atoi(argv[1]);
  while (seconds>0) seconds = sleep(seconds);

  return 0;
}
