/* vi: set sw=4 ts=4: */
/*
 * crypt() for uClibc
 * Copyright (C) 2008 by Erik Andersen <andersen@uclibc.org>
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define __FORCE_GLIBC
#include <crypt.h>
#include <unistd.h>
#include "libcrypt.h"
#include <errno.h>

char *crypt(const char *key, const char *salt)
{
	errno = ENOSYS;
	return NULL;
}

void
setkey(const char *key)
{
	errno = ENOSYS;
}

void
encrypt(char *block, int flag)
{
	errno = ENOSYS;
}
