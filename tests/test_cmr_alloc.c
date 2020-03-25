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
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "esan_wrapper.h"
#define TEST_MEMORY_SIZE 100
int main()
{
	{
		char *test_malloc_buffer = (char *)malloc(TEST_MEMORY_SIZE);
		if (test_malloc_buffer) {
			test_malloc_buffer[0] = '1';
			free(test_malloc_buffer);
		} else {
			exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
				     "malloc FAILED.");
		}

		char *test_calloc_buffer = (char *)calloc(TEST_MEMORY_SIZE, 1);
		if (test_calloc_buffer) {
			test_calloc_buffer[0] = '1';
			free(test_calloc_buffer);
		} else {
			exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
				     "calloc FAILED.");
		}

		char *test_realloc_buffer =
			(char *)realloc(NULL, TEST_MEMORY_SIZE);
		if (test_realloc_buffer) {
			test_realloc_buffer[0] = '1';
			free(test_realloc_buffer);
		} else {
			exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
				     "realloc FAILED.");
		}

		test_realloc_buffer =
			(char *)realloc(NULL, 2 * TEST_MEMORY_SIZE);
		if (test_realloc_buffer) {
			test_realloc_buffer = (char *)realloc(
				test_realloc_buffer, 3 * TEST_MEMORY_SIZE);
			if (test_realloc_buffer) {
				test_realloc_buffer =
					(char *)realloc(test_realloc_buffer,
							3 * TEST_MEMORY_SIZE);
			} else {
				exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
					     "realloc FAILED.");
			}
			free(test_realloc_buffer);
		}
	}

	{
		char *test_malloc_buffer = (char *)malloc(TEST_MEMORY_SIZE);
		if (test_malloc_buffer) {
			test_malloc_buffer[0] = '1';
			free(test_malloc_buffer);
		} else {
			exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
				     "malloc FAILED.");
		}

		char *test_calloc_buffer = (char *)calloc(TEST_MEMORY_SIZE, 1);
		if (test_calloc_buffer) {
			test_calloc_buffer[0] = '1';
			free(test_calloc_buffer);
		} else {
			exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
				     "calloc FAILED.");
		}

		char *test_realloc_buffer =
			(char *)realloc(NULL, TEST_MEMORY_SIZE);
		if (test_realloc_buffer) {
			test_realloc_buffer[0] = '1';
			free(test_realloc_buffer);
		} else {
			exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
				     "realloc FAILED.");
		}
	}

	return 0;
}
