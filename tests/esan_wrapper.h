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

#include "error_sanitizer.h"

#define MAX_FILE_SIZE (1024)

inline static void exit_print(const char* msg, int code)
{
	if(msg != NULL)
		fprintf(stderr, "%s\n", msg);
	exit(code);
}

extern int perform_testing(uint8_t* buffer_ptr, size_t buffer_size);

inline static uint8_t* read_data_from_file(const char *path, size_t *ptr_data_size)
{
	FILE *file_desc;
	int result = -1;
	uint8_t *buffer = NULL;
	unsigned i;
	if (path == NULL)
		return NULL;

	struct stat stat_str;
	result = stat(path, &stat_str);
	if (result != 0)
		return NULL;		

	file_desc = fopen(path, "r");
	if (file_desc == NULL)
		return NULL;
	size_t data_size = stat_str.st_size;

	buffer = (uint8_t*)malloc(data_size+1);
	if (buffer == NULL)
        exit(-1);

	buffer[data_size] = 0;

	size_t data_size_read = fread(buffer, 1, data_size, file_desc);
	assert(data_size_read == data_size);
	*ptr_data_size = data_size;

	for (i = 0; i < data_size+1; ++i)
		printf("%02hhX ", (unsigned char) (buffer[i] & 0xff));
	printf("\n");

	fclose(file_desc);
	return buffer;
}


inline static int main0(int argc, char* argv[])
{
    uint8_t *buff_ptr, *esan_bitmap, *test_buff_ptr = NULL;
    size_t buff_size = 0, test_buff_size = 0;
	int i = 0;

	if(argc != 2)
		exit_print("Wrong number of arguments.  Expected 1 argument - file path!\n", -1);

	buff_ptr = read_data_from_file(argv[1], &buff_size);

	if (buff_size == 0)
		exit_print("Input file is empty!\n", -1);

	printf("size of input data = %zu\n", buff_size);

    esan_bitmap = esan_split_input_file(buff_ptr, buff_size);

    if (NULL != esan_bitmap) {
        esan_initialize(buff_ptr, esan_bitmap-buff_ptr-4);
        test_buff_ptr = esan_bitmap;
        test_buff_size = buff_size - (esan_bitmap - buff_ptr);
        printf("offset of esan_error_bitmap = %ld\n", (esan_bitmap - buff_ptr));
    } else {
        esan_initialize(buff_ptr, buff_size);
        test_buff_ptr = buff_ptr;
        test_buff_size = buff_size;
    }

    if (NULL == esan_error_bitmap) {
        esan_error_bitmap = buff_ptr;
        esan_error_bitmap_size = buff_size;
    }

	printf("start testing!!\n");
    perform_testing(test_buff_ptr, test_buff_size);
	printf("finished testing!!\n");

 	free(buff_ptr);
    exit(0);
	return 0;
}

