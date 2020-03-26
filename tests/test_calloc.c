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
#define MSG_BUF_SIZE 512

int main()
{
	char *test_calloc_buffer = calloc(TEST_MEMORY_SIZE, 1);
	if (test_calloc_buffer) {
#ifdef ESAN_FAIL_TEST
		free(test_calloc_buffer);
		exit_failure(ESAN_TESTS_FAILURE,
			     "calloc should have failed, but didn't.");
#else
		size_t callocIt;
		for (callocIt = 0; callocIt < TEST_MEMORY_SIZE; ++callocIt) {
			if (test_calloc_buffer[callocIt] != 0) {
				char msgBuf[MSG_BUF_SIZE];
				snprintf(
					msgBuf, MSG_BUF_SIZE,
					"Memory at: %zd, is %d. calloc FAILED.",
					callocIt, test_calloc_buffer[callocIt]);
				free(test_calloc_buffer);
				exit_failure(ESAN_TESTS_INTERNAL_ERROR, "%s",
					     msgBuf);
			}
		}
		free(test_calloc_buffer);
#endif
	} else {
#ifdef ESAN_FAIL_TEST
		log("calloc successfully failed.");
#else
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "calloc FAILED.");
#endif
	}

	return 0;
}
