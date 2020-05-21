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
#include "esan_fail.h"
#include "error_sanitizer.h"
#include "in_library.h"
#include "stats.h"

static enum ESAN_FAILURE_STATUS_E esan_always_succeed = ESAN_ALWAYS_SUCCEED;
void esan_disable_failure(void)
{
	esan_always_succeed = ESAN_ALWAYS_SUCCEED;
}
void esan_enable_map_based_failure(void)
{
	esan_always_succeed = ESAN_MAP_BASED_FAILURE;
}
void esan_enable_always_failure(void)
{
	esan_always_succeed = ESAN_ALWAYS_FAIL;
}
enum ESAN_FAILURE_STATUS_E esan_get_failure_status(void)
{
	return esan_always_succeed;
}
enum ESAN_FAILURE_STATUS_E esan_get_and_disable_failure(void)
{
	enum ESAN_FAILURE_STATUS_E current_status = esan_always_succeed;
	esan_always_succeed = ESAN_ALWAYS_SUCCEED;
	return current_status;
}
void esan_set_failure_status(enum ESAN_FAILURE_STATUS_E failure_status)
{
	esan_always_succeed = failure_status;
}

static unsigned long esan_total_execs = 0;
#define BITS_PER_BYTE 8
static enum ESAN_FAILURE_E get_failure_status_from_map(void)
{
	unsigned int index_byte = esan_total_execs / BITS_PER_BYTE;
	unsigned int index_bit = esan_total_execs % BITS_PER_BYTE;
	++esan_total_execs;

	/* fail if map is to short, not to let afl cut the input */
	if (index_byte >= esan_error_bitmap_size)
		return 1;

	return ((1U << index_bit) &
		(unsigned char)esan_error_bitmap[index_byte]);
}

enum ESAN_FAILURE_E esan_should_I_fail(const void *caller_addr,
				       enum ESAN_FUNCTIONS_E hook)
{
	enum ESAN_FAILURE_STATUS_E failure_status = esan_get_failure_status();
	enum ESAN_FAILURE_E ret_code;
	int in_lib = in_library(caller_addr);
	if (failure_status == ESAN_ALWAYS_SUCCEED || in_lib)
		ret_code = ESAN_SUCCEED;
	else if (failure_status == ESAN_ALWAYS_FAIL)
		ret_code = ESAN_FAIL;
	/* failure_status == ESAN_MAP_BASED_FAILURE */
	/* Get failure status from map */
	else
		ret_code = !!(get_failure_status_from_map());
#ifndef AFL
	if (!in_lib)
		add_execution(hook, ret_code);
#else
	(void)hook;
#endif
	return ret_code;
}
