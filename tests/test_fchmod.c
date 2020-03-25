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
    Author: Mateusz Nosek <m.nosek@samsung.com>
*/
#include "esan_wrapper.h"
#include <sys/stat.h>

#define ESAN_FULL_ACCESS_ALL 0777
#define ESAN_RW_ACCESS_OWNER 0600

int main()
{
	int fd;
	FILE *file;
	const char *path = "./fchmod.test";
	file = fopen(path, "a");
	if (!file)
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "Creating temporary file failed!");
	fclose(file);

	file = fopen(path, "a");
	if (!file)
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "Opening temporary file failed!");
	fd = fileno(file);

	if (0 != fchmod(fd, ESAN_FULL_ACCESS_ALL)) {
		fclose(file);
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fchmod 0777 failed!");
	}

	if (0 != fchmod(fd, 0)) {
		fclose(file);
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fchmod 0000 failed!");
	}

	if (0 != fchmod(fd, ESAN_RW_ACCESS_OWNER)) {
		fclose(file);
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fchmod 0600 failed!");
	}

	fclose(file);

	return 0;
}
