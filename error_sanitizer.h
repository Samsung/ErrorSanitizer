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
#ifndef	_ERROR_SANITIZER_H
#define	_ERROR_SANITIZER_H

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

extern uint8_t esan_always_succeed;
extern uint8_t *esan_error_bitmap;
extern size_t esan_error_bitmap_size;

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
    ESAN_EVP_CIPHER_CTX_NEW,
    ESAN_EVP_MD_CTX_CREATE,
    ESAN_LAST_FUNCTION,  // End marker.
    ESAN_NR_FUNCTIONS  // Counts how many functions are defined.
};

uint8_t* esan_split_input_file(uint8_t* data, size_t size);

void esan_initialize(uint8_t* bitmap_ptr, size_t bitmap_size);

void esan_fail_message(const char* function_name);

#endif // #ifndef	_ERROR_SANITIZER_H
