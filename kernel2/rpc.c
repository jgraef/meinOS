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

#include <rpc.h>
#include <procm.h>
#include <string.h>
#include <errno.h>
#include <llist.h>
#include <debug.h>
#include <syscall.h>
#include <interrupt.h>

#include <cpu.h>

int rpc_init() {
  rpc_functions = llist_create();
  rpc_lastid = 0;
  if (syscall_create(SYSCALL_RPC_CREATE,rpc_function_create,4)==-1) return -1;
  if (syscall_create(SYSCALL_RPC_DESTROY,rpc_function_destroy,1)==-1) return -1;
  if (syscall_create(SYSCALL_RPC_GETINFO,rpc_function_getinfo,8)==-1) return -1;
  if (syscall_create(SYSCALL_RPC_CALL,rpc_function_call,5)==-1) return -1;
  if (syscall_create(SYSCALL_RPC_POLL,rpc_call_poll,4)==-1) return -1;
  if (syscall_create(SYSCALL_RPC_RETURN,rpc_call_return,3)==-1) return -1;
  if (syscall_create(SYSCALL_RPC_LIST,rpc_list,3)==-1) return -1;
  return 0;
}

/**
 * Creates a RPC function (Syscall)
 *  @param name Function name
 *  @param rpc_handler RPC handler
 *  @param func Function pointer
 *  @param synopsis Funtion synopsis
 *  @return Function ID
 */
int rpc_function_create(char *name,void *func,char *synopsis,size_t params_size) {
  rpc_function_t *new = malloc(sizeof(rpc_function_t));
  new->id = rpc_lastid++;
  new->name = strdup(name);
  new->func = func;
  new->synopsis = strdup(synopsis);
  new->owner = proc_current;
  new->calls = llist_create();
  new->params_size = params_size;
  llist_push(rpc_functions,new);
  return new->id;
}

/**
 * Destroys a RPC function (Syscall)
 *  @param id Function ID
 *  @return Success?
 */
int rpc_function_destroy(int id) {
  rpc_function_t *func = rpc_find(id,NULL,NULL,0,0);
  if (func!=NULL) {
    if (func->owner==proc_current) {
      free(func->name);
      free(func->synopsis);
      rpc_call_t *call;
      while ((call = llist_pop(func->calls))) rpc_call_destroy(call);
      return 0;
    }
    else return -EACCES;
  }
  else return -EINVAL;
}

/**
 * Gets information about RPC function (Syscall)
 *  @param id Function ID (or -1)
 *  @param name Function name (or NULL)
 *  @param pid Function owner's PID (or 0)
 *  @param has_calls Whether function must have calls
 *  @param params_size Reference for size of params
 *  @param synopsis Buffer for synopsis
 *  @param maxlen Maxlen for synopsis
 *  @param sleep Whether to sleep til next found (works only for owner)
 *  @return Function ID
 */
int rpc_function_getinfo(int id,char *name,pid_t pid,int has_calls,size_t *params_size,char *synopsis,size_t maxlen,int sleep) {
  rpc_function_t *func = rpc_find(id,name,NULL,pid,has_calls);
  if (func!=NULL) {
    strncpy(synopsis,func->synopsis,maxlen);
    *params_size = func->params_size;
    return func->id;
  }
  else if (sleep && has_calls && pid==proc_current->pid) {
    *(interrupt_curregs.eax) = -1;
    proc_sleep(proc_current);
    return -1;
  }
  else return -EINVAL;
}

/**
 * Calls a RPC function (Syscall)
 *  @param id Function ID
 *  @param params Parameters
 *  @param ret Reference for return value
 *  @param ret_params Reference for return parameters (can be NULL)
 *  @param error Reference for RPC error
 *  @param sleep Sleep Reference
 *  @return Success?
 */
