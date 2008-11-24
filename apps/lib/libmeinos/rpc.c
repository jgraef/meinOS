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
#include <syscall.h>
#include <errno.h>
#include <pack.h>
#include <stdarg.h>
#include <llist.h>
#include <rpc.h>
#include <misc.h>
#include <dyncall.h>
#include <unistd.h>

int rpc_func_create(const char *name,void *func,const char *synopsis,size_t paramsz) {
  int ret = syscall_call(SYSCALL_RPC_CREATE,4,name,func,synopsis,paramsz);
  errno = ret<0?-ret:0;
  return ret<0?-1:ret;
}

int rpc_func_destroy(int id) {
  int ret = syscall_call(SYSCALL_RPC_DESTROY,1,id);
  errno = ret<0?-ret:0;
  return ret<0?-1:ret;
}

int rpc_vcall(const char *name,int ret_params,va_list args) {
  int ret,id,error;
  char synopsis[RPC_SYNOPSIS_MAXLEN];
  size_t paramsz,i;
  pack_t params;
  id = syscall_call(SYSCALL_RPC_GETINFO,8,-1,name,0,0,&paramsz,synopsis,RPC_SYNOPSIS_MAXLEN,0);
  if (id>=0) {
    // pack parameters
    params = pack_malloc(paramsz);
    int *param_list = malloc(strlen(synopsis));
    for (i=0;synopsis[i];i++) {
      param_list[i] = va_arg(args,int);
      if (synopsis[i]=='b') pack8(params,param_list[i]);
      else if (synopsis[i]=='w') pack16(params,param_list[i]);
      else if (synopsis[i]=='d') pack32(params,param_list[i]);
      else if (synopsis[i]=='c') packc(params,param_list[i]);
      else if (synopsis[i]=='s') packs(params,param_list[i]);
      else if (synopsis[i]=='i') packi(params,param_list[i]);
      else if (synopsis[i]=='l') packl(params,param_list[i]);
      else if (synopsis[i]=='$') packstr(params,(const char*)param_list[i]);
    }

    // call function
    if (syscall_call(SYSCALL_RPC_CALL,5,id,pack_data(params),&ret,ret_params?pack_data(params):NULL,&error)==0) error = 1;

    if (error==0 || 1) { /// @todo fix error detection
      if (ret_params) {
        pack_reset(params);
        for (i=0;synopsis[i];i++) {
          int null;
          char *str;
          if (synopsis[i]=='b') unpack8(params,&null);
          else if (synopsis[i]=='w') unpack16(params,&null);
          else if (synopsis[i]=='d') unpack32(params,&null);
          else if (synopsis[i]=='c') unpackc(params,&null);
          else if (synopsis[i]=='s') unpacks(params,&null);
          else if (synopsis[i]=='i') unpacki(params,&null);
          else if (synopsis[i]=='l') unpackl(params,&null);
          else if (synopsis[i]=='$') {
            unpackstr(params,&str);
            strncpy((char*)param_list[i],str,strlen((char*)param_list[i])+1);
          }
        }
      }
      errno = 0;
/// @todo FIXME!
/*printf("FOO: 0x%x\n",param_list);
      free(param_list);
printf("BAR\n");*/
      pack_free(params);
      return ret;
    }
    else pack_free(params);
  }
  errno = EINVAL;
  return -1;
}

int rpc_call(const char *name,int ret_params,...) {
  va_list args;
  va_start(args,ret_params);
  int ret = rpc_vcall(name,ret_params,args);
  va_end(args);
  return ret;
}

int rpc_poll(int id) {
  size_t paramsz,num_params;
  char synopsis[RPC_SYNOPSIS_MAXLEN];
  pack_t params;
  int (*func)(void);
  int ret;

  id = syscall_call(SYSCALL_RPC_GETINFO,8,id,NULL,getpid(),1,&paramsz,synopsis,RPC_SYNOPSIS_MAXLEN,1);
  if (id>=0) {
    num_params = strlen(synopsis);
    params = pack_malloc(paramsz);
    if (syscall_call(SYSCALL_RPC_POLL,4,id,&func,pack_data(params),&rpc_curpid)==0) {
      int *param_list = malloc(num_params);
      size_t i = 0;
      while (synopsis[i]) {
        if (synopsis[i]=='b') unpack8(params,param_list+i);
        else if (synopsis[i]=='w') unpack16(params,param_list+i);
        else if (synopsis[i]=='d') unpack32(params,param_list+i);
        else if (synopsis[i]=='c') unpackc(params,param_list+i);
        else if (synopsis[i]=='s') unpacks(params,param_list+i);
        else if (synopsis[i]=='i') unpacki(params,param_list+i);
        else if (synopsis[i]=='l') unpackl(params,param_list+i);
        else if (synopsis[i]=='$') unpackstr(params,(char**)param_list+i);
        i++;
      }

      ret = dyn_call(func,param_list,num_params);

      if (syscall_call(SYSCALL_RPC_RETURN,3,id,ret,pack_data(params))==0) {
        errno = 0;
        return 0;
      }
    }
  }
  errno = EINVAL;
  return -1;
}

int rpc_mainloop(int id) {
  /// @todo I don't know if that is such a good idea
  init_ready();
  while (1) rpc_poll(id);
}

llist_t rpc_list() {
  llist_t list = llist_create();
  size_t i;
  for (i=0;;i++) {
    size_t size = syscall_call(SYSCALL_RPC_LIST,3,i,NULL,0);
    if (size==0) break;
    char *buf = malloc(size);
    syscall_call(SYSCALL_RPC_LIST,3,i,buf,size);
    llist_push(list,buf);
  }
  return list;
}
