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
    Author: Mateusz Nosek <m.nosek@samsung.com>
*/
/* PLEASE remember to add "fcntl" to enum ESAN_FUNCTION_NAMES in error_sanitizer_preload.c and ESAN_FCNTL to enum ESAN_FUNCTIONS in error_sanitizer.h in the SAME position! */

#include "hooks_include.h"
#include <fcntl.h>
#include <stdarg.h>

#define U(x) ((unsigned)(x))

#define ARG_VOID                                                               \
	(U(F_GETFD) | U(F_GETFL) | U(F_GETOWN) | U(F_GETSIG) | U(F_GETLEASE) | \
	 U(F_GETPIPE_SZ) | U(F_GET_SEALS))
#define ARG_INT                                                      \
	(U(F_DUPFD) | U(F_DUPFD_CLOEXEC) | U(F_SETFD) | U(F_SETFL) | \
	 U(F_SETOWN) | U(F_SETSIG) | U(F_SETLEASE) | U(F_NOTIFY) |   \
	 U(F_SETPIPE_SZ) | U(F_ADD_SEALS))

#define UINT64_PTR                                                     \
	(U(F_GET_RW_HINT) | U(F_SET_RW_HINT) | U(F_GET_FILE_RW_HINT) | \
	 U(F_SET_FILE_RW_HINT))

#define ARG_POINTER                                                           \
	(U(F_SETLK) | U(F_SETLKW) | U(F_GETLK) | U(F_OFD_SETLK) |             \
	 U(F_OFD_SETLKW) | U(F_OFD_GETLK) | U(F_GETOWN_EX) | U(F_SETOWN_EX) | \
	 U(UINT64_PTR))

/* int fcntl(int fd, int cmd, ...); */

typedef int (*fcntl_func_t)(int fd, int cmd, ...);

int fcntl(int fd, int cmd, ...)
{
	static fcntl_func_t fcntl_func_ptr = NULL;
	va_list arg_list;
	int ret, int_arg, temp_cmd;
	void *ptr_arg;

	if (esan_should_I_fail(__builtin_return_address(0), ESAN_FCNTL)) {
		return -1;
	} else {
		if (NULL == fcntl_func_ptr)
			fcntl_func_ptr =
				(fcntl_func_t)dlsym(RTLD_NEXT, "fcntl");
		if (NULL == fcntl_func_ptr) {
			log("Error in dlsym - in 'fcntl' wrapper\n");
			exit(-1);
		}

		temp_cmd = cmd;
		if ((unsigned)cmd & ARG_VOID) {
			return (*fcntl_func_ptr)(fd, cmd);
		} else if ((unsigned)cmd & ARG_INT) {
			cmd = 1;
			va_start(arg_list, cmd);
			int_arg = va_arg(arg_list, int);
			ret = (*fcntl_func_ptr)(fd, temp_cmd, int_arg);
			va_end(arg_list);
			return ret;
		} else if ((unsigned)cmd &
			   ARG_POINTER) { /* TODO different types of pointer */
			cmd = 1;
			va_start(arg_list, cmd);
			ptr_arg = va_arg(arg_list, void *);
			ret = (*fcntl_func_ptr)(fd, temp_cmd, ptr_arg);
			va_end(arg_list);
			return ret;
		}

		log("Error in 'fcntl' wrapper implementation!\n");
		exit(-1);
	}
}
