#ifndef _DIRENT_H_
#define _DIRENT_H_

#ifdef NAME_MAX
  #define MAXNAMELEN
#endif

#define rewinddir(d) seekdir(d,0)

struct dirent {
  char *d_name;
};

typedef struct filelist_item DIR;

DIR *opendir(const char *path);
int closedir(DIR *dh);
struct dirent *readdir(DIR *dh);
void seekdir(DIR *dh,long loc);
long telldir(DIR *dirp);

#endif
