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
#include <sys/shm.h>
#include <rpc.h>
#include <misc.h>
#include <string.h>

int _create_unnamed_pipe(int read,int *_shmid);

/**
 * Creates a new process from executable file
 *  @param path Path to executable
 *  @param argv Argument vector
 *  @param stdin Reference for filehandle for stdin of new process
 *  @param stdout Reference for filehandle for stdout of new process
 *  @param stderr Reference for filehandle for stderr of new process
 *  @return PID of new process
 */
#include <stdio.h>
pid_t execute(const char *path,char *argv[],int *_stdin,int *_stdout,int *_stderr) {
  size_t shmsize = sizeof(struct process_data)+1;
  int shmid;
  struct process_data *data;
  size_t i;

  if (argv!=NULL) {
    for (i=0;argv[i];i++) shmsize += strlen(argv[i]);
  }
  shmid = shmget(IPC_PRIVATE,shmsize,0);

  if (shmid!=-1) {
    data = shmat(shmid,NULL,0);
    if (data!=NULL) {
      int stdin = _create_unnamed_pipe(0,&(data->shmid_stdin));
      int stdout = _create_unnamed_pipe(1,&(data->shmid_stdout));
      int stderr = _create_unnamed_pipe(1,&(data->shmid_stderr));
      if (_stdin!=NULL) *_stdin = stdin;
      if (_stdout!=NULL) *_stdout = stdout;
      if (_stderr!=NULL) *_stderr = stderr;

      size_t j = 0;
      if (argv!=NULL) {
        for (i=0;argv[i];i++) {
          strcpy(data->cmdline+j,argv[i]);
          j += strlen(data->cmdline+j)+1;
        }
        data->argc = i;
      }
      else data->argc = 0;

      shmdt(data);

      return rpc_call("proc_execute",0,path,shmid);
    }
  }
  return -1;
}
