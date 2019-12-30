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
    Author: Mateusz Nosek <m.nosek@samsung.com>
*/
#include "esan_wrapper.h"
#define FPUTS_ITERATION_COUNT 100

int main()
{
	unsigned fputs_it;
	FILE *pFile;

	pFile = fopen("fputs.test", "wb");
	if (pFile == NULL)
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "fopen FAILED.");

	for (fputs_it = 0; fputs_it < FPUTS_ITERATION_COUNT; ++fputs_it) {
		if (fputs("ab", pFile) < 0) {
			fclose(pFile);
			exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
				     "fputs FAILED.");
		}
	}
	fclose(pFile);
	return 0;
}
