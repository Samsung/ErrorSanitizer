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

/* int fgetpos(FILE * file, fpos_t * pos); */

typedef int (*fgetpos_func_t)(FILE *file, fpos_t *pos);

static int real_fgetpos(FILE *file, fpos_t *pos)
{
	static fgetpos_func_t fgetpos_func_ptr = NULL;
	if (NULL == fgetpos_func_ptr)
		fgetpos_func_ptr = (fgetpos_func_t)dlsym(RTLD_NEXT, "fgetpos");
	if (NULL != fgetpos_func_ptr)
		return (*fgetpos_func_ptr)(file, pos);

	exit_failure(ESAN_DLSYM_ERROR,
		     "Error in dlsym - in 'fgetpos' wrapper\n");
}

// parameter names starting with __ are reserved for standard library
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
int fgetpos(FILE *file, fpos_t *pos)
{
	if (esan_should_I_fail(__builtin_return_address(0), ESAN_FGETPOS)) {
		errno = EIO;
		return -1;
	} else {
		return real_fgetpos(file, pos);
	}
}

/**************************************************************************************/

/* int fseek(FILE * file, long int off, int whence); */

typedef int (*fseek_func_t)(FILE *file, long int off, int whence);

static int real_fseek(FILE *file, long int off, int whence)
{
	static fseek_func_t fseek_func_ptr = NULL;
	if (NULL == fseek_func_ptr)
		fseek_func_ptr = (fseek_func_t)dlsym(RTLD_NEXT, "fseek");
	if (NULL != fseek_func_ptr)
		return (*fseek_func_ptr)(file, off, whence);

	exit_failure(ESAN_DLSYM_ERROR, "Error in dlsym - in 'fseek' wrapper\n");
}

// parameter names starting with __ are reserved for standard library
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
int fseek(FILE *file, long int off, int whence)
{
	if (esan_should_I_fail(__builtin_return_address(0), ESAN_FSEEK)) {
		errno = EIO;
		return -1;
	} else {
		return real_fseek(file, off, whence);
	}
}

/**************************************************************************************/

/* int fsetpos(FILE * file, const fpos_t *pos); */

typedef int (*fsetpos_func_t)(FILE *file, const fpos_t *pos);

static int real_fsetpos(FILE *file, const fpos_t *pos)
{
	static fsetpos_func_t fsetpos_func_ptr = NULL;
	if (NULL == fsetpos_func_ptr)
		fsetpos_func_ptr = (fsetpos_func_t)dlsym(RTLD_NEXT, "fsetpos");
	if (NULL != fsetpos_func_ptr)
		return (*fsetpos_func_ptr)(file, pos);

	exit_failure(ESAN_DLSYM_ERROR,
		     "Error in dlsym - in 'fsetpos' wrapper\n");
}

// parameter names starting with __ are reserved for standard library
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
int fsetpos(FILE *file, const fpos_t *pos)
{
	if (esan_should_I_fail(__builtin_return_address(0), ESAN_FSETPOS)) {
		errno = EIO;
		return -1;
	} else {
		return real_fsetpos(file, pos);
	}
}
