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
#include <malloc.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "error_sanitizer.h"
#include "log.h"

#define MAX_LINE_LENGHT (32 * 4096ul)
char *esan_error_bitmap = NULL;
size_t esan_error_bitmap_size = 0;

static const char SPLIT_STRING[] = "XXXX";
// do not include null byte '\0' when counting string size
#define SPLIT_STRING_SIZE (sizeof(SPLIT_STRING) - 1)
#define TMP_EXTENSION_SIZE 5
#define ENV_ARRAY_START_SIZE 10
static const char afl_input_file_regex[] = "\\.cur_input$";
static const char *proc_cmdline_path = "/proc/self/cmdline";
static const char *proc_env_path = "/proc/self/environ";
static size_t env_array_len = ENV_ARRAY_START_SIZE;
static size_t env_array_it = 0;
static char **env_array = NULL;
static char *env_buffer = NULL;

static enum ESAN_ERROR_CODE_E
read_file(const char *file_path, char **file_contents, size_t *output_size)
{
	char *buffer;
	size_t current_bytes_read, bytes_read = 0;
	FILE *fp = fopen(file_path, "r");
	if (!fp) {
		log("Unable to open %s file.", file_path);
		return ESAN_LIBRARY_FUNCTION_ERROR;
	}
	buffer = (char *)malloc(MAX_LINE_LENGHT);
	if (buffer == NULL) {
		fclose(fp);
		log("Unable to allocate %ldb memory for: %s file.",
		    MAX_LINE_LENGHT, file_path);
		return ESAN_LIBRARY_FUNCTION_ERROR;
	}
	while (!feof(fp)) {
		current_bytes_read =
			fread(buffer + bytes_read, 1, MAX_LINE_LENGHT, fp);
		log("read: %ld from: %s", current_bytes_read, file_path);
		bytes_read += current_bytes_read;
		buffer = realloc(buffer, bytes_read + MAX_LINE_LENGHT);
		if (!buffer) {
			log("Unable to reallocate %ldb memory for: %s file.",
			    bytes_read + MAX_LINE_LENGHT, file_path);
			fclose(fp);
			free(buffer);
			return ESAN_LIBRARY_FUNCTION_ERROR;
		}
	}
	fclose(fp);
	*file_contents = buffer;
	*output_size = bytes_read;
	return ESAN_SUCCESS;
}

#ifdef CP_WRONG_MAP
static enum ESAN_ERROR_CODE_E
write_file(const char *file_path, char *file_contents, size_t output_size)
{
	if (access(file_path, F_OK) != -1) {
		log("file: %s, already exist.", file_path);
		return ESAN_INTERNAL_ERROR;
	}
	FILE *fp = fopen(file_path, "w");
	size_t written_size = 0, current_written_size;
	if (!fp) {
		log("Unable to open %s file for write.", file_path);
		return ESAN_LIBRARY_FUNCTION_ERROR;
	}
	while (written_size < output_size) {
		current_written_size = fwrite(file_contents + written_size,
					      output_size - written_size, 1,
					      fp);
		if (current_written_size == 0) {
			log("unable to write: %ld size to %s file.",
			    current_written_size, file_path);
			fclose(fp);
			return ESAN_LIBRARY_FUNCTION_ERROR;
		}
		written_size += current_written_size;
	}
	if (fclose(fp) != 0) {
		log("Uanble to close file: %s with %ld size.", file_path,
		    written_size);
		return ESAN_LIBRARY_FUNCTION_ERROR;
	}
	return ESAN_SUCCESS;
}
#endif

static enum ESAN_ERROR_CODE_E parse_env(void)
{
	int ret_code;
	char *buffer_end, *buffer_it;
	size_t bytes_read;
	ret_code = read_file(proc_env_path, &env_buffer, &bytes_read);
	if (ret_code != ESAN_SUCCESS)
		return ret_code;

