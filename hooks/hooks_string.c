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
#include "hooks_include.h"

/**************************************************************************************/

/* char *strdup(const char *s); */

typedef char *(*strdup_func_t)(const char *s);

char *real_strdup(const char *s)
{
	static strdup_func_t strdup_func_ptr = NULL;
	if (NULL == strdup_func_ptr)
		strdup_func_ptr = (strdup_func_t)dlsym(RTLD_NEXT, "strdup");
	if (NULL != strdup_func_ptr)
		return (*strdup_func_ptr)(s);

	log("Error in dlsym - in 'strdup' wrapper\n");
	exit(-1);
}

char *strdup(const char *s)
{
	if (esan_should_I_fail(__builtin_return_address(0), ESAN_STRDUP)) {
		errno = ENOMEM;
		return NULL;
	} else {
		return real_strdup(s);
	}
}

/**************************************************************************************/

/* char *strndup(const char *s, size_t n); */

typedef char *(*strndup_func_t)(const char *s, size_t n);

char *real_strndup(const char *s, size_t n)
{
	static strndup_func_t strndup_func_ptr = NULL;
	if (NULL == strndup_func_ptr)
		strndup_func_ptr = (strndup_func_t)dlsym(RTLD_NEXT, "strndup");
	if (NULL != strndup_func_ptr)
		return (*strndup_func_ptr)(s, n);

	log("Error in dlsym - in 'strndup' wrapper\n");
	exit(-1);
}

// parameter names starting with __ are reserved for standard library
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
char *strndup(const char *s, size_t n)
{
	if (esan_should_I_fail(__builtin_return_address(0), ESAN_STRNDUP)) {
		errno = ENOMEM;
		return NULL;
	} else {
		return real_strndup(s, n);
	}
}
