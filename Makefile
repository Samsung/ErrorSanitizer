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
# Author: Jakub Botwicz <j.botwicz@samsung.com>
# Author: Mateusz Nosek <m.nosek@samsung.com>
#
ESAN_PATH = $(shell pwd)
TEST_PATH = ${ESAN_PATH}/tests
HOOK_PATH = ${ESAN_PATH}/hooks

FAIL_CHANCE ?= 70;
CC ?= clang

INCLUDE_FLAGS = -I${ESAN_PATH}/include
CFLAGS = $(CFLAGS_LOCAL) ${INCLUDE_FLAGS} -fPIC -Wall -Wextra -Werror
LFLAGS = $(LFLAGS_LOCAL)
CFLAGS_LIB = -fPIC -shared -ldl ${INCLUDE_FLAGS}

PRELOAD_SRC     = error_sanitizer_preload.c in_library.c sanitizer_fail.c
PRELOAD_OBJ		= $(PRELOAD_SRC:.c=.o) ${HOOK_PATH}/hooks.o
PRELOAD_OBJ_RAND 	= $(PRELOAD_SRC:.c=_RAND.o) ${HOOK_PATH}/hooks.o

LIBS = error_sanitizer.so error_sanitizer_preload.so error_sanitizer_RAND.so

############################################################################

all: ev

rebuild: clean
	$(MAKE) all

run: ev
	cd $(TEST_PATH) && ESAN_PATH=$(ESAN_PATH) $(MAKE) tests

ev: hook $(LIBS) test

error_sanitizer.so:
	$(CC) $(CFLAGS_LIB) -o $@ error_sanitizer.c -Wall -Wextra

error_sanitizer_preload.so: error_sanitizer.so $(PRELOAD_OBJ)
	$(CC) $(CFLAGS_LIB) -o $@ $(PRELOAD_OBJ)

error_sanitizer_RAND.so: $(PRELOAD_OBJ_RAND)
	$(CC) $(CFLAGS_LIB) -o $@ $(PRELOAD_OBJ_RAND) -DFAIL_CHANCE=$(FAIL_CHANCE)


clean: hook_clean test_clean
	rm -f $(LIBS) $(PRELOAD_OBJ) $(PRELOAD_OBJ_RAND)

hook: ${HOOK_PATH}/hooks.o

${HOOK_PATH}/hooks.o:
	cd ${HOOK_PATH} && ESAN_PATH=${ESAN_PATH} CFLAGS_LOCAL="${CFLAGS}" CC=${CC} INCLUDE_FLAGS=${INCLUDE_FLAGS} $(MAKE)
hook_clean:
	cd ${HOOK_PATH} && ESAN_PATH=${ESAN_PATH} $(MAKE) clean

test: $(LIBS)
	cd ${TEST_PATH} && ESAN_PATH=${ESAN_PATH} CFLAGS_LOCAL="${CFLAGS}" CC=${CC} INCLUDE_FLAGS=${INCLUDE_FLAGS} $(MAKE)
test_clean:
	cd ${TEST_PATH} && ESAN_PATH=${ESAN_PATH} $(MAKE) clean

############################################################################
%_RAND.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) -DFAIL_CHANCE=$(FAIL_CHANCE)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

