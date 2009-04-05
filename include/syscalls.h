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

#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#define SYSCALL_INT                 0x37
#define SYSCALL_MAXNUM              (SYSCALL_FOURTYTWO+1)

// Memory
#define SYSCALL_MEM_MALLOC           1
#define SYSCALL_MEM_FREE             2
#define SYSCALL_MEM_GETPHYSADDR      3
#define SYSCALL_MEM_DMA_ALLOC        4 /* ONLY FOR ROOT */
#define SYSCALL_MEM_DMA_FREE         5 /* ONLY FOR ROOT */
#define SYSCALL_MEM_GETVGA           6 /* ONLY FOR ROOT */

// IPC
// Message Passing
#define SYSCALL_IPC_MSG_GET          7
#define SYSCALL_IPC_MSG_CREATE       8
#define SYSCALL_IPC_MSG_DESTROY      9
#define SYSCALL_IPC_MSG_SEND        10
#define SYSCALL_IPC_MSG_RECV        11
#define SYSCALL_IPC_MSG_STAT        12
#define SYSCALL_IPC_MSG_SET         13
// Semaphore
#define SYSCALL_IPC_SEM_GET         14
#define SYSCALL_IPC_SEM_CREATE      15
#define SYSCALL_IPC_SEM_OP          16
#define SYSCALL_IPC_SEM_GETVAL      17
#define SYSCALL_IPC_SEM_SETVAL      18
#define SYSCALL_IPC_SEM_GETPID      19
#define SYSCALL_IPC_SEM_GETCNT      20
#define SYSCALL_IPC_SEM_GETALL      21
#define SYSCALL_IPC_SEM_SETALL      22
#define SYSCALL_IPC_SEM_STAT        23
#define SYSCALL_IPC_SEM_SET         24
#define SYSCALL_IPC_SEM_DESTROY     25
// Shared Memory
#define SYSCALL_IPC_SHM_GET         26
#define SYSCALL_IPC_SHM_CREATE      27
#define SYSCALL_IPC_SHM_ATTACH      28
#define SYSCALL_IPC_SHM_DETACH      29
#define SYSCALL_IPC_SHM_STAT        30
#define SYSCALL_IPC_SHM_SET         31
#define SYSCALL_IPC_SHM_DESTROY     32

// RPC
#define SYSCALL_RPC_CREATE          33
#define SYSCALL_RPC_DESTROY         34
#define SYSCALL_RPC_GETINFO         35
#define SYSCALL_RPC_CALL            36
#define SYSCALL_RPC_POLL            37
#define SYSCALL_RPC_RETURN          38
#define SYSCALL_RPC_LIST            39

// Process
#define SYSCALL_PROC_GETPID         40
#define SYSCALL_PROC_GETUID         41
#define SYSCALL_PROC_GETGID         42
#define SYSCALL_PROC_SETUID         43
#define SYSCALL_PROC_SETGID         44
#define SYSCALL_PROC_GETPARENT      45
#define SYSCALL_PROC_GETCHILD       46
#define SYSCALL_PROC_GETNAME        47
#define SYSCALL_PROC_SETNAME        48 /* ONLY FOR SYSTEM */
#define SYSCALL_PROC_GETPIDBYNAME   49
#define SYSCALL_PROC_GETVAR         50
#define SYSCALL_PROC_SETVAR         51
#define SYSCALL_PROC_EXIT           52
#define SYSCALL_PROC_ABORT          53
#define SYSCALL_PROC_STOP           54
#define SYSCALL_PROC_CREATE         55 /* ONLY FOR SYSTEM */
#define SYSCALL_PROC_DESTROY        56 /* ONLY FOR SYSTEM */
#define SYSCALL_PROC_MEMMAP         57 /* ONLY FOR SYSTEM */
#define SYSCALL_PROC_MEMALLOC       58 /* ONLY FOR SYSTEM */
#define SYSCALL_PROC_MEMUNMAP       59 /* ONLY FOR SYSTEM */
#define SYSCALL_PROC_MEMFREE        60 /* ONLY FOR SYSTEM */
#define SYSCALL_PROC_MEMFINDFREE    61 /* ONLY FOR SYSTEM */
#define SYSCALL_PROC_MEMGET         62 /* ONLY FOR SYSTEM */
#define SYSCALL_PROC_MEMPAGELIST    63 /* ONLY FOR SYSTEM */
#define SYSCALL_PROC_SYSTEM         64 /* ONLY FOR SYSTEM */
#define SYSCALL_PROC_JUMP           65 /* ONLY FOR SYSTEM */
#define SYSCALL_PROC_CREATESTACK    66 /* ONLY FOR SYSTEM */
#define SYSCALL_PROC_GETSTACK       67 /* ONLY FOR SYSTEM */
#define SYSCALL_PROC_SETSTACK       68 /* ONLY FOR SYSTEM */
#define SYSCALL_PROC_WAITPID        69

// I/O port
#define SYSCALL_IO_REG              70 /* ONLY FOR ROOT */
#define SYSCALL_IO_UNREG            71 /* ONLY FOR ROOT */

// Timer
#define SYSCALL_TIME_HANDLER        72
#define SYSCALL_TIME_SLEEP          73
#define SYSCALL_TIME_USLEEP         74
#define SYSCALL_TIME_GETTICKS       75

// IRQ
#define SYSCALL_IRQ_HANDLER         76 /* ONLY FOR ROOT */
#define SYSCALL_IRQ_SLEEP           77 /* ONLY FOR ROOT */

// Signal
#define SYSCALL_SIG_SETUP           78
#define SYSCALL_SIG_SEND            79

// Misc
#define SYSCALL_MISC_BIOSINT        80 /* ONLY FOR ROOT */

// Debug
/// @deprecated Only for debugging
#define SYSCALL_PUTSN               81
#define SYSCALL_FOURTYTWO           82

#endif
