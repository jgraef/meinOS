#include <misc.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void usage(char *cmd,int err) {
  FILE *out = err?stderr:stdout;
  fprintf(out,"Usage: %s [-t fstype] device dir\n");
}

int main(int argc,char *argv[]) {
#if 0
  char *type = NULL;
  char *dev = NULL;
  char *mountpoint = NULL;
  int c;
  int list = 0;
  int readonly = 0;

  if (argc==1) list = 1;
  else {
    while ((c = getopt(argc,argv,":ht:lVrw"))!=-1) {
      switch(c) {
        case 'h':
          usage(argv[0],0);
          break;
        case 't':
          if (type==NULL) type = strdup(optarg);
          else {
            fprintf(stderr,"Multiple FS types selected\n");
            usage(argv[0],1);
          }
          break;
        case 'l':
          list = 1;
          break;
        case 'V':
          printf("mount v0.1\n(c) Janosch Graef 2008\n");
          return 0;
          break;
        case 'r':
          readonly = 1;
          break;
        case 'w':
          readonly = 0;
          break;
        case ':':
          fprintf(stderr,"Option -%c requires an operand\n",optopt);
          usage(argv[0],1);
          break;
        case '?':
          fprintf(stderr,"Unrecognized option: -%c\n", optopt);
          usage(argv[0],1);
          break;
      }
    }
  }

  if (list==0) {
    if (optind+1<argc) {
      dev = argv[optind];
      mountpoint = argv[optind+1];
    }
    if (mountpoint==NULL) {
      fprintf(stderr,"No mountpoint given\n");
      usage(argv[0],1);
    }
    if (dev==NULL) {
      fprintf(stderr,"No device given\n");
      usage(argv[0],1);
    }
    else if (strcmp(dev,"/dev/cdrom")==0 && type==NULL) type = "iso9660";
    if (type==NULL) {
      fprintf(stderr,"No FS type given\n");
      usage(argv[0],1);
    }
    return vfs_mount(type,mountpoint,dev,readonly)==0?0:1;
  }
  else {
    printf("TODO: List\n");
    return -1;
  }
#endif

  printf("[MOUNT ] mount is here (PID: %d)\n",getpid());
  while (1);

}
