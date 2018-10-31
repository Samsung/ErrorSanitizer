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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <unistd.h>

int perform_testing(uint8_t* buffer_ptr, size_t buffer_size)
{
    // correct usage with error handling
    {
        char* test_strdup = strdup("asdads");
        if (test_strdup) {
            printf("strdup!! %p %s \n", test_strdup, test_strdup);
            test_strdup[0] = '1';
            free(test_strdup);
        }

        char* test_strndup = strndup("asdads", 7);
        if (test_strndup) {
            printf("strndup!! %p %s \n", test_strdup, test_strdup);
            test_strndup[0] = '1';
            free(test_strndup);
        }
    }

    // incorrect usage without error handling
    {
        char* test_strdup = strdup("asdads");
        printf("strdup!! %p %s \n", test_strdup, test_strdup);
        test_strdup[0] = '1';

        char* test_strndup = strndup("asdads", 7);
        printf("strndup!! %p %s \n", test_strdup, test_strdup);
        test_strndup[0] = '1';

        free(test_strdup);
        free(test_strndup);
    }

    return 1;
}

