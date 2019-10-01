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
#include "error_sanitizer.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void esan_fail_message(const char *function_name)
{
	fflush(stdout);
	fprintf(stderr, "====: ErrorSanitizer: Injected failure at %s\n",
		function_name);
}

extern int in_library(const void *addr);
static unsigned int internal_fail(void);

int esan_should_I_fail(void)
{
	/*
  if(in_library(NULL))
          return 0;
  */
	return !!(internal_fail());
}

static unsigned long esan_total_execs = 0;

#ifndef FAIL_CHANCE
#define BITS_PER_BYTE 8
static unsigned int internal_fail(void)
{
	unsigned int index_byte = esan_total_execs / BITS_PER_BYTE;
	unsigned int index_bit = esan_total_execs % BITS_PER_BYTE;
	++esan_total_execs;

	if (esan_always_succeed)
		return 0;

	/* fail if map is to short, not to let afl cut the input */
	if (index_byte >= esan_error_bitmap_size)
		return 1;

	return ((1U << index_bit) & esan_error_bitmap[index_byte]);
}
#else

#if FAIL_CHANCE < 0 || FAIL_CHANCE > 100
#error "Bad FAIL_CHANCE definition. It should be between 0 and 100\n"
#endif

static unsigned long safe_first = 0;
static long fail_chance = FAIL_CHANCE;
static unsigned initialized = 0;

#define MAX_PERCENT_GRANUALITY 100
static int random_fail(long fail_chance)
{
	long random = rand() % MAX_PERCENT_GRANUALITY;
	return random < fail_chance;
}

static void initialize(void)
{
	char *env;
	if (!initialized) {
		++initialized;
		srand(time(0));
		env = getenv("ESAN_FAIL_CHANCE");
		if (env != NULL)
			fail_chance = strtol(env, NULL, 0) %
				      (MAX_PERCENT_GRANUALITY + 1);
		env = getenv("ESAN_SKIP_FIRST");
		if (env != NULL)
			safe_first = strtoul(env, NULL, 0);
	}
}

static unsigned int internal_fail(void)
{
	initialize();
	if (safe_first > 0) {
		fprintf(stderr, "%lu\n", safe_first);
		--safe_first;
		return 0;
	}
	++esan_total_execs;

	if (esan_total_execs > (ULONG_MAX >> 4U))
		return 1;

	return random_fail(fail_chance);
}
#endif
