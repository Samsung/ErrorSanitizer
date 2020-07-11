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
#include "esan_visibility.h"
#include "log.h"
#include "stats.h"
#include <signal.h>
#include <stdlib.h>

#ifdef AFL
#define ESAN_NO_SIGNALS
#endif

#ifndef ESAN_NO_SIGNALS
#define ESAN_MAX_SIGNALS ((SIGSEGV > SIGILL ? SIGSEGV : SIGILL) + 1)
static struct sigaction old_signals[ESAN_MAX_SIGNALS];
static void handler(int sig, siginfo_t *info, void *data)
{
	(void)data;
	(void)info;
	esan_print_stats();
	if (0 != sigaction(sig, &old_signals[sig], NULL))
		exit_failure(ESAN_LIBRARY_FUNCTION_ERROR,
			     "Restoring signal handler failed!");
	raise(sig);
}

HIDE void esan_signal_initialize(void)
{
	struct sigaction info;
	int ret;

	info.sa_sigaction = handler;
	info.sa_flags = SA_SIGINFO;

	ret = sigaction(SIGSEGV, &info, &old_signals[SIGSEGV]);
	if (0 == ret)
		ret = sigaction(SIGILL, &info, &old_signals[SIGILL]);
	if (0 != ret)
		exit_failure(ESAN_LIBRARY_FUNCTION_ERROR,
			     "Unable to set signal handlers!");
}

HIDE void esan_signal_destructor(void)
{
	int ret;
	ret = sigaction(SIGSEGV, &old_signals[SIGSEGV], NULL);
	if (0 == ret)
		ret = sigaction(SIGILL, &old_signals[SIGILL], NULL);
	if (0 != ret)
		exit_failure(ESAN_LIBRARY_FUNCTION_ERROR,
			     "Unable to restore signal handlers!");
}
#endif
