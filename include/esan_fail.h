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
*/
#ifndef ESAN_FAIL_H
#define ESAN_FAIL_H

#include "stats.h"

enum ESAN_FAILURE_STATUS_E {
	ESAN_ALWAYS_SUCCEED,
	ESAN_ALWAYS_FAIL,
	ESAN_MAP_BASED_FAILURE,
};

void esan_disable_failure(void);
void esan_enable_always_failure(void);
void esan_enable_map_based_failure(void);
enum ESAN_FAILURE_STATUS_E esan_get_and_disable_failure(void);
enum ESAN_FAILURE_STATUS_E esan_get_failure_status(void);
void esan_set_failure_status(enum ESAN_FAILURE_STATUS_E failure_status);
enum ESAN_FAILURE_E esan_should_I_fail(const void *caller_addr,
				       enum ESAN_FUNCTIONS_E hook);

#endif /* ESAN_FAIL_H */