int rpc_function_call(int id,void *params,int *ret,void *ret_params,int *error) {
  rpc_function_t *func = rpc_find(id,NULL,NULL,0,0);
  if (func!=NULL) {
    rpc_call_t *new = malloc(sizeof(rpc_call_t));
    new->caller = proc_current;
    new->ret = ret;
    new->ret_params = ret_params;
    new->params = memcpy(malloc(func->params_size),params,func->params_size);
    new->error = error;
    llist_push(func->calls,new);
    if (error!=NULL) *error = 1;
    proc_wake(func->owner);
    *(interrupt_curregs.eax) = 0;
    proc_sleep(proc_current);
    return 0;
  }
  else return -EINVAL;
}

/**
 * Destroys a RPC Call
 *  @param call RPC Call
 *  @return Success?
 */
int rpc_call_destroy(rpc_call_t *call) {
  free(call->params);
  free(call);
  return 0;
}

/**
 * Checks for new calls (Syscall)
 *  @param id Function ID (or -1)
 *  @param func Reference for function pointer
 *  @param params Buffer for params
 *  @param caller Reference for caller PID
 *  @return Success?
 */
int rpc_call_poll(int id,void **func_ptr,void *params,pid_t *caller) {
  rpc_function_t *func = rpc_find(-1,NULL,proc_current,0,1);
  if (func!=NULL) {
    rpc_call_t *call = llist_pop(func->calls);
    func->current = call;
    *func_ptr = func->func;
    *caller = call->caller->pid;
    memcpy(params,call->params,func->params_size);
    return 0;
  }
  else return -EINVAL;
}

/**
 * Returns from a RPC functions (Syscall)
 *  @param id Function ID (or -1)
 *  @param ret Return value
 *  @param ret_params Return params
 *  @return Success?
 */
int rpc_call_return(int id,int ret,void *ret_params) {
  rpc_function_t *func = rpc_find(id,NULL,proc_current,0,0);
  if (func!=NULL) {
    rpc_call_t *call = func->current;
    if (call!=NULL) {
      if (call->ret_params!=NULL) {
        void *buf = memcpy(malloc(func->params_size),ret_params,func->params_size);
        memuser_load_addrspace(call->caller->addrspace);
        memcpy(call->ret_params,buf,func->params_size);
      }
      else memuser_load_addrspace(call->caller->addrspace);
      if (call->ret!=NULL) *(call->ret) = ret;
      if (call->error!=NULL) *(call->error) = 0;
      memuser_load_addrspace(proc_current->addrspace);
      proc_wake(call->caller);
      rpc_call_destroy(call);
      func->current = NULL;
      return 0;
    }
  }
  return -EINVAL;
}

/**
 * Finds a RPC function
 *  @param id Function ID (or -1)
 *  @param name Function name (or NULL)
 *  @param owner Function owner (or NULL)
 *  @param pid Function owner's PID (or 0)
 *  @param has_calls Whether to check if functions is called
 *  @return RPC function
 */
rpc_function_t *rpc_find(int id,char *name,proc_t *owner,pid_t pid,int has_calls) {
  rpc_function_t *func;
  size_t i;
  for (i=0;(func = llist_get(rpc_functions,i));i++) {
    if ((id==-1 || id==func->id) && (name==NULL || strcmp(name,func->name)==0) && (owner==NULL || owner==func->owner) && (pid==0 || pid==func->owner->pid) && (has_calls==0 || !llist_empty(func->calls))) return func;
  }
  return NULL;
}

/**
 * "Lists" RPC function.
 *  @param i Index of current function
 *  @param buf Buffer for name
 *  @param bufsize Size of buffer
 *  @return Count of copied bytes
 */
size_t rpc_list(size_t i,char *buf,size_t bufsize) {
  rpc_function_t *func;
  func = llist_get(rpc_functions,i);
  if (func!=NULL) {
    if (buf==NULL || bufsize==0) { // only return size
      return strlen(func->name)+1;
    }
    else {
      strncpy(buf,func->name,bufsize);
      return strlen(buf)+1;
    }
  }
  else return 0;
}
