#if 0
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <llist.h>

struct ls_opt opt = {
  int show_hidden;
  int show_dot;
  int show_backup;
  int show_inode;
  int show_username;
  int show_unreadable;
  int escape;
  int sort;
  int humanreadable;
  int reverse;
  int recursive;
};

typedef struct {
  char *filename;
  struct stat stbuf;
  char *path;
} ls_file_t;

static ls_file_t *ls_file_create(const char *path,char *filename) {
  ls_file_t *file = malloc(sizeof(ls_file_t));

  file->filename = strdup(filename);
  asprintf(&(file->path),"%s/%s",path,filename);
  stat(file->path,&(file->stbuf));

  return file;
}

static void ls_file_destroy(ls_file_t *file) {
  free(file->filename);
  free(file->path);
  free(file);
}

static char ls_filetype(mode_t mode) {
  if (S_ISBLK(mode)) return 'b';
  else if (S_ISCHR(mode)) return 'c';
  else if (S_ISDIR(mode)) return 'd';
  else if (S_ISFIFO(mode)) return 'f';
  else if (S_ISLNK(mode)) return 'l';
  else if (S_ISSOCK(mode)) return 's';
  else return '-';
}

static const char *ls_perm(mode_t mode) {
  static char buf[10];
  snprintf(buf,10,"%c%c%c%c%c%c%c%c%c",mode&S_IRUSR?'r':'-',mode&S_IWUSR?'w':'-',mode&S_IXUSR?'x':'-',mode&S_IRGRP?'r':'-',mode&S_IWGRP?'w':'-',mode&S_IXGRP?'x':'-',mode&S_IROTH?'r':'-',mode&S_IWOTH?'w':'-',mode&S_IXOTH?'x':'-');
  return buf;
}

static fs_file_t **ls_sort(struct ls_opt *opt,llist_t files) {
  fs_file_t **sorted = malloc((llist_size(files)+1)*sizeof(fs_file_t*));
  size_t i,j;

  for (i=0;(file = llist_get(files,i));i++) {
    if ((!opt->show_dot && strcmp(file->filename,".")!=0 && !strcmp(file->filename,"..")==0)
      ||(!opt->show_hidden && file->filename[0]=='.' && (file->filename[1]=='.' || file->filename[1]==0))
      ||(!opt->show_backup && file->filename[strlen(file->filename)-1]=='~')) llist_remove(files,i--);
  }

  sorted[0] = llist_get(files,0);
  for (i=1;(file = llist_get(files,i));i++) {
    for (j=0;j<i;j++) {
      if (strcmp(file->filename,sorted[j]->filename)<0) { // file is smaller then sorted[j]
        memmove(sorted+j+1,sorted+j,j-i);
        sorted[j] = file;
        break;
      }
    }
  }
}

static int ls_list(struct ls_opt *opt,char *path) {
  DIR *dir = opendir(path);
  llist_t files = llist_create();

  if (dir!=NULL) {
    size_t i;
    struct dirent *dirent;

    while (1) {
      dirent = readdir(dir);
      if (dirent==NULL) break;
      else llist_push(files,ls_file_create(path,dirent->d_name));
    }

    fs_file_t **sorted = ls_sort(opt,files);
    llist_destroy(files);

    if (opt->recursive) printf("%s:\n",path);
    printf("absolute %d",llist_size(files));

    for (i=0;sorted[i]!=NULL;i++) {
      ls_file_print(opt,sorted[i]);
      if (S_ISDIR(sorted[i]->stbuf.st_mode) && opt->recursive) ls_list(opt,sorted[i]->path);
    }

    return 0;
  }
  else return 1;
}

int ls_main(int argc,char *argv[]) {
  int c;
  char *dir;
  struct ls_opt opt = {
    .show_hidden = 0,
    .show_dot = 1,
    .show_backup = 1,
    .show_inode = 0,
    .show_username = 1,
    .show_unreadable = 1,
    .escape = 0,
    .sort = 1,
    .humanreadable = 0,
    .reverse = 0,
    .recursive = 0
  };

  while ((c = getopt(argc,argv,":hv"))!=-1) {
    switch(c) {
      case 'a':
        opt.show_hidden = 1;
      case 'A':
        opt.show_dot = 0;
      case 'b':
        opt.escape = 1;
      case 'B':
        opt.show_backup = 0;
      case 'f':
        opt.sort = 0;
      case 'h':
        opt.humanreadable = 1;
      case 'i':
        opt.show_inode = 1;
      case 'n':
        opt.show_username = 0;
      case 'q':
        opt.show_unreadable = 0;
      case 'r':
        opt.reverse = 1;
      case 'R':
        opt.recursive = 1;
      case 'h':
        usage(argv[0],0);
        break;
      case 'v':
        printf("cat v0.1\n(c) 2008 Janosch Graef\n");
        return 0;
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  if (optind<argc) dir = argv[argc];
  else dir = ".";

  return ls_list(opt,dir);
}
#endif
