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
    Author: Mateusz Nosek <m.nosek@samsung.com>
*/
*/
#include "error_sanitizer.h"

unsigned long esan_total_nr_execs = 0;

unsigned long esan_nr_executions[ESAN_NR_FUNCTIONS];
unsigned long esan_nr_failed_executions[ESAN_NR_FUNCTIONS];

const char* const ESAN_FUNCTION_NAMES[ESAN_NR_FUNCTIONS] = {
    "calloc",
    "malloc",
    "realloc",
    "strdup",
    "strndup",
    "fopen",
    "fclose",
    "fwrite",
    "fputs",
    "rewind",
    "ftell",
    "ftello",
    "EVP_CIPHER_CTX_new",
    "EVP_MD_CTX_create"
};

void esan_print_stats(void)
{
    unsigned long esan_all_failed_executions = 0;
    for (int i=0; i<ESAN_LAST_FUNCTION; ++i) 
        esan_all_failed_executions += esan_nr_failed_executions[i];
    printf("\nError Sanitizer stats\n");
    printf("--------------------|----------|-----------------\n");
    printf(" %-19s|%-10s|%-17s\n", "Function ", " nr execs ", " nr failed execs ");
    printf("--------------------|----------|-----------------\n");
    for (int i=0; i<ESAN_LAST_FUNCTION; ++i)
        if (esan_nr_executions[i] > 0)
            printf(" %-18s | %8ld | %15ld\n", ESAN_FUNCTION_NAMES[i], esan_nr_executions[i], esan_nr_failed_executions[i]);
    printf("--------------------|----------|-----------------\n");
    printf(" %-18s | %8ld | %15ld\n", "TOTAL ", esan_total_nr_execs, esan_all_failed_executions);
    printf("--------------------|----------|-----------------\n");
}
// void exit(int status);

typedef void(*exit_func_t) (int status);

// Wrapper for exit() function that displays ESAN stats on exit
void exit(int status)
{
    esan_print_stats();

    static exit_func_t exit_func_ptr = NULL;
    if (NULL == exit_func_ptr)
        exit_func_ptr = dlsym(RTLD_NEXT, "exit");
    if (NULL != exit_func_ptr)
        (*exit_func_ptr)(status);
    else
        fprintf(stderr, "Error in dlsym - %s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
    abort();
}


