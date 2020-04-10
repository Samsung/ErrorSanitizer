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
#define BUF_SIZE 4096

int main()
{
	static char buf[BUF_SIZE];
	FILE *pFile;
	int ret;

	pFile = fopen("setvbuf.test", "wb");
	if (NULL == pFile)
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fopen FAILED.");

	ret = setvbuf(pFile, buf, _IOFBF, sizeof(buf));

	if (0 != ret) {
#ifdef ESAN_FAIL_TEST
		log("setvbuf successfully failed");
#else
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "setvbuf FAILED.");
#endif
	} else {
#ifdef ESAN_FAIL_TEST
		exit_failure(ESAN_TESTS_FAILURE,
			     "setvbuf should have failed, but didn't.");
#endif
	}

	ret = setvbuf(pFile, buf, _IOFBF, sizeof(buf));
	if (0 != ret)
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "setvbuf FAILED.");

	fclose(pFile);

	return 0;
}
