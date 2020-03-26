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
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "error_sanitizer.h"
#include "log.h"

#define MAX_FILE_SIZE (1024)

enum ESAN_TESTS_ERROR_CODE_E {
	ESAN_TESTS_SUCCESS = 0,
	ESAN_TESTS_INTERNAL_ERROR = 1,
	ESAN_TESTS_LIBRARY_FUNCTION_ERROR = 2,
	ESAN_TESTS_INVALID_ARGUMENT = 3,
	ESAN_TESTS_FAILURE = 4
};
