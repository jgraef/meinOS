#ifndef _RPC_H_
#define _RPC_H_

#include <sys/types.h>
#include <procm.h>
#include <llist.h>

typedef struct {
  proc_t *caller;
  void *params;
  int *ret;
  void *ret_params;
  int *error;
} rpc_call_t;

typedef struct {
  id_t id;
  char *name;
  void *func;
  char *synopsis;
  size_t params_size;
  proc_t *owner;
  llist_t calls;
  rpc_call_t *current;
} rpc_function_t;

int rpc_lastid;
llist_t rpc_functions;

int rpc_init();
int rpc_function_create(char *name,void *func,char *synopsis,size_t params_size);
int rpc_function_destroy(int id);
int rpc_function_getinfo(int id,char *name,pid_t pid,int has_calls,size_t *params_size,char *synopsis,size_t maxlen,int sleep);
int rpc_function_call(int id,void *params,int *ret,void *ret_params,int *error);
int rpc_call_destroy(rpc_call_t *call);
int rpc_call_poll(int id,void **func,void *params,pid_t *caller);
int rpc_call_return(int id,int ret,void *ret_params);
rpc_function_t *rpc_find(int id,char *name,proc_t *owner,pid_t pid,int has_calls);
int rpc_synopsis_check(char *synopsis);
size_t rpc_list(size_t i,char *buf,size_t bufsize);

#endif
