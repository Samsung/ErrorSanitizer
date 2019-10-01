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
*/
#include "error_sanitizer.h"

uint8_t esan_always_succeed = 1;
uint8_t *esan_error_bitmap = NULL;
size_t esan_error_bitmap_size = 0;

const char SPLIT_STRING[] = "XXXX";
static long const_strlen(const char *str)
{
	const char *cp = str;
	while (*str++ != '\0')
		;
	return str - cp;
}

static int strncmp(const char *s1, const char *s2, size_t n)
{
	size_t i;
	int tmp;

	for (i = 0; i < n; i++, s1++, s2++) {
		tmp = *s1 - *s2;
		if (tmp != 0)
			return tmp;
		if (*s1 == '\0')
			return 0;
	}

	return 0;
}

static char *strnstr(char *s1, const char *s2, int length)
{
	if (s1 == NULL || s2 == NULL)
		return NULL;

	long len_s1 = const_strlen(s1);
	long len_s2 = const_strlen(s2);

	for (long i = 0; (i < len_s1 - len_s2) && (i < length - len_s2); ++i) {
		if (0 == strncmp(s1 + i, s2, len_s2))
			return s1 + i;
	}
	return NULL;
}

uint8_t *esan_split_input_file(uint8_t *data, size_t size)
{
	uint8_t *result = (uint8_t *)strnstr((char *)data, SPLIT_STRING, size);
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
