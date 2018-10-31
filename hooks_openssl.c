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
#include "error_sanitizer_local.h"

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

// EVP_CIPHER_CTX* EVP_CIPHER_CTX_new(void);

typedef EVP_CIPHER_CTX*(*EVP_CIPHER_CTX_new_func_t) (void);

EVP_CIPHER_CTX* esan_on_fail_EVP_CIPHER_CTX_new(void)
{
    return NULL; 
}

EVP_CIPHER_CTX* real_EVP_CIPHER_CTX_new(void)
{
    static EVP_CIPHER_CTX_new_func_t EVP_CIPHER_CTX_new_func_ptr = NULL;
    if (NULL == EVP_CIPHER_CTX_new_func_ptr)
        EVP_CIPHER_CTX_new_func_ptr = dlsym(RTLD_NEXT, "EVP_CIPHER_CTX_new");
    if (NULL != EVP_CIPHER_CTX_new_func_ptr)
        return (*EVP_CIPHER_CTX_new_func_ptr)();
    else {
        ESAN_ERROR("Error in dlsym - %s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
        exit(-1);
    }
}

EVP_CIPHER_CTX* EVP_CIPHER_CTX_new(void)
{
    ESAN_DEBUG("%s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
    esan_nr_executions[ESAN_EVP_CIPHER_CTX_NEW] += 1;
    if (esan_should_I_fail()) {
        esan_fail_message(ESAN_FUNCTION_NAMES[ESAN_EVP_CIPHER_CTX_NEW]);
        esan_nr_failed_executions[ESAN_EVP_CIPHER_CTX_NEW] += 1;
        return esan_on_fail_EVP_CIPHER_CTX_new();
    } else {
        return real_EVP_CIPHER_CTX_new();
    }
}

////////////////////////////////////////////////////////////////////////////////////////

// EVP_MD_CTX* EVP_MD_CTX_create(void);

typedef EVP_MD_CTX*(*EVP_MD_CTX_create_func_t) (void);

EVP_MD_CTX* esan_on_fail_EVP_MD_CTX_create(void)
{
    return NULL; 
}

EVP_MD_CTX* real_EVP_MD_CTX_create(void)
{
    static EVP_MD_CTX_create_func_t EVP_MD_CTX_create_func_ptr = NULL;
    if (NULL == EVP_MD_CTX_create_func_ptr)
        EVP_MD_CTX_create_func_ptr = dlsym(RTLD_NEXT, "EVP_MD_CTX_create");
    if (NULL != EVP_MD_CTX_create_func_ptr)
        return (*EVP_MD_CTX_create_func_ptr)();
    else {
        ESAN_ERROR("Error in dlsym - %s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
        exit(-1);
    }
}

EVP_MD_CTX* EVP_MD_CTX_create(void)
{
    ESAN_DEBUG("%s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
    esan_nr_executions[ESAN_EVP_MD_CTX_CREATE] += 1;
    if (esan_should_I_fail()) {
        esan_fail_message(ESAN_FUNCTION_NAMES[ESAN_EVP_MD_CTX_CREATE]);
        esan_nr_failed_executions[ESAN_EVP_MD_CTX_CREATE] += 1;
        return esan_on_fail_EVP_MD_CTX_create();
    } else {
        return real_EVP_MD_CTX_create();
    }
}

////////////////////////////////////////////////////////////////////////////////////////


