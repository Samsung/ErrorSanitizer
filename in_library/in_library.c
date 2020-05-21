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
#include <string.h>

#include "in_library.h"
#include "log.h"

#define MAX_LIB_COUNT 128ul
#define MAX_LINE_LENGTH (32 * 4096ul)
#define HEX_NUMBER_BASE 16

typedef struct library_address_range {
	unsigned long start, end;
#ifdef DEBUG
	char *lib_name;
#endif /* DEBUG */
} library_address_range_s;

static library_address_range_s lib_addr_range[MAX_LIB_COUNT];
static const char *proc_maps_path = "/proc/self/maps";
// regular  expression for excluding  libraries, currently works for:
// ld, libc, libm, libstc++, libgcc_s
static const char *library_exclusion_regex =
	"/(l(ib(c|m|stdc\\+\\+|gcc_s)|d)|error_sanitizer_preload)(-[0-9.-]+)?\\.so";
static long current_address_idx = -1;
static char line[MAX_LINE_LENGTH];

static void add_new_library(const library_address_range_s *lib,
			    const char *lib_name)
{
	if (current_address_idx >= 0 &&
	    lib_addr_range[current_address_idx].end == lib->start)
		lib_addr_range[current_address_idx].end = lib->end;
	else {
		lib_addr_range[++current_address_idx] = *lib;
#ifdef DEBUG
		if (lib_name)
			lib_addr_range[current_address_idx].lib_name =
				strdup(lib_name);
#else
		(void)lib_name;
#endif /* DEBUG */
	}
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

	while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
		ret_code = regexec(&compiledReg, line, 0, NULL, 0);

		// regex not matched
		if (ret_code)
			continue;
		// find - character separating memory start,end values
		while (end_mem_offset < MAX_LINE_LENGTH &&
		       line[end_mem_offset] != '-')
			++end_mem_offset;

		if (end_mem_offset >= MAX_LINE_LENGTH) {
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
#ifdef DEBUG
		line[end_mem_offset] = ' ';
		int line_len = strlen(line);
		line_len -= 1;
		line[line_len] = '\0';
		char *buff = NULL;
		if (line[line_len - 1] != ' ') {
			int it = line_len - 1;
			while (line[it] > 0 && line[it] != ' ')
				--it;
			buff = strdup(line + it);
			log("Adding: %s", buff);
		}
		add_new_library(&lib_addr, buff);
		free(buff);
#else
		add_new_library(&lib_addr, NULL);
#endif /* DEBUG */
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
#ifdef DEBUG
		lib_addr_range[library_it].lib_name = NULL;
#endif /* DEBUG */
	}
	parse_file();
}

void in_library_destructor(void)
{
#ifdef DEBUG
	long library_it;
	for (library_it = 0; library_it < current_address_idx; ++library_it) {
		free(lib_addr_range[library_it].lib_name);
		lib_addr_range[library_it].lib_name = NULL;
	}
#endif /* DEBUG */
}

int in_library(const void *address)
{
	long library_it;
	unsigned long addr;
	addr = (unsigned long)address;
	for (library_it = 0; library_it < current_address_idx; ++library_it) {
		if (addr > lib_addr_range[library_it].start &&
		    addr < lib_addr_range[library_it].end) {
#ifdef DEBUG
			log("Addr %p in_library: %s.", address,
			    lib_addr_range[library_it].lib_name);
#endif /* DEBUG */
			return 1;
		}
	}
	log("Addr %p no in_library.", address);
	return 0;
}
