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

// FILE* fopen( const char* filename, const char* mode);

typedef FILE*(*fopen_func_t) ( const char* filename, const char* mode);

FILE* real_fopen( const char* filename, const char* mode)
{
    static fopen_func_t fopen_func_ptr = NULL;
    if (NULL == fopen_func_ptr)
        fopen_func_ptr = dlsym(RTLD_NEXT, "fopen");
    if (NULL != fopen_func_ptr)
        return (*fopen_func_ptr)(filename, mode);
    else {
        fprintf(stderr, "Error in dlsym - %s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
        exit(-1);
    }
}

FILE* fopen( const char* filename, const char* mode)
{
    ESAN_DEBUG("%s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
    esan_nr_executions[ESAN_FOPEN] += 1;
    if (esan_should_I_fail()) {
        esan_fail_message(ESAN_FUNCTION_NAMES[ESAN_FOPEN]);
        esan_nr_failed_executions[ESAN_FOPEN] += 1;
        return NULL;
    } else {
        return real_fopen(filename, mode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////

// off_t ftello(FILE *stream);

typedef off_t(*ftello_func_t) (FILE *stream);

off_t real_ftello(FILE *stream)
{
    static ftello_func_t ftello_func_ptr = NULL;
    if (NULL == ftello_func_ptr)
        ftello_func_ptr = dlsym(RTLD_NEXT, "ftello");
    if (NULL != ftello_func_ptr)
        return (*ftello_func_ptr)(stream);
    else {
        ESAN_ERROR("Error in dlsym - %s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
        exit(-1);
    }
}

off_t ftello(FILE *stream)
{
    ESAN_DEBUG("%s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
    esan_nr_executions[ESAN_FTELLO] += 1;
    if (esan_should_I_fail()) {
        esan_fail_message(ESAN_FUNCTION_NAMES[ESAN_FTELLO]);
        esan_nr_failed_executions[ESAN_FTELLO] += 1;
        return -1;
    } else {
        return real_ftello(stream);
    }
}

////////////////////////////////////////////////////////////////////////////////////////

// long ftell(FILE *stream);

typedef long(*ftell_func_t) (FILE *stream);

long real_ftell(FILE *stream)
{
    static ftell_func_t ftell_func_ptr = NULL;
    if (NULL == ftell_func_ptr)
        ftell_func_ptr = dlsym(RTLD_NEXT, "ftell");
    if (NULL != ftell_func_ptr)
        return (*ftell_func_ptr)(stream);
    else {
        ESAN_ERROR("Error in dlsym - %s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
        exit(-1);
    }
}

long ftell(FILE *stream)
{
    ESAN_DEBUG("%s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
    esan_nr_executions[ESAN_FTELL] += 1;
    if (esan_should_I_fail()) {
        esan_fail_message(ESAN_FUNCTION_NAMES[ESAN_FTELL]);
        esan_nr_failed_executions[ESAN_FTELL] += 1;
        return -1; 
    } else {
        off_t off_t_result = real_ftello(stream);
        long long_result = real_ftell(stream);
        if (off_t_result != (off_t)long_result) {
            ESAN_ERROR("File size overflow when using ftell() - use ftello() instead! (See: FIO19-C)");
            exit(-1);
        }
        return long_result;
    }
}

////////////////////////////////////////////////////////////////////////////////////////

// int fclose(FILE *stream);

typedef int(*fclose_func_t) (FILE *stream);

int real_fclose(FILE *stream)
{
    static fclose_func_t fclose_func_ptr = NULL;
    if (NULL == fclose_func_ptr)
        fclose_func_ptr = dlsym(RTLD_NEXT, "fclose");
    if (NULL != fclose_func_ptr)
        return (*fclose_func_ptr)(stream);
    else {
        fprintf(stderr, "Error in dlsym - %s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
        exit(-1);
    }
}

int fclose(FILE *stream)
{
    ESAN_DEBUG("%s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
    esan_nr_executions[ESAN_FCLOSE] += 1;
    if (esan_should_I_fail()) {
        esan_fail_message(ESAN_FUNCTION_NAMES[ESAN_FCLOSE]);
        esan_nr_failed_executions[ESAN_FCLOSE] += 1;
        return EOF;
    } else {
        return real_fclose(stream);
    }
}

////////////////////////////////////////////////////////////////////////////////////////

// void rewind(FILE *stream);

typedef void(*rewind_func_t) (FILE *stream);

void real_rewind(FILE *stream)
{
    static rewind_func_t rewind_func_ptr = NULL;
    if (NULL == rewind_func_ptr)
        rewind_func_ptr = dlsym(RTLD_NEXT, "rewind");
    if (NULL != rewind_func_ptr)
        (*rewind_func_ptr)(stream);
    else {
        ESAN_ERROR("Error in dlsym - %s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
        exit(-1);
    }
}

void rewind(FILE *stream)
{
    ESAN_DEBUG("%s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
    esan_nr_executions[ESAN_REWIND] += 1;
    if (esan_should_I_fail()) {
        esan_fail_message(ESAN_FUNCTION_NAMES[ESAN_REWIND]);
        esan_nr_failed_executions[ESAN_REWIND] += 1;
        ESAN_ERROR("%s is obsolete function - see SEI CERT C Coding Standard MSC24-C for details!\n", ESAN_FUNCTION_NAMES[ESAN_REWIND]);
        exit(-3);
    } else {
        real_rewind(stream);
    }
}

////////////////////////////////////////////////////////////////////////////////////////

// int fputs(const char * str, FILE * stream);

typedef int(*fputs_func_t) (const char * str, FILE * stream);

int real_fputs(const char * str, FILE * stream)
{
    static fputs_func_t fputs_func_ptr = NULL;
    if (NULL == fputs_func_ptr)
        fputs_func_ptr = dlsym(RTLD_NEXT, "fputs");
    if (NULL != fputs_func_ptr)
        return (*fputs_func_ptr)(str, stream);
    else {
        fprintf(stderr, "Error in dlsym - %s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
        exit(-1);
    }
}

int fputs(const char * str, FILE * stream)
{
    ESAN_DEBUG("%s %s:%d\n", __FILE__, __FUNCTION__, __LINE__);
    esan_nr_executions[ESAN_FPUTS] += 1;
    if (esan_should_I_fail()) {
        esan_fail_message(ESAN_FUNCTION_NAMES[ESAN_FPUTS]);
        esan_nr_failed_executions[ESAN_FPUTS] += 1;
        return EOF;
    } else {
        return real_fputs(str, stream);
    }
}


