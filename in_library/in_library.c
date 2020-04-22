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
*/
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

#include "in_library.h"
#include "log.h"

#define MAX_LIB_COUNT 128ul
#define MAX_LINE_LENGHT (32 * 4096ul)
#define HEX_NUMBER_BASE 16

typedef struct library_address_range {
	unsigned long start, end;
} library_address_range_s;

static library_address_range_s lib_addr_range[MAX_LIB_COUNT];
static const char *proc_maps_path = "/proc/self/maps";
// regular  expression for excluding  libraries, currently works for:
// ld, libc, libm, libstc++, libgcc_s
static const char *library_exclusion_regex =
	"/l(ib(c|m|stdc\\+\\+|gcc_s)|d)(-[0-9.-]+)?\\.so";
static long current_address_idx = -1;
static char line[MAX_LINE_LENGHT];

static void add_new_library(const library_address_range_s *lib)
{
	if (current_address_idx >= 0 &&
	    lib_addr_range[current_address_idx].end == lib->start)
		lib_addr_range[current_address_idx].end = lib->end;
	else
		lib_addr_range[++current_address_idx] = *lib;
}

static void parse_file(void)
{
	library_address_range_s lib_addr;
	unsigned long end_mem_offset = 0;
	regex_t compiledReg;
	int ret_code, line_it = 0;
	FILE *fp = NULL;
	char *strtoul_end_offset = NULL;

	// open File
	fp = fopen(proc_maps_path, "r");
	if (fp == NULL)
		exit_failure(ESAN_LIBRARY_FUNCTION_ERROR,
			     "Unable to open proc maps");

	// compile regular expression
	ret_code = regcomp(&compiledReg, library_exclusion_regex, REG_EXTENDED);
	if (ret_code != 0) {
		fclose(fp);
		exit_failure(ESAN_LIBRARY_FUNCTION_ERROR,
			     "Failed to compile regular expression pattern.");
	}

	while (fgets(line, MAX_LINE_LENGHT, fp) != NULL) {
		ret_code = regexec(&compiledReg, line, 0, NULL, 0);

		// regex not matched
		if (ret_code)
			continue;
		// find - character separating memory start,end values
		while (end_mem_offset < MAX_LINE_LENGHT &&
		       line[end_mem_offset] != '-')
			++end_mem_offset;

		if (end_mem_offset >= MAX_LINE_LENGHT) {
			fclose(fp);
			exit_failure(
				ESAN_INTERNAL_ERROR,
				"Could not find \"-\" character in %d line",
				line_it);
		}

		line[end_mem_offset] = '\0';

		// parse library start address
		lib_addr.start =
			strtoul(line, &strtoul_end_offset, HEX_NUMBER_BASE);
		if (strtoul_end_offset == line) {
			fclose(fp);
			exit_failure(
				ESAN_INTERNAL_ERROR,
				"Unable to parse start memory of %d line.\n",
				line_it);
		}

		// parse library end address
		lib_addr.end = strtoul(line + end_mem_offset + 1,
				       &strtoul_end_offset, HEX_NUMBER_BASE);
		if (strtoul_end_offset == line + end_mem_offset + 1) {
			fclose(fp);
			exit_failure(ESAN_INTERNAL_ERROR,
				     "Unable to parse end memory of %d line.\n",
				     line_it);
		}

		// add library address to array
		add_new_library(&lib_addr);

		++line_it;
	}
	fclose(fp);
	regfree(&compiledReg);
}

void in_library_initialize(void)
{
	unsigned long library_it;

	for (library_it = 0; library_it < MAX_LIB_COUNT; ++library_it) {
		lib_addr_range[library_it].start =
			lib_addr_range[library_it].end = 0;
	}
	parse_file();
}

int in_library(const void *address)
{
	long library_it;
	unsigned long addr;
	addr = (unsigned long)address;
	for (library_it = 0; library_it < current_address_idx; ++library_it) {
		if (addr > lib_addr_range[library_it].start &&
		    addr < lib_addr_range[library_it].end)
			return 1;
	}
	return 0;
}
