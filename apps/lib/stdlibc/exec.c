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
#include <sys/msg.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <syscall.h>
#include <libgen.h>

#define EXEC_TYPE_ARGV    1
#define EXEC_TYPE_ENVIRON 2
#define EXEC_TYPE_INFO    3

struct msg {
  long type;
  char buf[0];
};

struct msg_info {
  long type;
  size_t argv_size;
  size_t env_size;
  int io_msqid;
};

char **environ; ///< @todo

/**
 * Replace process image and save information that is needed after replace
 */
#include <stdio.h>
static int exec(const char *path,char *argv[],char *env[],int io_msqid,int do_fork) {
  int msqid = msgget(IPC_PRIVATE,IPC_CREAT);
  if (msqid!=-1) {
    size_t i,ins;
    struct msg *msg;
    size_t argv_size,env_size;

    // Send argv
    if (argv!=NULL) {
      ins = 0;
      for (i=0;argv[i];i++) ins += strlen(argv[i])+1;
      argv_size = ins+sizeof(struct msg);
      msg = malloc(argv_size);
      ins = 0;
      for (i=0;argv[i];i++) {
        strcpy((char*)(&(msg->buf)+ins),argv[i]);
        ins += strlen(argv[i])+1;
      }
      msg->type = EXEC_TYPE_ARGV;
      msgsnd(msqid,msg,argv_size,0);
      free(msg);
    }
    else argv_size = 0;

    // Send environ
    if (env!=NULL) {
      for (i=0;env[i];i++) ins += strlen(env[i])+1;
      env_size = ins+sizeof(struct msg);
      msg = malloc(env_size);
      ins = 0;
      for (i=0;env[i];i++) {
        strcpy((char*)(&(msg->buf)+ins),env[i]);
        ins += strlen(env[i])+1;
      }
      msg->type = EXEC_TYPE_ENVIRON;
      msgsnd(msqid,msg,env_size,0);
      free(msg);
    }
    else env_size = 0;

    // Send info message
    struct msg_info msg_info = {
      .type = EXEC_TYPE_INFO,
      .argv_size = argv_size,
      .env_size = env_size,
      .io_msqid = io_msqid
    };
    msgsnd(msqid,&msg_info,sizeof(msg_info),0);

    // Load file
    int fd = open(path,O_RDONLY);
    if (fd!=-1) {
      struct stat stbuf;
      fstat(fd,&stbuf);
      char *buf = memalign(PAGE_SIZE,stbuf.st_size);
      if (read(fd,buf,stbuf.st_size)==stbuf.st_size) {
        close(fd);

        // Replace process image
        char *path_copy = strdup(path);
        int ret = syscall_call(SYSCALL_PROC_EXEC,5,basename(path_copy),buf,stbuf.st_size,msqid,do_fork);
printf("TODO: %s line %d\n",__FILE__,__LINE__); while (1);
        free(path_copy);
        free(buf);
        return ret;
      }
    }
  }
  errno = EIO;
  return -1;
}

int _exec_getdata(int msqid,int *argc,char ***argv) {
  if (msqid!=-1 && 0) {
    // Receive info message
    struct msg_info msg_info;
    msgrcv(msqid,&msg_info,sizeof(msg_info),EXEC_TYPE_INFO,0);
    //_io_msgid = msg_info.io_msqid;

    // Load argv
    *argc = 0;
    *argv = NULL;

    // Load environ
    *environ = NULL;

    msgctl(msqid,IPC_RMID,NULL);
  }

  return 0;
}

int execv(const char *path,const char *argv[]) {
  return exec(path,(char**)argv,environ,0,0); ///< @todo FIX io_msqid
}

int execve(const char *path,const char *argv[],const char *env) {
  return exec(path,(char**)argv,(char**)env,0,0); ///< @todo FIX io_msqid
}

// in libmeinos-Header
int execute(const char *path,char *argv[]) {
  return exec(path,(char**)argv,NULL,0,1); ///< @todo FIX io_msqid
}
