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
ESAN_PATH = $(shell pwd)
TEST_PATH = ${ESAN_PATH}/tests
HOOK_PATH = ${ESAN_PATH}/hooks
LIB_PATH = ${ESAN_PATH}/in_library

FAIL_CHANCE ?= 70
CC ?= clang

INCLUDE_FLAGS = -I${ESAN_PATH}/include
# Shared CFLAGS between library and other components
CFLAGS_SHARED = ${INCLUDE_FLAGS} -fPIC -Wall -Wextra -Werror -std=gnu89
# CFLAGS for other components
CFLAGS = $(CFLAGS_LOCAL) ${CFLAGS_SHARED}
LDFLAGS = $(LDFLAGS_LOCAL)
# CFLAGS for library
CFLAGS_LIB = $(CFLAGS_LIB_LOCAL) ${CFLAGS_SHARED}
LDFLAGS_LIB = $(LDFLAGS_LIB_LOCAL) -shared -ldl

HOOK_OBJ = ${HOOK_PATH}/hooks.o
LIB_OBJ = ${LIB_PATH}/in_library_api.o

PRELOAD_SRC     = error_sanitizer_preload.c sanitizer_fail.c

PRELOAD_OBJ_ALL = ${HOOK_OBJ} ${LIB_OBJ}
PRELOAD_OBJ		= $(PRELOAD_SRC:.c=.o) ${PRELOAD_OBJ_ALL}

LIBS = error_sanitizer.so error_sanitizer_preload.so

############################################################################

all: ev

rebuild: clean
	$(MAKE) all

run: ev
	cd $(TEST_PATH) && ESAN_PATH=$(ESAN_PATH) $(MAKE) tests

ev: hook $(LIBS) test

error_sanitizer.so: error_sanitizer.c
	$(CC) $(CFLAGS_LIB) -o $@ $^ $(LDFLAGS_LIB)

error_sanitizer_preload.so: error_sanitizer.so $(PRELOAD_OBJ)
	$(CC) $(CFLAGS_LIB) -o $@ $(PRELOAD_OBJ) $(LDFLAGS_LIB)

clean: hook_clean test_clean lib_clean
	rm -f $(LIBS) $(PRELOAD_OBJ)

hook: ${HOOK_PATH}/hooks.o

${HOOK_OBJ}: ${HOOK_PATH}
	cd ${HOOK_PATH} && ESAN_PATH=${ESAN_PATH} CFLAGS_LOCAL="${CFLAGS}" \
		LDFLAGS_LOCAL="${LDFLAGS}" CC=${CC} $(MAKE)

${LIB_OBJ}: ${LIB_PATH}
	cd ${LIB_PATH} && ESAN_PATH=${ESAN_PATH} CFLAGS_LOCAL="${CFLAGS}" \
		LDFLAGS_LOCAL="${LDFLAGS}" CC=${CC} $(MAKE)

hook_clean:
	cd ${HOOK_PATH} && ESAN_PATH=${ESAN_PATH} $(MAKE) clean

lib_clean: ${LIB_PATH}
	cd ${LIB_PATH} && ESAN_PATH=${ESAN_PATH} $(MAKE) clean

test: $(LIBS)
	cd ${TEST_PATH} && ESAN_PATH=${ESAN_PATH} CFLAGS_LOCAL="${CFLAGS}" \
		LDFLAGS_LOCAL="${LDFLAGS}" CC=${CC} $(MAKE)
test_clean:
	cd ${TEST_PATH} && ESAN_PATH=${ESAN_PATH} $(MAKE) clean

############################################################################
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
