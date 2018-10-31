/*
    Copyright (c) 2018, Samsung Electronics Co., Ltd. All rights reserved.

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

    Author: Jakub Botwicz <j.botwicz@samsung.com>
*/
#ifndef	_ERROR_SANITIZER_LOCAL_H
#define	_ERROR_SANITIZER_LOCAL_H

#include "error_sanitizer.h"

extern unsigned long esan_all_nr_executions;

extern unsigned long esan_nr_executions[ESAN_NR_FUNCTIONS];
extern unsigned long esan_nr_failed_executions[ESAN_NR_FUNCTIONS];

extern const char* const ESAN_FUNCTION_NAMES[ESAN_NR_FUNCTIONS];

int esan_should_I_fail(void);

#define ESAN_DEBUG(...) fprintf(stdout, "====: ErrorSanitizer: DBG: " __VA_ARGS__)
#define ESAN_ERROR(...) fprintf(stderr, "====: ErrorSanitizer: " __VA_ARGS__)

#endif // #ifndef	_ERROR_SANITIZER_LOCAL_H
