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
RESULTS = $(TESTS:.c=.txt)

DEPS    = Makefile

############################################################################

all: $(EXEC) $(EXEC_FAIL)

clean:
	rm -f $(EXEC) $(EXEC_FAIL)
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

%.elf: %.c $(DEPS)
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

%.elf.fail: %.c $(DEPS)
# ESAN_FAIL_TEST produces test binaries that should fail, at test point
# e.g. test_fpen should fail at fopen() call
	$(CC) -o $@ $< $(CFLAGS) -DESAN_FAIL_TEST $(LDFLAGS)

test_openssl.elf: test_openssl.o
	$(CC) -o $@ $^ $(LDFLAGS) -lssl -lcrypto

test_empty_map.txt: test_empty_map.elf
	touch "esan_empty.map.$<.cur_input"
	LD_PRELOAD=${ESAN_PATH}/error_sanitizer_preload.so ./$< "esan_empty.map.$<.cur_input" 2>&1 | grep -qE '^error_sanitizer\.c \| parse_map:3(08|11) \| Unable to read input data. bitmap: 0x[0-9a-f]+, size: 0$$'
	rm -f "esan_empty.map.$<.cur_input"

%.txt: %.elf
	cp esan_always_succeed.map "esan_always_succeed.map.$<.cur_input"
	LD_PRELOAD=${ESAN_PATH}/error_sanitizer_preload.so ./$< "esan_always_succeed.map.$<.cur_input"
	rm -f "esan_always_succeed.map.$<.cur_input"

%fail.txt: esan_fail%.map
	cp "esan_fail%.map" "esan_fail%.map.cur_input"
	LD_PRELOAD=${ESAN_PATH}/error_sanitizer_preload.so ./$< "esan_fail%.map.cur_input"
	rm -f "esan_fail%.map.cur_input"
