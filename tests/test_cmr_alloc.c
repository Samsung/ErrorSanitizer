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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <unistd.h>

#include "esan_wrapper.h"

int perform_testing(uint8_t* buffer_ptr, size_t buffer_size)
{
    {
        char* test_malloc_buffer = (char*)malloc(100);
        if (test_malloc_buffer) {
            test_malloc_buffer[0] = '1';
            free(test_malloc_buffer);
        }

        char* test_calloc_buffer = (char*)calloc(100, 1);
        if (test_calloc_buffer) {
            test_calloc_buffer[0] = '1';
            free(test_calloc_buffer);
        }

        char* test_realloc_buffer = (char*)realloc(NULL, 100);
        if (test_realloc_buffer) {
            test_realloc_buffer[0] = '1';
            free(test_realloc_buffer);
        }

        test_realloc_buffer = (char*)realloc(NULL, 200);
        if (test_realloc_buffer) {
            test_realloc_buffer = (char*)realloc(NULL, 300);
            if (test_realloc_buffer)
                test_realloc_buffer = (char*)realloc(NULL, 300);
        }
        free(test_realloc_buffer);
    }

    {
        char* test_malloc_buffer = (char*)malloc(100);
        test_malloc_buffer[0] = '1';
        free(test_malloc_buffer);

        char* test_calloc_buffer = (char*)calloc(100, 1);
        test_calloc_buffer[0] = '1';
        free(test_calloc_buffer);

        char* test_realloc_buffer = (char*)realloc(NULL, 100);
        test_realloc_buffer[0] = '1';
        free(test_realloc_buffer);
    }

    return 1;
}

int main(int argc, char **argv) {return main0(argc, argv);}

