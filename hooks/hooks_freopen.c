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
#include "hooks_include.h"

/* FILE *freopen(const char *filename, const char *mode, FILE *stream); */

typedef FILE *(*freopen_func_t)(const char *filename, const char *mode,
				FILE *stream);

static FILE *real_freopen(const char *filename, const char *mode, FILE *stream)
{
	static freopen_func_t freopen_func_ptr = NULL;
	if (NULL == freopen_func_ptr)
		freopen_func_ptr = (freopen_func_t)dlsym(RTLD_NEXT, "freopen");
	if (NULL != freopen_func_ptr)
		return (*freopen_func_ptr)(filename, mode, stream);

	exit_failure(ESAN_DLSYM_ERROR,
		     "Error in dlsym - in 'freopen' wrapper\n");
}

// parameter names starting with __ are reserved for standard library
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
FILE *freopen(const char *filename, const char *mode, FILE *stream)
{
	if (esan_should_I_fail(__builtin_return_address(0), ESAN_FREOPEN)) {
		errno = ENOMEM;
		return NULL;
	} else {
		return real_freopen(filename, mode, stream);
	}
}
