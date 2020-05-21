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
#ifndef ESAN_STATS_H_
#define ESAN_STATS_H_

enum ESAN_FUNCTIONS_E {
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
	ESAN_ACCESS,
	ESAN_FACCESSAT,
	ESAN_CHMOD,
	ESAN_FCHMOD,
#ifndef ESAN_DISABLE_HOOKS_OPENSSL
	ESAN_EVP_CIPHER_CTX_NEW,
	ESAN_EVP_MD_CTX_CREATE,
#endif /* ESAN_DISABLE_HOOKS_OPENSSL */
	ESAN_LAST_FUNCTION, /* End marker. */
	ESAN_NR_FUNCTIONS /* Counts how many functions are defined. */
};

enum ESAN_FAILURE_E {
	ESAN_SUCCEED,
	ESAN_FAIL,
};

void add_execution(enum ESAN_FUNCTIONS_E fun_name,
		   enum ESAN_FAILURE_E fail_status);
void esan_print_stats(void);

#endif /* ESAN_STATS_H_ */