	buffer_it = env_buffer;
	buffer_end = env_buffer + bytes_read;
	env_array = malloc(sizeof(char *) * env_array_len);
	if (!env_array) {
		free(env_buffer);
		env_buffer = NULL;
		log("Unable to allocate: %ldb memory for environment array pointers.",
		    sizeof(char) * env_array_len);
		return ESAN_LIBRARY_FUNCTION_ERROR;
	}
	env_array_it = 0;
	while (buffer_it < buffer_end) {
		env_array[env_array_it++] = buffer_it;
		if (env_array_it == env_array_len - 1) {
			env_array = realloc(env_array,
					    sizeof(char *) * env_array_len * 2);
			if (!env_array) {
				free(env_buffer);
				env_buffer = NULL;
				log("Unable to reallocate: %ldb memory for environment array pointers.",
				    sizeof(char *) * env_array_len * 2);
				return ESAN_LIBRARY_FUNCTION_ERROR;
			}
			env_array_len *= 2;
		}
		// skip until next '\0' delimiter
		while (buffer_it < buffer_end && *buffer_it++)
			;
	}
	if (env_array_it == 0) {
		free(env_buffer);
		free(env_array);
		env_array = NULL;
		env_buffer = NULL;
		return ESAN_INTERNAL_ERROR;
	}
	return ESAN_SUCCESS;
}

static char *esan_getenv(const char *env)
{
	size_t it = 0;
	for (; it < env_array_it; ++it) {
		if (strncmp(env, env_array[it], strlen(env)) == 0)
			return env_array[it];
	}
	return NULL;
}

static char *find_bitmap_filepath()
{
	int ret_code;
	char *result;
	regex_t compiledReg;
	char *buffer, *buffer_it, *buffer_end;
	size_t bytes_read = 0;
	ret_code = read_file(proc_cmdline_path, &buffer, &bytes_read);

	if (ret_code != ESAN_SUCCESS) {
		return NULL;
	}
	ret_code = regcomp(&compiledReg, afl_input_file_regex, REG_EXTENDED);
	if (ret_code != 0) {
		free(buffer);
		log("Failed to compile regular expression pattern.");
		return NULL;
	}

	// process each cmdline argument
	buffer_it = buffer;
	buffer_end = buffer + bytes_read;
	log("Processing %ld bytes.", bytes_read);
	while (buffer_it < buffer_end) {
		ret_code = regexec(&compiledReg, buffer_it, 0, NULL, 0);
		//regex matched
		if (!ret_code) {
			result = strdup(buffer_it);
			if (result) {
				free(buffer);
				regfree(&compiledReg);
				log("Found input filepath: \"%s\".", result);
				return result;
			}
		}
		// skip until next '\0' delimiter
		while (buffer_it < buffer_end && *buffer_it++)
			;
	}
	free(buffer);
	regfree(&compiledReg);
	log("AFL input filepath not found.");
	return NULL;
}

static char *read_data_from_file(size_t *ptr_data_size,
				 const char *bitmap_filepath)
{
	FILE *file_ptr;
	int result = -1;
	char *buffer = NULL;
	struct stat stat_str;
	if (bitmap_filepath == NULL)
		return NULL;

	file_ptr = fopen(bitmap_filepath, "r");
	if (file_ptr == NULL) {
		log("Unable to open \"%s\" file.", bitmap_filepath);
		return NULL;
	}
	result = fstat(fileno(file_ptr), &stat_str);
	if (result != 0) {
		fclose(file_ptr);
		log("Unable to fstat \"%s\" file.", bitmap_filepath);
		return NULL;
	}
	size_t data_size = stat_str.st_size;
	log("Input file size: %ld", data_size);

	buffer = (char *)malloc(data_size + 1);
	if (buffer == NULL) {
		fclose(file_ptr);
		log("Unable to alocate: %ld memory.", data_size + 1);
		return NULL;
	}

	buffer[data_size] = 0;

	size_t data_size_read = fread(buffer, 1, data_size, file_ptr);
	log("Read: %ld bytes from \"%s\" file.", data_size_read,
	    bitmap_filepath);
	if (data_size_read != data_size) {
		free(buffer);
		log("Read: %ld from \"%s\" file but file size is: %ld bytes.",
		    data_size_read, bitmap_filepath, data_size);
		buffer = NULL;
	} else {
		*ptr_data_size = data_size;
	}
	fclose(file_ptr);
	return buffer;
}

