/*
    Copyright (c) 2018 - 2019, Samsung Electronics Co., Ltd. All rights reserved.

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
    Author: Mateusz Nosek <m.nosek@samsung.com>
*/
#include "hooks_include.h"

/**************************************************************************************/

/* void* calloc(size_t num, size_t size); */

typedef void*(*calloc_func_t) (size_t num, size_t size);

void* real_calloc(size_t num, size_t size)
{
    static calloc_func_t calloc_func_ptr = NULL;
    if (NULL == calloc_func_ptr)
        calloc_func_ptr = (calloc_func_t)dlsym(RTLD_NEXT, "calloc");
    if (NULL != calloc_func_ptr)
        return (*calloc_func_ptr)(num, size);

    ESAN_ERROR("Error in dlsym - in 'calloc' wrapper\n");
    exit(-1);
}

void* calloc(size_t num, size_t size)
{
    ESAN_DEBUG("%s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
    obj_stats[ESAN_CALLOC].esan_nr_executions += 1;
    if (esan_should_I_fail()) {
		fail_common();
        esan_fail_message("calloc");
        obj_stats[ESAN_CALLOC].esan_nr_failed_executions += 1;
        return NULL;
    } else {
        return real_calloc(num, size);
    }
}

/**************************************************************************************/

/* void* malloc(size_t size); */

typedef void*(*malloc_func_t) (size_t size);

void* real_malloc(size_t size)
{
    static malloc_func_t malloc_func_ptr = NULL;
    if (NULL == malloc_func_ptr)
        malloc_func_ptr = (malloc_func_t)dlsym(RTLD_NEXT, "malloc");
    if (NULL != malloc_func_ptr)
        return (*malloc_func_ptr)(size);

    ESAN_ERROR("Error in dlsym - in 'malloc' wrapper\n");
    exit(-1);
}

void* malloc(size_t size)
{
    obj_stats[ESAN_MALLOC].esan_nr_executions += 1;
    if (esan_should_I_fail()) {
		fail_common();
        esan_fail_message("malloc");
        obj_stats[ESAN_MALLOC].esan_nr_failed_executions += 1;
        return NULL;
    } else {
        return real_malloc(size);
    }
}

/**************************************************************************************/

/* void *realloc(void *ptr, size_t new_size); */

typedef void*(*realloc_func_t) (void *ptr, size_t new_size);

void* real_realloc(void *ptr, size_t new_size)
{
    static realloc_func_t realloc_func_ptr = NULL;
    if (NULL == realloc_func_ptr)
        realloc_func_ptr = (realloc_func_t)dlsym(RTLD_NEXT, "realloc");
    if (NULL != realloc_func_ptr)
        return (*realloc_func_ptr)(ptr, new_size);

    ESAN_ERROR("Error in dlsym - in 'realloc' wrapper\n");
    exit(-1);
}

void* realloc(void *ptr, size_t new_size)
{
    ESAN_DEBUG("%s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
    obj_stats[ESAN_REALLOC].esan_nr_executions += 1;
    if (esan_should_I_fail()) {
		fail_common();
        esan_fail_message("realloc");
        obj_stats[ESAN_REALLOC].esan_nr_failed_executions += 1;
        return NULL;
    } else {
        return real_realloc(ptr, new_size);
    }
}

