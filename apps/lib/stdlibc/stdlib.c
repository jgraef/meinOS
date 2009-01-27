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
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <llist.h>
#include <syscall.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>

static llist_t atexit_list;

// some variables/functions needed
void _stdio_init();            ///< @see stdio.h
void _close_all_filehandles(); ///< @see files.c
void _signal_init();           ///< @see signal.c
void _fs_init();               ///< @see apps/lib/stdlibc/files.c

void _stdlib_init() {
  rand_seed = rand_seed%RAND_MAX;
  errno = 0;
  atexit_list = llist_create();
  environ = malloc(sizeof(char*));
  environ[0] = NULL;
  _signal_init();
  _fs_init();
  _stdio_init();
}

/**
 * Exits process
 *  @param result Program result value
 *  @see also used by crt0.asm
 */
void exit(int result) {
  void (*func)(void);
  while ((func = llist_pop(atexit_list))!=NULL) func();
  _Exit(result);
}

/**
 * Closes all filehandles and exits
 *  @param result Program result
 */
void _Exit(int result) {
  _close_all_filehandles();
  syscall_call(SYSCALL_PROC_EXIT,1,result);
}

/**
 * Registers a function to run at process termination
 *  @param func Function
 *  @return 0=successful; -1=failed
 */
int atexit(void (*func)(void)) {
  return llist_push(atexit_list,func)!=NULL?0:-1;
}

/**
 * Converts a string to a number
 *  @param str String representing a number
 *  @return Represented number from string
 */
int atoi(const char *str) {
  size_t len,i;
  int num = 0;
  int sign = 0;

  len = strlen(str);
  for (i=0;i<len;i++) {
    if (str[i]=='+') sign = 0;
    else if (str[i]=='-') sign = 1;
    else num = num*10+str[i]-'0';
  }
  if (sign) num *= -1;
  return num;
}

/**
 * Returns absolute number
 *  @param num Number to get absolute number of
 *  @return Absolute number
 */
int abs(int num) {
  if (num<0) num = -num;
  return num;
}

/**
 * Devides a number
 *  @param numer Numer
 *  @param denom Denom
 *  @return Devision result
 */
div_t div(int numer, int denom) {
  div_t res;
  res.quot = numer/denom;
  res.rem = numer%denom;
  return res;
}

/**
 * Binary search a sorted table
 *  @param vdkey Key
 *  @param vdbase Base
 *  @param nel Num elements
 *  @param width Size of an element
 *  @param compar Function called when match found
 */
void *bsearch(const void *vdkey,const void *vdbase,size_t nel,size_t width,int (*compar)(const void *, const void *)) {
  char *key = (char*)vdkey;
  char *cpkey = memcpy(malloc(width),key,width);
  char *base = (char*)vdbase;
  void *last = NULL;
  size_t found = 0;
  size_t i;

  for (i=0;i<nel;i++) {
    if (*(base+nel*width)==*key) found++;
    if (found==width) {
      char *cpbase = memcpy(malloc(width),base,width);
      if (compar!=NULL) compar(key,base);
      *key = *cpkey;
      *base = *cpbase;
      free(cpbase);
      found = 0;
      last = (void*)(vdbase+nel*width);
    }
  }

  free(cpkey);
  return last;
}

/**
 * Gets a new page from kernel
 *  @param size How many bytes (should be devidable by PAGESIZE)
 *  @return Pointer to new page
 */
void *sbrk(intptr_t size) {
  return (void*)syscall_call(SYSCALL_MEM_MALLOC,1,size);
}

/// @todo FIXME
int rand() {
  return 0xD00FC0DE;
  const int N = 624;
  const int M = 397;
  const unsigned A[2] = { 0, 0x9908b0df };
  const unsigned HI = 0x80000000;
  const unsigned LO = 0x7fffffff;

  static unsigned *y;
  static int init = 0;
  static int index;
  if (!init) {
    index = N;
    init = 1;
  }

  if (index >= N) {
    if (index > N) {
       // initialisiere y mit Pseudozufallszahlen:
       y = calloc(N,sizeof(unsigned int));
       unsigned r = 9, s = 3402;
       int i;
       for (i=0 ; i<N ; ++i) {
          r = 509845221 * r + 3;
          s *= s + 1;
          y[i] = s + (r >> 10);
       }
    }
    unsigned h;
    int k;
    for (k=0 ; k<N-M ; ++k) {
       h = (y[k] & HI) | (y[k+1] & LO);
       y[k] = y[k+M] ^ (h >> 1) ^ A[h & 1];
    }
    for (k=N-M ; k<N-1 ; ++k) {
       h = (y[k] & HI) | (y[k+1] & LO);
       y[k] = y[k+(M-N)] ^ (h >> 1) ^ A[h & 1];
    }
    h = (y[N-1] & HI) | (y[0] & LO);
    y[N-1] = y[M-1] ^ (h >> 1) ^ A[h & 1];
    index = 0;
  }

  unsigned e = y[index++];
  // tempering:
  e ^= (e >> 11);
  e ^= (e << 7) & 0x9d2c5680;
  e ^= (e << 15) & 0xefc60000;
  e ^= (e >> 18);
  return e%RAND_MAX;
}

/**
 * Issues a (shell) command
 *  @param command Shell command
 * @todo change to /bin/sh or something not fixed (eg. read the SHELL environment variable or use execlp)
 */
int system(const char *command) {
  if (command==NULL) {
    return execl("/boot/bin/sh","sh","--version",NULL);
  }
  else {
    pid_t pid = fork();
    if (pid==0) { // child
      execl("/boot/bin/sh","sh","-c",command,NULL);
      return 0;
    }
    else { // parent
      int stat = 0;
      waitpid(pid,&stat,0);
      return stat;
    }
  }
}