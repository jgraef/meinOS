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

#ifndef _RPC_H_
#define _RPC_H_

#include <sys/cdefs.h>
#include <sys/types.h>
#include <limits.h>
#include <stdarg.h>
#include <llist.h>

#define RPC_SYNOPSIS_MAXLEN NAME_MAX

#define RPC_TRIES_MAXNUM   100

#define RPC_FLAG_RETPARAMS 1
#define RPC_FLAG_SENDTO    2
#define RPC_FLAG_NORET     4

#define rpc_func(func,synopsis,paramsz) rpc_func_create(__STRING(func),func,synopsis,paramsz)

pid_t rpc_curpid;

int rpc_func_create(const char *name,void *func,const char *synopsis,size_t paramsz);
int rpc_func_destroy(int id);
int rpc_vcall(const char *name,int flags,va_list args);
int rpc_call(const char *name,int flags,...);
int rpc_vcallself(const char *name,void *func,int ret_params,va_list args);
int rpc_callself(const char *name,void *func,int ret_params,...);
int rpc_poll(int id);
int rpc_mainloop(int id);
llist_t rpc_list();

#endif
