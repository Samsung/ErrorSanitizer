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
#ifndef _ERROR_SANITIZER_H
#define _ERROR_SANITIZER_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "esan_fail.h"

extern char *esan_error_bitmap;
extern size_t esan_error_bitmap_size;

uint8_t *esan_split_input_file(uint8_t *data, size_t size);
void esan_initialize(uint8_t *bitmap_ptr, size_t bitmap_size);
void parse_map(int argc, char **argv, const char *const *envp);
void parse_map_cleanup(void);

#endif /* _ERROR_SANITIZER_H */
