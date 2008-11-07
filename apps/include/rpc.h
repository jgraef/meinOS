#ifndef _RPC_H_
#define _RPC_H_

#include <sys/cdefs.h>
#include <sys/types.h>
#include <limits.h>
#include <stdarg.h>
#include <llist.h>

#define RPC_SYNOPSIS_MAXLEN NAME_MAX

#define rpc_func(func,synopsis,paramsz) rpc_func_create(__STRING(func),func,synopsis,paramsz)

pid_t rpc_curpid;

int rpc_func_create(const char *name,void *func,const char *synopsis,size_t paramsz);
int rpc_func_destroy(int id);
int rpc_vcall(const char *name,int ret_params,va_list args);
int rpc_call(const char *name,int ret_params,...);
int rpc_poll(int id);
int rpc_mainloop(int id);
llist_t rpc_list();

#endif
