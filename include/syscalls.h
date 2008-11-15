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

#define SYSCALL_INT               0x37
#define SYSCALL_MAXNUM              67

// Memory
#define SYSCALL_MEM_MALLOC           1
#define SYSCALL_MEM_FREE             2
#define SYSCALL_MEM_GETPHYSADDR      3
#define SYSCALL_MEM_DMA_ALLOC        4
#define SYSCALL_MEM_DMA_FREE         5
#define SYSCALL_MEM_GETVGA           6

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
#define SYSCALL_PROC_FORK           40
#define SYSCALL_PROC_GETPID         41
#define SYSCALL_PROC_GETUID         42
#define SYSCALL_PROC_GETGID         43
#define SYSCALL_PROC_SETUID         44
#define SYSCALL_PROC_SETGID         45
#define SYSCALL_PROC_GETPARENT      46
#define SYSCALL_PROC_GETNAME        47
#define SYSCALL_PROC_GETPIDBYNAME   48
#define SYSCALL_PROC_GETVAR         49
#define SYSCALL_PROC_EXIT           50
#define SYSCALL_PROC_ABORT          51
#define SYSCALL_PROC_STOP           52
#define SYSCALL_PROC_EXEC           53

// I/O port
#define SYSCALL_IO_REG              54
#define SYSCALL_IO_UNREG            55

// Timer
#define SYSCALL_TIME_HANDLER        56
#define SYSCALL_TIME_SLEEP          57
#define SYSCALL_TIME_USLEEP         58
#define SYSCALL_TIME_GETTICKS       59

// IRQ
#define SYSCALL_IRQ_HANDLER         60
#define SYSCALL_IRQ_SLEEP           61

// Signal
#define SYSCALL_SIG_SETUP           62
#define SYSCALL_SIG_SEND            63

// Misc
#define SYSCALL_MISC_BIOSINT        64

// Debug
/// @deprecated Only for debugging
#define SYSCALL_PUTSN               65
#define SYSCALL_FOURTYTWO           66

#endif
