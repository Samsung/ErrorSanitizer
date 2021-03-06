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
#include "esan_wrapper.h"
#define TEST_MEMORY_SIZE 100

int main()
{
	char *test_malloc_buffer = (char *)malloc(TEST_MEMORY_SIZE);
	if (test_malloc_buffer) {
#ifdef ESAN_FAIL_TEST
		exit_failure(ESAN_TESTS_FAILURE, "malloc SUCCEEDED.");
#else
		test_malloc_buffer[0] = '1';
		free(test_malloc_buffer);
#endif
	} else {
#ifdef ESAN_FAIL_TEST
		log("malloc successfully failed.");
#else
		exit_failure(ESAN_TESTS_INTERNAL_ERROR, "malloc FAILED.");
#endif
	}

	return 0;
}
