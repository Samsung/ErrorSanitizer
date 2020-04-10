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

/* void setbuf(FILE *stream, char *buf); */

typedef void (*setbuf_func_t)(FILE *stream, char *buf);

static void real_setbuf(FILE *stream, char *buf)
{
	static setbuf_func_t setbuf_func_ptr = NULL;
	if (NULL == setbuf_func_ptr)
		setbuf_func_ptr = (setbuf_func_t)dlsym(RTLD_NEXT, "setbuf");
	if (NULL != setbuf_func_ptr) {
		(*setbuf_func_ptr)(stream, buf);
		return;
	}

	exit_failure(ESAN_DLSYM_ERROR,
		     "Error in dlsym - in 'setbuf' wrapper\n");
}

// parameter names starting with __ are reserved for standard library
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
void setbuf(FILE *stream, char *buf)
{
	if (esan_should_I_fail(__builtin_return_address(0), ESAN_SETBUF)) {
		exit_failure(
			ESAN_DEPRECATED,
			"setbuf is obsolete function - use setvbuf instead");
	} else {
		real_setbuf(stream, buf);
	}
}
