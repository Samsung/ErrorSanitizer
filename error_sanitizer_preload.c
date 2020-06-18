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

    Author: Ernest Borowski <e.borowski@samsung.com>
    Author: Jakub Botwicz <j.botwicz@samsung.com>
    Author: Mateusz Nosek <m.nosek@samsung.com>
*/
#include "error_sanitizer.h"
#include "error_sanitizer_preload.h"
#include "esan_mutex.h"
#include "esan_signal.h"
#include "in_library.h"
#include "log.h"
#include "stats.h"

void lib_init(int argc, char **argv, char **envp)
{
	static unsigned execute_once = 0;

	esan_mutex_lock();
	if (execute_once++) {
		log("lib_init called %u times.", execute_once);
		esan_mutex_unlock();
		return;
	}
	esan_signal_initialize();
	parse_map(argc, argv, (const char *const *)envp);
	in_library_initialize();
	esan_enable_map_based_failure();
	esan_mutex_unlock();
}

void lib_exit(void)
{
	static unsigned execute_once = 0;

	esan_mutex_lock();
	if (execute_once++) {
		log("lib_exit called %u times.", execute_once);
		esan_mutex_unlock();
		return;
	}
	esan_disable_failure();
#ifndef AFL
	esan_print_stats();
#endif
	parse_map_cleanup();
	in_library_destructor();
#ifndef AFL
	if (stderr && fileno(stderr) >= 0)
		fflush(stderr);
#endif
	esan_signal_destructor();
	esan_mutex_unlock();
}
