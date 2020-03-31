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
#include "esan_mutex.h"
#include "esan_visibility.h"
#include "log.h"
#include "stats.h"
#include <stdio.h>

typedef struct stats {
	unsigned long esan_nr_executions;
	unsigned long esan_nr_failed_executions;
} stats;

static struct stats obj_stats[ESAN_NR_FUNCTIONS];
static enum ESAN_FUNCTIONS_E last_fallen = ESAN_NR_FUNCTIONS;

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
	"access",
	"faccessat",
	"chmod",
	"fchmod",
	"tmpfile",
#ifndef ESAN_DISABLE_HOOKS_OPENSSL
	"EVP_CIPHER_CTX_new",
	"EVP_MD_CTX_create"
#endif /* ESAN_DISABLE_HOOKS_OPENSSL */
};

HIDE void add_execution(enum ESAN_FUNCTIONS_E fun_name,
			enum ESAN_FAILURE_E fail_status)
{
	obj_stats[fun_name].esan_nr_executions++;
	obj_stats[fun_name].esan_nr_failed_executions += fail_status;
	if (fail_status)
		last_fallen = fun_name;
}

void esan_print_stats(void)
{
	static unsigned execute_once = 0;
	unsigned long esan_all_failed_executions = 0, esan_all_executions = 0;
	unsigned i;

	esan_mutex_lock();
	if (execute_once++) {
		log("esan_print_stats called %u times.", execute_once);
		esan_mutex_unlock();
		return;
	}
	fprintf(stderr, "\nError Sanitizer stats\n");
	fprintf(stderr, "--------------------|----------|-----------------\n");
	fprintf(stderr, " %-19s|%-10s|%-17s\n", "Function ", " nr execs ",
		" nr failed execs ");
	fprintf(stderr, "--------------------|----------|-----------------\n");

	for (i = 0; i < ESAN_LAST_FUNCTION; ++i) {
		esan_all_failed_executions +=
			obj_stats[i].esan_nr_failed_executions;
		esan_all_executions += obj_stats[i].esan_nr_executions;
		if (obj_stats[i].esan_nr_executions > 0)
			fprintf(stderr, " %-18s | %8ld | %15ld\n",
				ESAN_FUNCTION_NAMES[i],
				obj_stats[i].esan_nr_executions,
				obj_stats[i].esan_nr_failed_executions);
	}

	fprintf(stderr, "--------------------|----------|-----------------\n");
	fprintf(stderr, " %-18s | %8ld | %15ld\n", "TOTAL ",
		esan_all_executions, esan_all_failed_executions);
	fprintf(stderr, "--------------------|----------|-----------------\n");
	if (ESAN_NR_FUNCTIONS != last_fallen) {
		fprintf(stderr, "Last fallen: %s\n",
			ESAN_FUNCTION_NAMES[last_fallen]);
		fprintf(stderr,
			"--------------------|----------|-----------------\n");
	}
	esan_mutex_unlock();
}
