/*
    Copyright (c) 2018 - 2020, Samsung Electronics Co., Ltd. All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

    Author: Mateusz Nosek <m.nosek@samsung.com>
*/
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "log.h"
#include <dlfcn.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned long skip = 2;

static void hook_init(void)
{
	static const char env_name[] = "SIGACTION_FAIL";
	static int done = 0;
	const char *env;

	if (done)
		return;
	done = 1;

	env = getenv(env_name);
	if (env)
		skip = strtoul(env, NULL, 0);
}

typedef int (*sigaction_func_t)(int signum, const struct sigaction *act,
				struct sigaction *oldact);

static int real_sigaction(int signum, const struct sigaction *act,
			  struct sigaction *oldact)
{
	static sigaction_func_t sigaction_func_ptr = NULL;
	if (NULL == sigaction_func_ptr)
		sigaction_func_ptr =
			(sigaction_func_t)dlsym(RTLD_NEXT, "sigaction");
	if (NULL != sigaction_func_ptr)
		return (*sigaction_func_ptr)(signum, act, oldact);

	exit_failure(
		ESAN_DLSYM_ERROR,
		"Error in dlsym - in 'sigaction' wrapper - in test library\n");
}

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
	hook_init();
	if (0 == skip) {
		return -1;
	} else {
		--skip;
		return real_sigaction(signum, act, oldact);
	}
}
