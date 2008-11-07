#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <stdio.h>
#include <sys/types.h>
#include <wchar.h>
#include <devfs.h>

// keyboard.c
int init_keyboard();
ssize_t onread(devfs_dev_t *dev,void *buffer,size_t count,off_t offset);

// screen.c
int init_screen();
ssize_t onwrite(devfs_dev_t *dev,void *buffer,size_t count,off_t offset);

#endif