static char *read_data_from_stdio(size_t *map_data_size)
{
	size_t stdin_it = SPLIT_STRING_SIZE, buff_size = MAX_LINE_LENGHT;
	char *buff = malloc(MAX_LINE_LENGHT);
	if (!buff) {
		log("Unable to alocate memory for AFL input buffer.");
		return NULL;
	}
	log("Reading bitmap from stdin...");
	if (fread(buff, sizeof(char), SPLIT_STRING_SIZE, stdin) !=
	    sizeof(char) * SPLIT_STRING_SIZE) {
		log("Unable to read first %ld characters from stdin",
		    SPLIT_STRING_SIZE);
		free(buff);
		return NULL;
	}

	while (fread(buff + stdin_it, sizeof(char), 1, stdin)) {
		if (strncmp(SPLIT_STRING, (buff + stdin_it - SPLIT_STRING_SIZE),
			    SPLIT_STRING_SIZE) == 0) {
			log("find spliting_point at: %ld", stdin_it);
			*map_data_size = stdin_it - SPLIT_STRING_SIZE;
			return buff;
		}
		++stdin_it;
		if (stdin_it >= buff_size) {
			if (!(buff = realloc(buff, buff_size * 2))) {
				log("Unable to reallocate memory for AFL input buffer.");
				free(buff);
				return NULL;
			}
		}
	}
	log("Bitmap does not contain magic value:\"%s\".", SPLIT_STRING);
	free(buff);
	return NULL;
}

static enum ESAN_ERROR_CODE_E
write_test_input_data(const char *original_file_path, char *test_data_ptr,
		      size_t test_data_size)
{
	FILE *fp;
	size_t written_data_size;
	// TMP_EXTENSION_SIZE - sizeof(.tmp + '\0')
	size_t tmp_file_path_size =
		(strlen(original_file_path) + TMP_EXTENSION_SIZE) *
		sizeof(char);
	char *tmp_file_path = malloc(tmp_file_path_size);
	if (!tmp_file_path) {
		log("Unable to allocate memory for tmp input file path.");
		return ESAN_LIBRARY_FUNCTION_ERROR;
	}
	snprintf(tmp_file_path, tmp_file_path_size, "%s.tmp",
		 original_file_path);
	fp = fopen(tmp_file_path, "w");
	if (!fp) {
		log("Unable to open: %s", tmp_file_path);
		free(tmp_file_path);
		return ESAN_LIBRARY_FUNCTION_ERROR;
	}
	log("Writting: %ld bytes to: \"%s\" file.", test_data_size,
	    tmp_file_path);
	written_data_size =
		fwrite(test_data_ptr, sizeof(char), test_data_size, fp);
	if (fclose(fp)) {
		log("Unable to write: %ld bytes to: \"%s\" file.",
		    test_data_size, tmp_file_path);
		free(tmp_file_path);
		return ESAN_LIBRARY_FUNCTION_ERROR;
	}
	log("Input file written: %ld bytes.", written_data_size);
	if (written_data_size != test_data_size ||
	    rename(tmp_file_path, original_file_path) != 0) {
		log("Unable to write: %s file or rename file to: %s.",
		    tmp_file_path, original_file_path);
		free(tmp_file_path);
		return ESAN_LIBRARY_FUNCTION_ERROR;
	}
	free(tmp_file_path);
	return ESAN_SUCCESS;
}

