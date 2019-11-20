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
#include "esan_strings.h"
#include "error_sanitizer.h"

uint8_t esan_always_succeed = 1;
uint8_t *esan_error_bitmap = NULL;
size_t esan_error_bitmap_size = 0;

const char SPLIT_STRING[] = "XXXX";

uint8_t *esan_split_input_file(uint8_t *data, size_t size)
{
	uint8_t *result =
		(uint8_t *)esan_strnstr((char *)data, SPLIT_STRING, size);
	if (NULL != result)
		result += 4; /* sizeof SPLIT_STRING */
	return result;
}

void esan_initialize(uint8_t *bitmap_ptr, size_t bitmap_size)
{
	esan_error_bitmap = bitmap_ptr;
	esan_error_bitmap_size = bitmap_size;
	esan_always_succeed = 0;
	fprintf(stderr, "====: ErrorSanitizer: Library initialized!!\n");
	fprintf(stderr, "====: ErrorSanitizer: esan_error_bitmap_size = %zu\n",
		esan_error_bitmap_size);
}
