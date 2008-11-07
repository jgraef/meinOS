#include <path.h>
#include <unistd.h>
#include <string.h>

char *basename(char *path) {
  if (path==NULL) return ".";
  else if (path[0]==0) return ".";
  else if (strcmp(path,"/")==0) return path;

  path_t *pathbuf = path_parse(path);
  ssize_t i;
  int any_part = 0;

  for (i=pathbuf->num_parts-1;i>=0;i--) {
    if (pathbuf->parts[i][0]!=0) {
      strcpy(path,pathbuf->parts[i]);
      any_part = 1;
      break;
    }
  }

  if (!any_part) strcpy(path,"/");
  path_destroy(pathbuf);

  return path;
}

char *dirname(char *path) {
  if (path==NULL) return ".";
  else if (strcmp(path,"/")==0) return "/";
  else if (strchr(path,'/')==NULL || path[0]==0 || strcmp(path,".")==0 || strcmp(path,"..")==0) return ".";

  path_t *pathbuf = path_parse(path);
  if (pathbuf->num_parts<2) strcpy(path,".");
  else {
    ssize_t i;
    for (i=pathbuf->num_parts-1;i>=0;i--) {
      if (pathbuf->parts[i][0]!=0) {
        memset(pathbuf->parts[i],0,strlen(pathbuf->parts[i]));
        break;
      }
    }
  }
  path = path_output(pathbuf,path);
  path_destroy(pathbuf);
  return path;
}

char *realpath(const char *file_name,char *resolved_name) {
  char *buf = resolved_name;
  path_t *path = path_parse(file_name);
  path_reject_dots(path);

  if (!path->root) {
    getcwd(buf,PATH_MAX);
    buf += strlen(buf);
  }

  /// @todo solve symbolic links

  path_output(path,buf);

  return resolved_name;
}