static enum ESAN_ERROR_CODE_E find_bitmap_splitting_point(char *bitmap,
							  size_t bitmap_size,
							  char **spliting_point)
{
	size_t bitmap_it = 0;
	char tmp;
	for (; bitmap_it + SPLIT_STRING_SIZE < bitmap_size; ++bitmap_it) {
		tmp = bitmap[bitmap_it + SPLIT_STRING_SIZE];
		bitmap[bitmap_it + SPLIT_STRING_SIZE] = '\0';
		if (strncmp(bitmap + bitmap_it, SPLIT_STRING,
			    bitmap_size - bitmap_it) == 0) {
			bitmap[bitmap_it + SPLIT_STRING_SIZE] = tmp;
			*spliting_point =
				bitmap + bitmap_it + SPLIT_STRING_SIZE;
			return ESAN_SUCCESS;
		}
		bitmap[bitmap_it + SPLIT_STRING_SIZE] = tmp;
	}
	return ESAN_INTERNAL_ERROR;
}

void parse_map(void)
{
	char *bitmap_path, *bitmap_path_to_free = NULL;
	char *after_split_prt = 0;
	enum ESAN_ERROR_CODE_E ret_code;
#if CP_WRONG_MAP
	static int srand_initialized = 0;
#endif
	log("Start map parsing.");
	if (parse_env() != ESAN_SUCCESS) {
		parse_map_cleanup();
		exit_failure(
			ESAN_INTERNAL_ERROR,
			"Unable to read environment variables file from procfs");
	}
	if (esan_getenv("AFL_USE_STDIO=")) {
		// read map from stdio
		esan_error_bitmap =
			read_data_from_stdio(&esan_error_bitmap_size);
	} else {
		// read map + test data from file
		bitmap_path = esan_getenv("AFL_MAP_FILEPATH=");
		if (bitmap_path)
			bitmap_path += strlen("AFL_MAP_FILEPATH=");
		else
			bitmap_path_to_free = bitmap_path =
				find_bitmap_filepath();

		esan_error_bitmap = read_data_from_file(&esan_error_bitmap_size,
							bitmap_path);
		if (!esan_error_bitmap) {
			free(bitmap_path_to_free);
			parse_map_cleanup();
			exit_failure(
				ESAN_INTERNAL_ERROR,
				"Unable to find input file path or read input data.");
		}
		log("Read: %ld bytes from: \"%s\" file.",
		    esan_error_bitmap_size, bitmap_path);
		ret_code = find_bitmap_splitting_point(esan_error_bitmap,
						       esan_error_bitmap_size,
						       &after_split_prt);
		if (ret_code != ESAN_SUCCESS) {
			free(bitmap_path_to_free);
#ifdef CP_WRONG_MAP
			if (srand_initialized == 0) {
				srand(time(NULL));
				srand_initialized = 1;
			}
			char buff[64];
			snprintf(buff, sizeof(buff), "/tmp/esan_debug%d.txt",
				 rand());
			(void)write_file(buff, esan_error_bitmap,
					 esan_error_bitmap_size);
#endif
			parse_map_cleanup();
			exit_failure(
				ret_code,
				"Unable to find input bitmap splitting point.");
		}
		log("buff: %p, split: %p, diff: %ld", esan_error_bitmap,
		    after_split_prt, after_split_prt - esan_error_bitmap);

		// write test data to file
		ret_code = write_test_input_data(
			bitmap_path, after_split_prt,
			esan_error_bitmap_size -
				(after_split_prt - esan_error_bitmap));
		if (ret_code != ESAN_SUCCESS) {
			free(bitmap_path_to_free);
			parse_map_cleanup();
			exit_failure(ret_code, "Unable to write test data.");
		}
		esan_error_bitmap_size =
			after_split_prt - esan_error_bitmap - SPLIT_STRING_SIZE;
		free(bitmap_path_to_free);
	}
	if (esan_error_bitmap == NULL || esan_error_bitmap_size == 0) {
		parse_map_cleanup();
		exit_failure(ESAN_INTERNAL_ERROR, "Input file is empty!");
	}
	log("Input map parsed, size of input data = %zu",
	    esan_error_bitmap_size);
}

void parse_map_cleanup(void)
{
	free(esan_error_bitmap);
	free(env_buffer);
	free(env_array);
	esan_error_bitmap = NULL;
}
