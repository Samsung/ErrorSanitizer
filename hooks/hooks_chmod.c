/*
    Copyright (c) 2018 - 2019, Samsung Electronics Co., Ltd. All rights reserved.

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

/* int chmod(const char *path, mode_t mode); */

typedef int (*chmod_func_t)(const char *path, mode_t mode);

int real_chmod(const char *path, mode_t mode)
{
	static chmod_func_t chmod_func_ptr = NULL;
	if (NULL == chmod_func_ptr)
		chmod_func_ptr = (chmod_func_t)dlsym(RTLD_NEXT, "chmod");
	if (NULL != chmod_func_ptr)
		return (*chmod_func_ptr)(path, mode);

	ESAN_ERROR("Error in dlsym - in 'chmod' wrapper\n");
	exit(-1);
}

int chmod(const char *path, mode_t mode)
{
	ESAN_DEBUG("%s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
	obj_stats[ESAN_CHMOD].esan_nr_executions += 1;
	if (esan_should_I_fail()) {
		fail_common();
		esan_fail_message("chmod");
		obj_stats[ESAN_CHMOD].esan_nr_failed_executions += 1;
		return -1;
	} else {
		return real_chmod(path, mode);
	}
}

/**************************************************************************************/

/* int fchmod(int fd, mode_t mode); */

typedef int (*fchmod_func_t)(int fd, mode_t mode);

int real_fchmod(int fd, mode_t mode)
{
	static fchmod_func_t fchmod_func_ptr = NULL;
	if (NULL == fchmod_func_ptr)
		fchmod_func_ptr = (fchmod_func_t)dlsym(RTLD_NEXT, "fchmod");
	if (NULL != fchmod_func_ptr)
		return (*fchmod_func_ptr)(fd, mode);

	ESAN_ERROR("Error in dlsym - in 'fchmod' wrapper\n");
	exit(-1);
}

int fchmod(int fd, mode_t mode)
{
	ESAN_DEBUG("%s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
	obj_stats[ESAN_FCHMOD].esan_nr_executions += 1;
	if (esan_should_I_fail()) {
		fail_common();
		esan_fail_message("fchmod");
		obj_stats[ESAN_FCHMOD].esan_nr_failed_executions += 1;
		return -1;
	} else {
		return real_fchmod(fd, mode);
	}
}
