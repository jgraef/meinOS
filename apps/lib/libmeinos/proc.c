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
#include <sys/stat.h>
#include <syscall.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pack.h>

size_t _fs_pack_filehandles(pack_t buf);
void _fs_unpack_filehandles(pack_t buf);

/**
 * Gets name of a process
 *  @param pid PID of process
 *  @return Name of process (can be passed to free())
 */
char *getname(pid_t pid) {
  size_t size = syscall_call(SYSCALL_PROC_GETNAME,3,pid,NULL,0);
  char *buf = malloc(size);
  syscall_call(SYSCALL_PROC_GETNAME,3,pid,buf,size);
  return buf;
}

/**
 * Gets list of used pages
 *  @param pid PID of process to get list from
 *  @param _num_pages Reference for number of pages
 *  @return Array with pages (must be free()'d)
 */
void **proc_mempagelist(pid_t pid,size_t *_num_pages) {
  ssize_t num_pages = syscall_call(SYSCALL_PROC_MEMPAGELIST,3,pid,NULL,0);
  if (num_pages>0) {
    void **pages = malloc(num_pages*sizeof(void*));
    *_num_pages = syscall_call(SYSCALL_PROC_MEMPAGELIST,3,pid,pages,num_pages);
    return pages;
  }
  else return NULL;
}

/**
 * Pack process data
 *  @param argv Argument vector
 *  @param env Enviroment variables
 *  @return Var for new process
 */
int proc_pack_procdata(char **argv,char **env,char *cwd,mode_t cmask) {
  size_t argv_size,fh_size,cwd_size,env_size,cmask_size,i;
  int shmid,argc,env_num;
  void *shmbuf;
  pack_t buf;

  // prepare argv
  argv_size = sizeof(argc);
  if (argv!=NULL) {
    for (argc=0;argv[argc];argc++) argv_size += strlen(argv[argc])+1;
  }
  else argc = 0;

  // prepare env
  env_size = sizeof(env_num);
  if (env!=NULL) {
    for (env_num=0;env[env_num];env_num++) env_size += strlen(env[env_num]);
  }
  else env_num = 0;

  // prepare current workdir
  if (cwd==NULL) cwd = "/";
  cwd_size = strlen(cwd)+1;

  // prepare creation mask
  cmask_size = sizeof(cmask);

  // prepare filehandles
  fh_size = _fs_pack_filehandles(NULL);

  // allocate space
  shmid = shmget(IPC_PRIVATE,argv_size+env_size+cwd_size+cmask_size+fh_size,0);
  if (shmid!=-1) {
    shmbuf = shmat(shmid,NULL,0);
    if (shmbuf!=NULL) {
      buf = pack_create(shmbuf);

      // pack argv
      packi(buf,argc);
      for (i=0;i<argc;i++) packstr(buf,argv[i]);
      // pack env
      packi(buf,env_num);
      for (i=0;i<env_num;i++) packstr(buf,env[i]);

      // pack current workdir
      packstr(buf,cwd);

      // pack creation mask
      packi(buf,cmask);

      // pack filehandles
      _fs_pack_filehandles(buf);

      // release space
      pack_destroy(buf);
      shmdt(shmbuf);
      return shmid;
    }
    else shmctl(shmid,IPC_RMID,NULL);
  }

  return -1;
}

/**
 * Unpack process data
 *  @param var Var
 *  @param argv Reference for argument vector
 *  @param argc Reference for argument count
 *  @return Var for new process
 */
int proc_unpack_procdata(int var,int *argc,char ***argv) {
  int ret;
  size_t i,env_num;
  char *cwd;
  void *shmbuf;
  pack_t buf;
  mode_t cmask;

  // get space
  shmbuf = shmat(var,NULL,0);
  if (shmbuf!=NULL) {
    buf = pack_create(shmbuf);

    // unpack argv
    unpacki(buf,argc);
    *argv = malloc(*argc*sizeof(char*));
    for (i=0;i<*argc;i++) unpackstr(buf,(*argv)+i);
    (*argv)[i] = NULL;

    // unpack env
    unpacki(buf,&env_num);
    environ = malloc(env_num*sizeof(char*));
    for (i=0;i<env_num;i++) {
      char *tmp;
      unpackstr(buf,&tmp);
      environ[i] = strdup(tmp);
    }
    environ[i] = NULL;

    // unpack current workdir
    unpackstr(buf,&cwd);
    chdir(cwd);

    // unpack creation mask
    unpacki(buf,&cmask);
    umask(cmask);

    // unpack filehandles
    _fs_unpack_filehandles(buf);

    // free space
    pack_destroy(buf);
    shmdt(shmbuf);
    ret = var;
  }
  else ret = -1;

  shmctl(var,IPC_RMID,NULL);

  return ret;
}
