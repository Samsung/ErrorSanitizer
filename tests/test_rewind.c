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
    Author: Jakub Botwicz <j.botwicz@samsung.com>
    Author: Mateusz Nosek <m.nosek@samsung.com>
*/

#include "esan_wrapper.h"
#include <errno.h>

int main()
{
	FILE *pFile;
	long file_size;
	char *buffer;
	size_t result;

	pFile = fopen("Makefile", "rb");
	if (pFile == NULL)
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fopen FAILED.");

	/* obtain file size: */
	if (fseeko(pFile, 0, SEEK_END) != 0) {
		fclose(pFile);
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fseeko FAILED.");
	}

	file_size = ftello(pFile);
	if (file_size == -1) {
		fclose(pFile);
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "ftello FAILED.");
	}

	if (fseeko(pFile, 0L, SEEK_SET) != 0) {
		fclose(pFile);
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fseeko FAILED.");
	}

	rewind(pFile);

	/* allocate memory to contain the whole file: */
	buffer = (char *)malloc(sizeof(char) * file_size);
	if (buffer == NULL) {
		fclose(pFile);
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "malloc FAILED.");
	}

	/* copy the file into the buffer: */
	result = fread(buffer, 1, file_size, pFile);

	if (result != (size_t)file_size) {
		fclose(pFile);
		free(buffer);
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "Reading FAILED.");
	}

	/* the whole file is now loaded in the memory buffer. */

	/* terminate */
	if (fclose(pFile) != 0) {
		free(buffer);
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fclose FAILED.");
	}

	free(buffer);

	pFile = fopen("Makefile", "rb");

	if (pFile == NULL)
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fopen FAILED.");

	/* obtain file size: */
	if (fseeko(pFile, 0, SEEK_END) != 0) {
		fclose(pFile);
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fseeko FAILED.");
	}

	file_size = ftell(pFile);
	if (file_size == -1) {
		fclose(pFile);
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "ftell FAILED.");
	}

	rewind(pFile);

	/* allocate memory to contain the whole file: */
	buffer = (char *)malloc(sizeof(char) * file_size);
	if (buffer == NULL) {
		fclose(pFile);
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "malloc FAILED.");
	}

	/* copy the file into the buffer: */
	result = fread(buffer, 1, file_size, pFile);
	free(buffer);

	if (result != (size_t)file_size) {
		fclose(pFile);
		fprintf(stderr, "result: %ld ", result);
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fread FAILED.");
	}

	if (fclose(pFile) != 0) {
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fclose FAILED.");
	}

	return 0;
}
