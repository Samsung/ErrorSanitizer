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
#define MSG_BUF_SIZE 512
//TODO: what about exiting on failure and printing error msg
int perform_testing(const uint8_t *buffer_ptr, size_t buffer_size)
{
	(void)buffer_ptr;
	(void)buffer_size;
	size_t callocIt;
	char *test_calloc_buffer = (char *)calloc(TEST_MEMORY_SIZE, 1);
	if (test_calloc_buffer) {
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
	} else {
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "calloc FAILED.");
	}

	return 0;
}

int main(int argc, char **argv)
{
	return main0(argc, argv);
}
