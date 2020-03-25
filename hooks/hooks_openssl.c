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
#include "hooks_include.h"

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <string.h>

/* EVP_CIPHER_CTX* EVP_CIPHER_CTX_new(void); */

typedef EVP_CIPHER_CTX *(*EVP_CIPHER_CTX_new_func_t)(void);

EVP_CIPHER_CTX *esan_on_fail_EVP_CIPHER_CTX_new(void)
{
	return NULL;
}

EVP_CIPHER_CTX *real_EVP_CIPHER_CTX_new(void)
{
	static EVP_CIPHER_CTX_new_func_t EVP_CIPHER_CTX_new_func_ptr = NULL;
	if (NULL == EVP_CIPHER_CTX_new_func_ptr)
		EVP_CIPHER_CTX_new_func_ptr = (EVP_CIPHER_CTX_new_func_t)dlsym(
			RTLD_NEXT, "EVP_CIPHER_CTX_new");
	if (NULL != EVP_CIPHER_CTX_new_func_ptr)
		return (*EVP_CIPHER_CTX_new_func_ptr)();

	ESAN_ERROR("Error in dlsym - in 'EVP_CIPHER_CTX_new' wrapper\n");
	exit(-1);
}

EVP_CIPHER_CTX *EVP_CIPHER_CTX_new(void)
{
	ESAN_DEBUG("%s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
	obj_stats[ESAN_EVP_CIPHER_CTX_NEW].esan_nr_executions += 1;
	if (esan_should_I_fail()) {
		fail_common();
		esan_fail_message("EVP_CIPHER_CTX_new");
		obj_stats[ESAN_EVP_CIPHER_CTX_NEW].esan_nr_failed_executions +=
			1;
		return esan_on_fail_EVP_CIPHER_CTX_new();
	} else {
		return real_EVP_CIPHER_CTX_new();
	}
}

/**************************************************************************************/
#if OPENSSL_VERSION_NUMBER >= 0x11000000L
/* EVP_MD_CTX* EVP_MD_CTX_create(void); */

typedef EVP_MD_CTX *(*EVP_MD_CTX_create_func_t)(void);

EVP_MD_CTX *esan_on_fail_EVP_MD_CTX_create(void)
{
	return NULL;
}

EVP_MD_CTX *real_EVP_MD_CTX_create(void)
{
	static EVP_MD_CTX_create_func_t EVP_MD_CTX_create_func_ptr = NULL;
	if (NULL == EVP_MD_CTX_create_func_ptr)
		EVP_MD_CTX_create_func_ptr = (EVP_MD_CTX_create_func_t)dlsym(
			RTLD_NEXT, "EVP_MD_CTX_create");
	if (NULL != EVP_MD_CTX_create_func_ptr)
		return (*EVP_MD_CTX_create_func_ptr)();

	ESAN_ERROR("Error in dlsym - in 'EVP_MD_CTX_create' wrapper\n");
	exit(-1);
}

EVP_MD_CTX *EVP_MD_CTX_create(void)
{
	ESAN_DEBUG("%s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
	obj_stats[ESAN_EVP_MD_CTX_CREATE].esan_nr_executions += 1;
	if (esan_should_I_fail()) {
		fail_common();
		esan_fail_message("EVP_MD_CTX_create");
		obj_stats[ESAN_EVP_MD_CTX_CREATE].esan_nr_failed_executions +=
			1;
		return esan_on_fail_EVP_MD_CTX_create();
	} else {
		return real_EVP_MD_CTX_create();
	}
}
#endif
