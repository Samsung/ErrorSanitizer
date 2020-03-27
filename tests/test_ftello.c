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
#define FPUTS_ITERATION_COUNT 100
#define MSG_BUF_SIZE 512

int main()
{
	unsigned fputs_it;
	FILE *pFile;
	long file_start_pos, file_end_pos;
	static const char test_string[] = "ab";
	long test_string_size = sizeof(test_string) - 1;

	pFile = fopen("ftell.test", "wb");
	if (pFile == NULL)
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fopen FAILED.");

	file_start_pos = ftello(pFile);
	if (file_start_pos == -1) {
		fclose(pFile);
#ifdef ESAN_FAIL_TEST
		log("ftello failed successfully.");
		return 0;
#else
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "ftello FAILED.");
#endif
	} else {
#ifdef ESAN_FAIL_TEST
		fclose(pFile);
		exit_failure(ESAN_TESTS_FAILURE,
			     "ftello should have failed, but didn't.");
#endif
	}
	for (fputs_it = 0; fputs_it < FPUTS_ITERATION_COUNT; ++fputs_it) {
		if (fputs(test_string, pFile) < 0) {
			fclose(pFile);
			exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
				     "fputs FAILED.");
		}
	}
	file_end_pos = ftell(pFile);
	if (file_end_pos == -1) {
		fclose(pFile);
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "ftell FAILED.");
	}
	if (file_end_pos - file_start_pos !=
	    test_string_size * FPUTS_ITERATION_COUNT) {
		char msgBuf[MSG_BUF_SIZE];
		snprintf(
			msgBuf, MSG_BUF_SIZE,
			"Ftell position values missmatch: %ld,expected %ld. ftell FAILED.",
			file_end_pos - file_start_pos,
			test_string_size * FPUTS_ITERATION_COUNT);
		fclose(pFile);
		exit_failure(ESAN_TESTS_INTERNAL_ERROR, "%s", msgBuf);
	}
	if (fclose(pFile) != 0) {
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fclose FAILED.");
	}
	return 0;
}
