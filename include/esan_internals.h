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
#ifndef ESAN_INTERNALS_H_
#define ESAN_INTERNALS_H_

#include "esan_fail.h"
#include "in_library.h"

enum ESAN_FUNCTIONS {
	ESAN_CALLOC,
	ESAN_MALLOC,
	ESAN_REALLOC,
	ESAN_STRDUP,
	ESAN_STRNDUP,
	ESAN_FOPEN,
	ESAN_FCLOSE,
	ESAN_FWRITE,
	ESAN_FPUTS,
	ESAN_REWIND,
	ESAN_FTELL,
	ESAN_FTELLO,
	ESAN_FCNTL,
	ESAN_ACCESS,
	ESAN_FACCESSAT,
	ESAN_CHMOD,
	ESAN_FCHMOD,
	ESAN_EVP_CIPHER_CTX_NEW,
	ESAN_EVP_MD_CTX_CREATE,
	ESAN_LAST_FUNCTION, /* End marker. */
	ESAN_NR_FUNCTIONS /* Counts how many functions are defined. */
};

typedef struct stats {
	unsigned long esan_nr_executions;
	unsigned long esan_nr_failed_executions;
} stats;

extern struct stats obj_stats[ESAN_NR_FUNCTIONS];

unsigned int get_failure_status_from_map(void);

#define esan_should_I_fail()                                                  \
	({                                                                    \
		const void *tmp = __builtin_return_address(0);                \
		enum ESAN_FAILURE_STATUS_E failure_status =                   \
			esan_get_failure_status();                            \
		int ret_code;                                                 \
		if (failure_status == ESAN_ALWAYS_SUCCEED || in_library(tmp)) \
			ret_code = 0;                                         \
		else if (failure_status == ESAN_ALWAYS_FAIL)                  \
			ret_code = 1;                                         \
		/* failure_status == ESAN_MAP_BASED_FAILURE */                \
		/* Get failure status from map */                             \
		else                                                          \
			ret_code = !!(get_failure_status_from_map());         \
		(ret_code);                                                   \
	})

void esan_fail_message(const char *function_name);

#endif /* ESAN_INTERNALS_H_ */
