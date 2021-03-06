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

    Author: Mateusz Nosek <m.nosek@samsung.com>
*/
#include "esan_wrapper.h"

int main()
{
	FILE *pFile;
	int ret;

	pFile = fopen("Makefile", "r");
	if (NULL == pFile)
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fopen FAILED.");

	ret = fseek(pFile, 0, SEEK_SET);
	fclose(pFile);

	if (-1 == ret) {
#ifdef ESAN_FAIL_TEST
		log("fseek failed successfully.");
#else
		exit_failure(ESAN_TESTS_FAILURE, "fseek FAILED.");
#endif
	} else {
#ifdef ESAN_FAIL_TEST
		exit_failure(ESAN_TESTS_FAILURE,
			     "fseek should have failed, but didn't");
#endif
	}

	return 0;
}
