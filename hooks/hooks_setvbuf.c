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

/* int setvbuf(FILE *stream, char *buf, int mode, size_t size); */

typedef int (*setvbuf_func_t)(FILE *stream, char *buf, int mode, size_t size);

static int real_setvbuf(FILE *stream, char *buf, int mode, size_t size)
{
	static setvbuf_func_t setvbuf_func_ptr = NULL;
	if (NULL == setvbuf_func_ptr)
		setvbuf_func_ptr = (setvbuf_func_t)dlsym(RTLD_NEXT, "setvbuf");
	if (NULL != setvbuf_func_ptr)
		return (*setvbuf_func_ptr)(stream, buf, mode, size);

	exit_failure(ESAN_DLSYM_ERROR,
		     "Error in dlsym - in 'setvbuf' wrapper\n");
}

// parameter names starting with __ are reserved for standard library
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
int setvbuf(FILE *stream, char *buf, int mode, size_t size)
{
	if (esan_should_I_fail(__builtin_return_address(0), ESAN_SETVBUF)) {
		return EOF;
	} else {
		return real_setvbuf(stream, buf, mode, size);
	}
}
