# Copyright (c) 2018 - 2019, Samsung Electronics Co., Ltd. All rights reserved.
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
    $(error Please set ESAN_PATH variable before using this Makefile!)
endif

CC ?= clang

CFLAGS = ${CFLAGS_LOCAL}
LDFLAGS = ${LDFLAGS_LOCAL} ${ESAN_PATH}/error_sanitizer.so

SRC     = $(wildcard *.c)
OBJ     = $(SRC:.c=.o)
TESTS   = $(wildcard test_*.c)
EXEC    = $(TESTS:.c=.elf)
RESULTS = $(TESTS:.c=.txt)

DEPS    = Makefile

############################################################################

all: $(OBJ) $(EXEC)

clean:
	rm -f $(OBJ) $(EXEC)
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

############################################################################

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test_openssl.elf: test_openssl.o
	$(CC) -o $@ $^ $(LDFLAGS) -lssl -lcrypto

%.elf: %.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.txt: %.elf
	LD_PRELOAD=${ESAN_PATH}/error_sanitizer_preload.so ./$< esan_always_succeed.map
