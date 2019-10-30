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
#include <string.h>

// TODO: Unify printf msg
int perform_testing(const uint8_t *buffer_ptr, size_t buffer_size)
{
	(void)buffer_ptr;
	(void)buffer_size;
	FILE *pFile;
	const char *test_buffer = "fopen_test_buffer";
	size_t test_buffer_size = strlen(test_buffer);

	pFile = fopen("fclose.test", "wb");
	if (pFile == NULL) {
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fopen FAILED.");
	}
	if (fwrite(test_buffer, 1, test_buffer_size, pFile) !=
	    test_buffer_size) {
		fclose(pFile);
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fwrite FAILED.");
	}
	if (fclose(pFile) != 0) {
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fclose FAILED.");
	}
	return 0;
}

int main(int argc, char **argv)
{
	return main0(argc, argv);
}
