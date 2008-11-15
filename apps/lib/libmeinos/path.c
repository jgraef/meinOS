/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/types.h>
#include <path.h>
#include <stdlib.h>
#include <string.h>

path_t *path_parse(const char *path) {
  if (path==NULL) return NULL;
  size_t i;
  size_t num_parts = 1;

  for (i=0;path[i];i++) {
    if (path[i]=='/') num_parts++;
  }

  path_t *pathbuf = malloc(sizeof(path_t));
  pathbuf->num_parts = num_parts;
  pathbuf->data = strdup(path);
  pathbuf->parts = malloc(num_parts*sizeof(char*));
  pathbuf->root = (path[0]=='/');

  char *last = pathbuf->data;
  size_t j = 0;
  for (i=0;i<num_parts;i++) {
    while (pathbuf->data[j]!='/' && pathbuf->data[j]!=0) j++;
    pathbuf->parts[i] = last;
    pathbuf->data[j] = 0;
    j++;
    last = pathbuf->data+j;
  }

  return pathbuf;
}

void path_destroy(path_t *path) {
  if (path==NULL) return;
  free(path->parts);
  free(path->data);
  free(path);
}

void path_reject_dots(path_t *path) {
  if (path==NULL) return;
  size_t i;
  ssize_t j;
  for (i=0;i<path->num_parts;i++) {
    if (strcmp(path->parts[i],".")==0) memset(path->parts[i],0,strlen(path->parts[i]));
    else if (strcmp(path->parts[i],"..")==0) {
      memset(path->parts[i],0,strlen(path->parts[i]));
      for (j=i-1;j>=0;j--) {
        if (path->parts[j][0]!=0) {
          memset(path->parts[j],0,strlen(path->parts[j]));
          break;
        }
      }
    }
  }
}

char *path_output(path_t *path,char *path_str) {
  if (path==NULL) return NULL;
  size_t real_parts = 0;
  size_t i;

  for (i=0;i<path->num_parts;i++) {
    if (path->parts[i][0]!=0) real_parts++;
  }

  if (real_parts>0) {
    size_t j = 0;

    if (path_str==NULL) {
      size_t pathlen = path->num_parts+1+(path->root?1:0);
      for (i=0;i<path->num_parts;i++) pathlen += strlen(path->parts[i]);
      path_str = malloc(pathlen);
    }

    if (path->root && path->parts[0][0]!=0) path_str[j++] = '/';

    for (i=0;i<path->num_parts;i++) {
      size_t part_len = strlen(path->parts[i]);
      memcpy(path_str+j,path->parts[i],part_len);
      j += part_len;
      if (i<path->num_parts-1) {
        if (path->parts[i+1][0]!=0) {
          path_str[j] = '/';
          j++;
        }
      }
    }

    path_str[j] = 0;

    return path_str;
  }
  else {
    if (path_str==NULL) {
      return strdup("/");
    }
    else return strcpy(path_str,"/");
  }
}

size_t path_compare(path_t *path1,path_t *path2) {
  if (path1==NULL || path2==NULL) return 0;
  size_t j1 = 0;
  size_t j2 = 0;
  size_t i = 0;

  if (path1->root!=path2->root) return 0;

  while (j1<path1->num_parts && j2<path2->num_parts) {
    if (path1->parts[j1][0]==0) {
      j1++;
      continue;
    }
    if (path2->parts[j2][0]==0) {
      j2++;
      continue;
    }
    if (path2->parts[j2][0]!=0 && path2->parts[j2][0]!=0) {
      if (strcmp(path1->parts[j1],path2->parts[j2])!=0) break;
    }
    j1++;
    j2++;
    i++;
  }

  return i;
}

path_t *path_cat(path_t *path1,path_t *path2) {
  if (path1==NULL || path2==NULL) return NULL;

  path_t *pathn = malloc(sizeof(path_t));
  pathn->root = path1->root;
  pathn->num_parts = path1->num_parts+path2->num_parts;

  size_t i,j;
  size_t path1_size = 0;
  size_t path2_size = 0;
  for (i=0;i<path1->num_parts;i++) path1_size += strlen(path1->parts[i])+1;
  for (i=0;i<path2->num_parts;i++) path2_size += strlen(path2->parts[i])+1;

  pathn->data = malloc(path1_size+path2_size);
  pathn->parts = malloc(pathn->num_parts*sizeof(char*));

  j = 0;
  for (i=0;i<path1->num_parts;i++) {
    strcpy(pathn->data+j,path1->parts[i]);
    pathn->parts[i] = pathn->data+j;
    j += strlen(path1->parts[i]);
    pathn->data[j] = 0;
    j++;
  }

  for (i=0;i<path2->num_parts;i++) {
    strcpy(pathn->data+j,path2->parts[i]);
    pathn->parts[path1->num_parts+i] = pathn->data+j;
    j += strlen(path2->parts[i]);
    pathn->data[j] = 0;
    j++;
  }

  return pathn;
}

void path_parent(path_t *path) {
  if (path->num_parts>0) {
    path->parts[path->num_parts-1][0] = 0;
  }
}
