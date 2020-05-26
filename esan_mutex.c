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
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <pthread.h>
#include <stdlib.h>

static pthread_mutex_t esan_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

HIDE void esan_mutex_lock(void)
{
	if (0 != pthread_mutex_lock(&esan_mutex))
		exit_failure(ESAN_LIBRARY_FUNCTION_ERROR,
			     "Unable to lock the mutex");
}

HIDE void esan_mutex_unlock(void)
{
	if (0 != pthread_mutex_unlock(&esan_mutex))
		exit_failure(ESAN_LIBRARY_FUNCTION_ERROR,
			     "Unable to unlock the mutex");
}
