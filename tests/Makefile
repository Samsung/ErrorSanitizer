# Copyright (c) 2018 - 2020, Samsung Electronics Co., Ltd. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
#
# Author: Ernest Borowski <e.borowski@samsung.com>
# Author: Jakub Botwicz <j.botwicz@samsung.com>
# Author: Mateusz Nosek <m.nosek@samsung.com>
#
ifeq (${ESAN_PATH},)
	ESAN_PATH := $(shell realpath ./../)
endif

CC ?= clang

CFLAGS = ${CFLAGS_LOCAL}
LDFLAGS = ${LDFLAGS_LOCAL}

SRC     = $(wildcard *.c)
TESTS   = $(wildcard test_*.c)
EXEC    = $(TESTS:.c=.elf)
EXEC_FAIL = $(TESTS:.c=.elf.fail)
EXEC_LINKED = $(TESTS:.c=.elf.linked)
RESULTS = $(TESTS:.c=.txt)
RESULTS += test_large_map.txt
RESULTS += test_access.fail.txt test_calloc.fail.txt test_chmod.fail.txt test_faccessat.fail.txt
RESULTS += test_fchmod.fail.txt test_fclose.fail.txt test_fopen.fail.txt test_fputs.fail.txt
RESULTS += test_ftell.fail.txt test_ftello.fail.txt test_malloc.fail.txt test_realloc.fail.txt
RESULTS += test_rewind.fail.txt test_strdup.fail.txt test_strndup.fail.txt

############################################################################

all: $(EXEC) $(EXEC_FAIL) $(EXEC_LINKED)

clean:
	rm -f $(EXEC) $(EXEC_FAIL) $(EXEC_LINKED)
	$(MAKE) clean_test_files

tests: $(RESULTS)
	$(MAKE) clean_test_files

clean_test_files:
	rm -f "fopen.test"
	rm -f "fclose.test"
	rm -f "access.test"
	rm -f "faccessat.test"
	rm -f "fputs.test"
	rm -f "ftell.test"
	rm -f "chmod.test"
	rm -f "fchmod.test"
	rm -f *.cur_input

############################################################################

%.elf: %.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)
%.elf.linked: %.c ${ESAN_PATH}/error_sanitizer_preload.so
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

%.elf.fail: %.c
# ESAN_FAIL_TEST produces test binaries that should fail, at test point
# e.g. test_fpen should fail at fopen() call
	$(CC) -o $@ $< $(CFLAGS) -DESAN_FAIL_TEST $(LDFLAGS)

test_openssl.elf: test_openssl.o
	$(CC) -o $@ $^ $(LDFLAGS) -lssl -lcrypto

test_large_map.txt: test_ftell.elf
	head -c 500k /dev/zero > 500k.cur_input
	printf 'XXXX\n' >> 500k.cur_input
	AFL_USE_STDIO=Y LD_PRELOAD=${ESAN_PATH}/error_sanitizer_preload.so \
		./$< < 500k.cur_input
	rm -f 500k.cur_input

%.txt: %.elf
	cp esan_always_succeed.map "esan_always_succeed.map.$<.cur_input"
	LD_PRELOAD=${ESAN_PATH}/error_sanitizer_preload.so ./$< "esan_always_succeed.map.$<.cur_input"
	rm -f "esan_always_succeed.map.$<.cur_input"

test_rewind.fail.txt: esan_fail.test_rewind.map test_rewind.elf.fail
	cp "$<" "$<.cur_input"
	LD_PRELOAD=${ESAN_PATH}/error_sanitizer_preload.so ./$(word 2,$^) "$<.cur_input" 2>&1 \
	| grep -vE "^hooks_file\.c | rewind:177 | rewind is obsolete function - see SEI CERT C Coding Standard MSC24-C for detailsgrep" - \
	|grep -vE '^(in_library|error_sanitizer)\.c' - | diff test_rewind_neg_results.txt -
	rm -f "$<.cur_input"

%.fail.txt: esan_fail.%.map %.elf.fail
	cp "$<" "$<.cur_input"
	LD_PRELOAD=${ESAN_PATH}/error_sanitizer_preload.so ./$(word 2,$^) "$<.cur_input"
	rm -f "$<.cur_input"
