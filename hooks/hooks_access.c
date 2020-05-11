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
#include "hooks_include.h"

/* int access(const char *pathname, int mode); */

typedef int (*access_func_t)(const char *pathname, int mode);

int real_access(const char *pathname, int mode)
{
	static access_func_t access_func_ptr = NULL;
	if (NULL == access_func_ptr)
		access_func_ptr = (access_func_t)dlsym(RTLD_NEXT, "access");
	if (NULL != access_func_ptr)
		return (*access_func_ptr)(pathname, mode);

	ESAN_ERROR("Error in dlsym - in 'access' wrapper\n");
	exit(-1);
}

int access(const char *pathname, int mode)
{
	ESAN_DEBUG("%s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
	obj_stats[ESAN_ACCESS].esan_nr_executions += 1;
	if (esan_should_I_fail()) {
		esan_fail_message("access");
		obj_stats[ESAN_ACCESS].esan_nr_failed_executions += 1;
		return -1;
	} else {
		return real_access(pathname, mode);
	}
}

/**************************************************************************************/

/* int faccessat(int dirfd, const char *pathname, int mode, int flags); */

typedef int (*faccessat_func_t)(int dirfd, const char *pathname, int mode,
				int flags);

int real_faccessat(int dirfd, const char *pathname, int mode, int flags)
{
	static faccessat_func_t faccessat_func_ptr = NULL;
	if (NULL == faccessat_func_ptr)
		faccessat_func_ptr =
			(faccessat_func_t)dlsym(RTLD_NEXT, "faccessat");
	if (NULL != faccessat_func_ptr)
		return (*faccessat_func_ptr)(dirfd, pathname, mode, flags);

	ESAN_ERROR("Error in dlsym - in 'faccessat' wrapper\n");
	exit(-1);
}

int faccessat(int dirfd, const char *pathname, int mode, int flags)
{
	ESAN_DEBUG("%s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
	obj_stats[ESAN_FACCESSAT].esan_nr_executions += 1;
	if (esan_should_I_fail()) {
		esan_fail_message("faccessat");
		obj_stats[ESAN_FACCESSAT].esan_nr_failed_executions += 1;
		return -1;
	} else {
		return real_faccessat(dirfd, pathname, mode, flags);
	}
}
