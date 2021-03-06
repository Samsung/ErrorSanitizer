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
#include <errno.h>
#define BUF_SIZE 4096

int main()
{
	static char buf[BUF_SIZE];
	FILE *pFile;

	pFile = fopen("Makefile", "rb");
	if (NULL == pFile)
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fopen FAILED.");

	setbuf(pFile, buf);

#ifdef ESAN_FAIL_TEST
	exit_failure(ESAN_TESTS_FAILURE,
		     "setbuf should have failed, but didn't.");
#endif

	if (fclose(pFile) != 0)
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fclose FAILED.");

	return 0;
}
