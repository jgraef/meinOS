#ifndef _PATH_H_
#define _PATH_H_

#include <sys/types.h>

typedef struct {
  size_t num_parts;
  char **parts;
  char *data;
  int root;
} path_t;

path_t *path_parse(const char *path);
void path_destroy(path_t *path);
void path_reject_dots(path_t *path);
char *path_output(path_t *path,char *path_str);
size_t path_compare(path_t *path1,path_t *path2);
path_t *path_cat(path_t *path1,path_t *path2);
void path_parent(path_t *path);

#endif
