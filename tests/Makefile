# Copyright (c) 2018, Samsung Electronics Co., Ltd. All rights reserved.
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
# Author: Jakub Botwicz <j.botwicz@samsung.com>
#
ifeq (${ESAN_PATH},)
    $(error Please set ESAN_PATH variable before using this Makefile!)
endif

CC = clang

CFLAGS_LOCAL = -I${ESAN_PATH}
LFLAGS_LOCAL = ${ESAN_PATH}/error_sanitizer.so

CFLAGS  = -fPIC -Wall -Wno-unused-variable $(CFLAGS_LOCAL)
LFLAGS  = $(LFLAGS_LOCAL)

SRC     = $(wildcard *.c)
OBJ     = $(SRC:.c=.o)
TESTS   = $(wildcard test_*.c)
EXEC    = $(TESTS:.c=.elf)
RESULTS = $(TESTS:.c=.txt)

DEPS    = Makefile $(wildcard *.h)

############################################################################

all: $(OBJ) $(EXEC)

clean:
	rm -f $(OBJ) $(EXEC)

tests: $(RESULTS)

#init-fuzz:
#    afl-fuzz 

############################################################################

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test_openssl.elf:
	clang -o test_openssl.elf esan_wrapper.o test_openssl.o /home/j.botwicz/src/errorsanitizer/error_sanitizer.so -lssl -lcrypto

%.elf: %.o $(OBJ) ${ESAN_PATH}/error_sanitizer.so
	$(CC) -o $@ esan_wrapper.o $< $(LFLAGS)

%.txt: $(EXEC)
	LD_PRELOAD=${ESAN_PATH}/error_sanitizer_preload.so ./$< esan_always_succeed.map
