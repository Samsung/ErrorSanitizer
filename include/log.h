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
#ifndef ESAN_LOG_H
#define ESAN_LOG_H

#include <stdio.h>
#include <stdlib.h>

enum ESAN_ERROR_CODE_E {
	ESAN_SUCCESS = 0,
	ESAN_INTERNAL_ERROR = 1,
	ESAN_LIBRARY_FUNCTION_ERROR = 2,
	ESAN_INVALID_ARGUMENT = 3,
	ESAN_DLSYM_ERROR = 4
};

#ifdef ESAN_COVERAGE
#define GCOV_CUSTOM_FLUSH                \
	do {                             \
		void __gcov_flush(void); \
		__gcov_flush();          \
	} while (0)
#else
#define GCOV_CUSTOM_FLUSH \
	do {              \
	} while (0)
#endif

#ifdef AFL
#define exit_failure(err_code, message, args...) \
	do {                                     \
		GCOV_CUSTOM_FLUSH;               \
		exit(err_code);                  \
	} while (0)
#else
#define exit_failure(err_code, message, args...)                         \
	do {                                                             \
		fprintf(stderr, "%s | %s:%d | ", __FILE__, __FUNCTION__, \
			__LINE__);                                       \
		fprintf(stderr, message, ##args);                        \
		fprintf(stderr, "\n");                                   \
		fflush(stderr);                                          \
		GCOV_CUSTOM_FLUSH;                                       \
		exit(err_code);                                          \
	} while (0)
#endif /* AFL */

#define exit_success(message, args...) \
	exit_failure(ESAN_SUCCESS, message, ##args)

#ifdef DEBUG
#define log(message, args...)                                            \
	do {                                                             \
		fprintf(stderr, "%s | %s:%d | ", __FILE__, __FUNCTION__, \
			__LINE__);                                       \
		fprintf(stderr, message, ##args);                        \
		fprintf(stderr, "\n");                                   \
		fflush(stderr);                                          \
	} while (0)
#else
#define log(message, args...) \
	do {                  \
	} while (0)
#endif /* DEBUG */
#endif /* ESAN_LOG_H */
