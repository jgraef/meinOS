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

#include <stdio.h>
#include <dirent.h>

#include "helper.h"

static rc_script_t *get_rc_scripts(int runlevel,char prefix,size_t *size) {
  rc_script_t *scripts = NULL;
  size_t i;
  char *dir_path = rcd_path(runlevel);
  DIR *dir = opendir(dir_path);

  if (dir!=NULL) {
    while (1) {
      struct dirent *dirent = readdir(dir);
      if (dirent==NULL) break;
      else if (dirent->d_name[0]==prefix) {
        scripts = realloc(scripts,(i+1)*sizeof(rc_script_t));
        parse_rc_script(scripts+i,dir_path,dirent->d_name);
        i++;
      }
    }
    if (size!=NULL) *size = i;
    return scripts;
  }
  else return NULL;
}

int main(int argc,char *argv[]) {
  size_t i,j,num_kills,num_starts;

  if (argc<2) {
    fprintf(stderr,"rc: Runlevel parameter missing.\n");
    return 1;
  }

  int newrl = str_to_runlevel(argv[1]);
  if (newrl==-1) {
    fprintf(stderr,"rc: Invalid runlevel: %s\n",argv[1]);
    return 1;
  }

  int oldrl = get_runlevel();

  printf("rc: Switching from runlevel %d to runlevel %d\n",oldrl,newrl);

  // get scripts
  rc_script_t *kills = get_rc_scripts(oldrl,'K',&num_kills);
  if (kills==NULL) {
    fprintf(stderr,"rc: Cannot get kill scripts\n");
  }

  rc_script_t *starts = get_rc_scripts(newrl,'S',&num_starts);
  if (starts==NULL) {
    fprintf(stderr,"rc: Cannot get start scripts\n");
  }

  // sort out script that are in old and new runlevel
  for (i=0;i<num_kills;i++) {
    for (j=0;j<num_starts;j++) {
      if (strcmp(kills[i].name,starts[i].name)==0) {
        free(kills[i].name);
        free(kills[i].path);
        kills[i].name = NULL;
        free(starts[i].name);
        free(starts[i].path);
        starts[i].name = NULL;
      }
    }
  }

  // sort by priority
  qsort(kills,num_kills,sizeof(rc_script_t),compare_rc_script);
  qsort(starts,num_starts,sizeof(rc_script_t),compare_rc_script);

  // run kills
  for (i=0;i<num_kills;i++) {
    char *command;
    asprintf(&command,"%s stop",kills[i].path);
    system(command);
    free(command);
  }

  // run starts
  for (i=0;i<num_starts;i++) {
    char *command;
    asprintf(&command,"%s start",starts[i].path);
    system(command);
    free(command);
  }

  printf("rc: Switched to runlevel %d successfully\n",newrl);

  return 0;
}
