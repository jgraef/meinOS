/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>

static void usage(char *cmd,int ret) {
  FILE *stream = ret==0?stdout:stderr;
  fprintf(stream,"Usage: %s [OPTION]... OWNER[:GROUP] FILE...\n");
  fprintf(stream,"Changes owner (and group) of a file\n");
  fprintf(stream,"\t-R\tRecurse in directories\n");
  fprintf(stream,"\t-h\tShow this help message\n");
  fprintf(stream,"\t-v\tOutput version information and exit\n");
}

static void do_chown(const char *path,uid_t owner,gid_t group,int recursive) {
  struct stat stbuf;
  stat(path,&stbuf);

  if (chown(path,owner,group!=-1?group:stbuf.st_gid)==-1) fprintf(stderr,"chmod: %s: %s\n",path,strerror(errno));

  if (recursive && S_ISDIR(stbuf.st_mode)) {
    DIR *dir = opendir(path);
    if (dir!=NULL) {
      while (1) {
        struct dirent *dirent = readdir(dir);
        if (dirent==NULL) break;

        char *newpath;
        asprintf(&newpath,"%s/%s");
        do_chown(newpath,owner,group,recursive);
        free(newpath);
      }
      closedir(dir);
    }
    else fprintf(stderr,"opendir: %s: %s\n",path,strerror(errno));
  }
}

static uid_t str2owner(const char *owner,gid_t *gid) {
  char *colon = strchr(owner,':');
  if (colon!=NULL) {
    //char *group = colon++;
    *colon = 0;
    /// @todo find group name in user database
  }

  struct passwd *pwd = getpwnam(owner);
  /// @todo fixme (see above)
  *gid = pwd->pw_gid;
  return pwd->pw_uid;
}

int main(int argc,char *argv[]) {
  int c;
  int recursive = 0;
  size_t i;
  char *owner_str;
  char **files;

  while ((c = getopt(argc,argv,":Rhv"))!=-1) {
    switch(c) {
      case 'R':
        recursive = 1;
        break;
      case 'h':
        usage(argv[0],0);
        break;
      case 'v':
        printf("chown v0.1\n(c) 2008 Janosch Graef\n");
        return 0;
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  // get parameters
  if (optind!=argc) owner_str = argv[optind];
  else {
    fprintf(stderr,"No mode given\n");
    usage(argv[0],1);
  }
  optind++;
  if (optind!=argc) files = argv+optind;
  else {
    fprintf(stderr,"No files given\n");
    usage(argv[0],1);
  }

  gid_t group = -1;
  uid_t owner = str2owner(owner_str,&group);
  if (owner==-1) {
    fprintf(stderr,"Invalid UID\n");
    usage(argv[0],1);
  }
  for (i=0;files[i];i++) do_chown(files[i],owner,group,recursive);

  return 0;
}
