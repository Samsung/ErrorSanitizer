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

#include "esan_fail.h"
#include "log.h"

int main()
{
	void *ptr;

	if (ESAN_MAP_BASED_FAILURE != esan_get_and_disable_failure())
		exit_failure(ESAN_INTERNAL_ERROR,
			     "ESAN should be based on map here!");

	if (ESAN_ALWAYS_SUCCEED != esan_get_failure_status())
		exit_failure(ESAN_INTERNAL_ERROR,
			     "ESAN should always succeed here!");

	esan_enable_always_failure();

	ptr = malloc(16);
	if (NULL != ptr) {
		free(ptr);
		exit_failure(ESAN_INTERNAL_ERROR, "Malloc should have failed!");
	}

	if (ESAN_ALWAYS_FAIL != esan_get_and_disable_failure())
		exit_failure(ESAN_INTERNAL_ERROR,
			     "ESAN should always fail here!");

	return 0;
}
