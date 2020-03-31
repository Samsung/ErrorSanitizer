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

/* FILE *tmpfile(void); */

typedef FILE *(*tmpfile_func_t)(void);

static FILE *real_tmpfile(void)
{
	static tmpfile_func_t tmpfile_func_ptr = NULL;
	if (NULL == tmpfile_func_ptr)
		tmpfile_func_ptr = (tmpfile_func_t)dlsym(RTLD_NEXT, "tmpfile");
	if (NULL != tmpfile_func_ptr)
		return (*tmpfile_func_ptr)();

	exit_failure(ESAN_DLSYM_ERROR,
		     "Error in dlsym - in 'tmpfile' wrapper\n");
}

// parameter names starting with __ are reserved for standard library
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
FILE *tmpfile(void)
{
	if (esan_should_I_fail(__builtin_return_address(0), ESAN_TMPFILE)) {
		errno = ENFILE;
		return NULL;
	} else {
		return real_tmpfile();
	}
}
