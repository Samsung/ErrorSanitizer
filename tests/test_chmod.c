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

    Author: Mateusz Nosek <m.nosek@samsung.com>
*/
#include "esan_wrapper.h"
#include <sys/stat.h>

#define ESAN_FULL_ACCESS_ALL 0777
#define ESAN_RW_ACCESS_OWNER 0600

int perform_testing(const uint8_t *buffer_ptr, size_t buffer_size)
{
	(void)buffer_ptr;
	(void)buffer_size;
	FILE *file;
	const char *path = "./chmod.test";
	file = fopen(path, "a");
	if (!file)
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "Creating temporary file failed!");
	fclose(file);
	/* race condition here... I know */
	if (0 != chmod(path, ESAN_FULL_ACCESS_ALL))
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "chmod 0777 failed!");
	if (0 != chmod(path, 0))
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "chmod 0000 failed!");
	if (0 != chmod(path, ESAN_RW_ACCESS_OWNER))
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "chmod 0600 failed!");
	return 0;
}

int main(int argc, char **argv)
{
	return main0(argc, argv);
}
