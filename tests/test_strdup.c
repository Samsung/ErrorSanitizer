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
    Author: Jakub Botwicz <j.botwicz@samsung.com>
    Author: Mateusz Nosek <m.nosek@samsung.com>
*/
#include "esan_wrapper.h"
#include <string.h>

int main()
{
	static const char test_string[] = "strdup_test_string";
	char *test_strdup = strdup(test_string);
	if (test_strdup) {
#ifdef ESAN_FAIL_TEST
		free(test_strdup);
		exit_failure(ESAN_TESTS_INTERNAL_ERROR,
			     "strdup should have failed, but didn't.");
#else
		test_strdup[0] = '1';
		free(test_strdup);
#endif
	} else {
#ifdef ESAN_FAIL_TEST
		log("strdup successfully failed.");
#else
		exit_failure(ESAN_TESTS_INTERNAL_ERROR, "strdup FAILED.");
#endif
	}

	return 0;
}
