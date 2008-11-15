/*
 * getopt.c
 *
 * Simple POSIX getopt(), no GNU extensions...
 *
 * Copyright (c)
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdint.h>
#include <unistd.h>
#include <string.h>

char *optarg;
int optind, opterr, optopt;
static struct getopt_private_state {
	const char *optptr;
	const char *last_optstring;
	char *const *last_argv;
} pvt;

int getopt(int argc, char *const *argv, const char *optstring)
{
	const char *carg;
	const char *osptr;
	int opt;

	/* getopt() relies on a number of different global state
	   variables, which can make this really confusing if there is
	   more than one use of getopt() in the same program.  This
	   attempts to detect that situation by detecting if the
	   "optstring" or "argv" argument have changed since last time
	   we were called; if so, reinitialize the query state. */

	if (optstring != pvt.last_optstring || argv != pvt.last_argv ||
	    optind < 1 || optind > argc) {
		/* optind doesn't match the current query */
		pvt.last_optstring = optstring;
		pvt.last_argv = argv;
		optind = 1;
		pvt.optptr = NULL;
	}

	carg = argv[optind];

	/* First, eliminate all non-option cases */

	if (!carg || carg[0] != '-' || !carg[1]) {
		return -1;
	}

	if (carg[1] == '-' && !carg[2]) {
		optind++;
		return -1;
	}

	if ((uintptr_t) (pvt.optptr - carg) > (uintptr_t) strlen(carg)) {
		/* Someone frobbed optind, change to new opt. */
		pvt.optptr = carg + 1;
	}

	opt = *pvt.optptr++;

	if (opt != ':' && (osptr = strchr(optstring, opt))) {
		if (osptr[1] == ':') {
			if (*pvt.optptr) {
				/* Argument-taking option with attached
				   argument */
				optarg = (char *)pvt.optptr;
				optind++;
			} else {
				/* Argument-taking option with non-attached
				   argument */
				if (argv[optind + 1]) {
					optarg = (char *)argv[optind+1];
					optind += 2;
				} else {
					/* Missing argument */
					optind++;
					return (optstring[0] == ':')
						? ':' : '?';
				}
			}
			return opt;
		} else {
			/* Non-argument-taking option */
			/* pvt.optptr will remember the exact position to
			   resume at */
			if (!*pvt.optptr)
				optind++;
			return opt;
		}
	} else {
		/* Unknown option */
		optopt = opt;
		if (!*pvt.optptr)
			optind++;
		return '?';
	}
}
