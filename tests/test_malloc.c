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

    Author: Ernest Borowski <e.borowski@samsung.com>
*/
#include "esan_wrapper.h"
#define TEST_MEMORY_SIZE 100
//TODO: what about exiting on failure and printing error msg
int perform_testing(const uint8_t *buffer_ptr, size_t buffer_size)
{
	(void)buffer_ptr;
	(void)buffer_size;
	char *test_malloc_buffer = (char *)malloc(TEST_MEMORY_SIZE);
	if (test_malloc_buffer) {
		test_malloc_buffer[0] = '1';
		free(test_malloc_buffer);
	} else {
		exit_failure(ESAN_TESTS_INTERNAL_ERROR, "malloc FAILED.");
	}

	return 0;
}

int main(int argc, char **argv)
{
	return main0(argc, argv);
}
