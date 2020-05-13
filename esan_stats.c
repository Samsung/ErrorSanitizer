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
#include "stats.h"
#include <stdio.h>

typedef struct stats {
	unsigned long esan_nr_executions;
	unsigned long esan_nr_failed_executions;
} stats;

struct stats obj_stats[ESAN_NR_FUNCTIONS];

static const char *const ESAN_FUNCTION_NAMES[ESAN_NR_FUNCTIONS] = {
	"calloc",
	"malloc",
	"realloc",
	"strdup",
	"strndup",
	"fopen",
	"fclose",
	"fwrite",
	"fputs",
	"rewind",
	"ftell",
	"ftello",
	"fcntl",
	"access",
	"faccessat",
	"chmod",
	"fchmod",
	"EVP_CIPHER_CTX_new",
	"EVP_MD_CTX_create"
};

void add_execution(enum ESAN_FUNCTIONS_E fun_name,
		   enum ESAN_FAILURE_E fail_status)
{
	obj_stats[fun_name].esan_nr_executions++;
	obj_stats[fun_name].esan_nr_failed_executions += fail_status;
}

void esan_print_stats(void)
{
	unsigned long esan_all_failed_executions = 0, esan_all_executions = 0;
	unsigned i;

	printf("\nError Sanitizer stats\n");
	printf("--------------------|----------|-----------------\n");
	printf(" %-19s|%-10s|%-17s\n", "Function ", " nr execs ",
	       " nr failed execs ");
	printf("--------------------|----------|-----------------\n");

	for (i = 0; i < ESAN_LAST_FUNCTION; ++i) {
		esan_all_failed_executions +=
			obj_stats[i].esan_nr_failed_executions;
		esan_all_executions += obj_stats[i].esan_nr_executions;
		if (obj_stats[i].esan_nr_executions > 0)
			printf(" %-18s | %8ld | %15ld\n",
			       ESAN_FUNCTION_NAMES[i],
			       obj_stats[i].esan_nr_executions,
			       obj_stats[i].esan_nr_failed_executions);
	}

	printf("--------------------|----------|-----------------\n");
	printf(" %-18s | %8ld | %15ld\n", "TOTAL ", esan_all_executions,
	       esan_all_failed_executions);
	printf("--------------------|----------|-----------------\n");
}
