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
#include "error_sanitizer.h"
#include "esan_fail.h"
#include "esan_internals.h"
#include "in_library.h"

#include <stdio.h>
#include <stdlib.h>

void esan_fail_message(const char *function_name)
{
#ifndef AFL
	fflush(stdout);
	fprintf(stderr, "====: ErrorSanitizer: Injected failure at %s\n",
		function_name);
#else
	(void)function_name;
#endif
}

static unsigned long esan_total_execs = 0;
#define BITS_PER_BYTE 8
unsigned int get_failure_status_from_map(void)
{
	unsigned int index_byte = esan_total_execs / BITS_PER_BYTE;
	unsigned int index_bit = esan_total_execs % BITS_PER_BYTE;
	++esan_total_execs;

	/* fail if map is to short, not to let afl cut the input */
	if (index_byte >= esan_error_bitmap_size)
		return 1;

	return ((1U << index_bit) &
		(unsigned char)esan_error_bitmap[index_byte]);
}
