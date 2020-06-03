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

/* FILE* fopen( const char* filename, const char* mode); */

typedef FILE *(*fopen_func_t)(const char *filename, const char *mode);

static FILE *real_fopen(const char *filename, const char *mode)
{
	static fopen_func_t fopen_func_ptr = NULL;
	if (NULL == fopen_func_ptr)
		fopen_func_ptr = (fopen_func_t)dlsym(RTLD_NEXT, "fopen");
	if (NULL != fopen_func_ptr)
		return (*fopen_func_ptr)(filename, mode);

	log("Error in dlsym - in 'fopen' wrapper\n");
	exit(-1);
}

// parameter names starting with __ are reserved for standard library
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
FILE *fopen(const char *filename, const char *mode)
{
	if (esan_should_I_fail(__builtin_return_address(0), ESAN_FOPEN)) {
		errno = EINTR;
		return NULL;
	} else {
		return real_fopen(filename, mode);
	}
}

/**************************************************************************************/

/* off_t ftello(FILE *stream); */

typedef off_t (*ftello_func_t)(FILE *stream);

static off_t real_ftello(FILE *stream)
{
	static ftello_func_t ftello_func_ptr = NULL;
	if (NULL == ftello_func_ptr)
		ftello_func_ptr = (ftello_func_t)dlsym(RTLD_NEXT, "ftello");
	if (NULL != ftello_func_ptr)
		return (*ftello_func_ptr)(stream);

	log("Error in dlsym - in 'ftello' wrapper\n");
	exit(-1);
}

// parameter names starting with __ are reserved for standard library
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
off_t ftello(FILE *stream)
{
	if (esan_should_I_fail(__builtin_return_address(0), ESAN_FTELLO)) {
		errno = EIO;
		return -1;
	} else {
		return real_ftello(stream);
	}
}

/**************************************************************************************/

/* long ftell(FILE *stream); */

typedef long (*ftell_func_t)(FILE *stream);

static long real_ftell(FILE *stream)
{
	static ftell_func_t ftell_func_ptr = NULL;
	if (NULL == ftell_func_ptr)
		ftell_func_ptr = (ftell_func_t)dlsym(RTLD_NEXT, "ftell");
	if (NULL != ftell_func_ptr)
		return (*ftell_func_ptr)(stream);

	log("Error in dlsym - in 'ftell' wrapper\n");
	exit(-1);
}

// parameter names starting with __ are reserved for standard library
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
long ftell(FILE *stream)
{
	if (esan_should_I_fail(__builtin_return_address(0), ESAN_FTELL)) {
		return -1;
	} else {
		off_t off_t_result = real_ftello(stream);
		long long_result = real_ftell(stream);
		if (off_t_result != (off_t)long_result) {
			log("File size overflow when using ftell() - use ftello() "
			    "instead! (See: FIO19-C)");
			exit(-1);
		}
		return long_result;
	}
}

/**************************************************************************************/

/* int fclose(FILE *stream); */

typedef int (*fclose_func_t)(FILE *stream);

static int real_fclose(FILE *stream)
{
	static fclose_func_t fclose_func_ptr = NULL;
	if (NULL == fclose_func_ptr)
		fclose_func_ptr = (fclose_func_t)dlsym(RTLD_NEXT, "fclose");
	if (NULL != fclose_func_ptr)
		return (*fclose_func_ptr)(stream);

	log("Error in dlsym - in 'fclose' wrapper\n");
	exit(-1);
}

// parameter names starting with __ are reserved for standard library
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
int fclose(FILE *stream)
{
#ifndef AFL
	// fake closing stderr, it is needed for statistics && logging
	if (stream == stderr) {
		log("Fake closing stderr file stream.");
		return 0;
	}
#endif
	if (esan_should_I_fail(__builtin_return_address(0), ESAN_FCLOSE)) {
		errno = EIO;
		return EOF;
	} else {
		return real_fclose(stream);
	}
}

/**************************************************************************************/

/* void rewind(FILE *stream); */

typedef void (*rewind_func_t)(FILE *stream);

static void real_rewind(FILE *stream)
{
	static rewind_func_t rewind_func_ptr = NULL;
	if (NULL == rewind_func_ptr)
		rewind_func_ptr = (rewind_func_t)dlsym(RTLD_NEXT, "rewind");
	if (NULL != rewind_func_ptr)
		(*rewind_func_ptr)(stream);
	else {
		log("Error in dlsym - in 'rewind' wrapper\n");
		exit(-1);
	}
}

// parameter names starting with __ are reserved for standard library
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
void rewind(FILE *stream)
{
	if (esan_should_I_fail(__builtin_return_address(0), ESAN_REWIND)) {
		log("%s is obsolete function - see SEI CERT C Coding Standard "
		    "MSC24-C for details!\n",
		    "rewind");
		exit(-3);
	} else {
		real_rewind(stream);
	}
}

/**************************************************************************************/

/* int fputs(const char * str, FILE * stream); */

typedef int (*fputs_func_t)(const char *str, FILE *stream);

static int real_fputs(const char *str, FILE *stream)
{
	static fputs_func_t fputs_func_ptr = NULL;
	if (NULL == fputs_func_ptr)
		fputs_func_ptr = (fputs_func_t)dlsym(RTLD_NEXT, "fputs");
	if (NULL != fputs_func_ptr)
		return (*fputs_func_ptr)(str, stream);

	log("Error in dlsym - in 'fputs' wrapper\n");
	exit(-1);
}

// parameter names starting with __ are reserved for standard library
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
int fputs(const char *str, FILE *stream)
{
	if (esan_should_I_fail(__builtin_return_address(0), ESAN_FPUTS)) {
		return EOF;
	} else {
		return real_fputs(str, stream);
	}
}
