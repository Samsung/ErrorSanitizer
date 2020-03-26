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
#include <string.h>

int main()
{
	FILE *pFile;
	static const char test_buffer[] = "fopen_test_buffer";
	size_t test_buffer_size = sizeof(test_buffer) - 1;

	pFile = fopen("fopen.test", "wb");
	if (pFile == NULL) {
#ifdef ESAN_FAIL_TEST
		(void)test_buffer_size;
		log("fopen successfully failed.");
		return 0;
#else
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fopen FAILED.");
#endif
	} else {
#ifdef ESAN_FAIL_TEST
		fclose(pFile);
		exit_failure(ESAN_TESTS_FAILURE,
			     "fopen should have failed, but didn't.");
#endif
	}
	if (fwrite(test_buffer, 1, test_buffer_size, pFile) !=
	    test_buffer_size) {
		fclose(pFile);
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fwrite FAILED.");
	}
	fclose(pFile);
	return 0;
}
