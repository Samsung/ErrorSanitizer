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
	ESAN_PATH := $(shell realpath ./)
endif

TEST_PATH = ${ESAN_PATH}/tests
HOOK_PATH = ${ESAN_PATH}/hooks
LIB_PATH = ${ESAN_PATH}/in_library
SYSROOT = ${LIB_PATH}/sysroot

CC ?= clang
OBJCOPY	:= $(shell which objcopy)

INCLUDE_FLAGS = -I${ESAN_PATH}/include
# Additional defines:
# -DAFL - disable logging:
#    on failure exit
#    on failure injection
#    statistics on exit
#  disable:
#    statistics counting
#
# -DCP_WRONG_MAP - enable copying maps with wrong format to /tmp/esan_debug%d.txt
#  where %d is random int, useful for debugging
#
# -DDEBUG - enable additional debug logging
#
# -DESAN_DISABLE_HOOKS_OPENSSL - disables openssl hooks


# Shared CFLAGS between library and other components
CFLAGS_SHARED = ${INCLUDE_FLAGS} -fPIC -Wall -Wextra -Werror -std=gnu89 -DESAN_DISABLE_HOOKS_OPENSSL
# CFLAGS for other components
CFLAGS = $(CFLAGS_LOCAL) ${CFLAGS_SHARED}
LDFLAGS = $(LDFLAGS_LOCAL)
# CFLAGS for library
CFLAGS_LIB = $(CFLAGS_LIB_LOCAL) ${CFLAGS_SHARED}
LDFLAGS_LIB = $(LDFLAGS_LIB_LOCAL) -shared -ldl

HOOK_OBJ = ${HOOK_PATH}/hooks.o
LIB_OBJ = ${LIB_PATH}/in_library_api.o
CFLAGS_COVERAGE = -fprofile-arcs -ftest-coverage -ggdb
LDFLAGS_COVERAGE = -lgcov --coverage

PRELOAD_SRC     = error_sanitizer_preload.c esan_stats.c esan_fail.c

ESAN_INIT_OBJ := error_sanitizer.o
ESAN_INIT_OBJ_LINKED := error_sanitizer_linked.o
ESAN_INIT_OBJ_HIDDEN := error_sanitizer_hidden.o
ESAN_INIT_OBJ_API := error_sanitizer_api.o

LIBS = error_sanitizer_preload.so
############################################################################

all: ev

rebuild: clean
	$(MAKE) all

run: ev
	cd $(TEST_PATH) && ESAN_PATH=$(ESAN_PATH) CFLAGS_LOCAL="${CFLAGS}" \
		LDFLAGS_LOCAL="${LDFLAGS}" CC=${CC} $(MAKE) tests

ev: hook $(LIBS)

$(ESAN_INIT_OBJ): error_sanitizer.c $(LIB_OBJ)
	$(CC) -nostdinc --sysroot=$(SYSROOT) -I$(SYSROOT)/include -c ${CFLAGS} $<

coverage_compile:
	CC="gcc" CFLAGS_LOCAL="$(CFLAGS_COVERAGE)" CFLAGS_LIB_LOCAL="$(CFLAGS_COVERAGE)" \
		LDFLAGS_LOCAL="$(LDFLAGS_COVERAGE)" LDFLAGS_LIB_LOCAL="$(LDFLAGS_COVERAGE)" \
		$(MAKE) rebuild

coverage:
	$(MAKE) coverage_compile
	$(MAKE) run
	gcovr -r "${ESAN_PATH}" --delete --print-summary
	find -type f -regex '.*\(gcno\|gcda\)$$' -delete
coverage_html:
	$(MAKE) coverage_compile
	$(MAKE) run
	gcovr -r "${ESAN_PATH}" --delete --print-summary --html --html-details -o coverage.html
	find -type f -regex '.*\(gcno\|gcda\)$$' -delete

$(ESAN_INIT_OBJ_LINKED): $(ESAN_INIT_OBJ)
	$(LD) -nostdlib --sysroot=$(SYSROOT) -L$(SYSROOT)/lib -r -o $@ $< -lc

$(ESAN_INIT_OBJ_HIDDEN): $(ESAN_INIT_OBJ_LINKED)
	$(OBJCOPY) --wildcard --localize-symbol='*' $< $@

$(ESAN_INIT_OBJ_API): $(ESAN_INIT_OBJ_HIDDEN)
	$(OBJCOPY) --globalize-symbol=esan_always_succeed \
		--globalize-symbol=esan_error_bitmap \
		--globalize-symbol=esan_error_bitmap_size \
		--globalize-symbol=parse_map \
		--globalize-symbol=parse_map_cleanup $< $@

error_sanitizer_preload.so: $(ESAN_INIT_OBJ_API) $(HOOK_OBJ) $(LIB_OBJ)
	$(CC) $(CFLAGS_LIB) -o $@ $(ESAN_INIT_OBJ_API) $(HOOK_OBJ) ${PRELOAD_SRC} $(LIB_OBJ)  \
		$(LDFLAGS_LIB)

clean: hook_clean test_clean lib_clean
	rm -f $(LIBS) $(HOOK_OBJ) $(ESAN_INIT_OBJ) $(ESAN_INIT_OBJ_LINKED) $(ESAN_INIT_OBJ_HIDDEN) \
		$(ESAN_INIT_OBJ_API)
	find -type f -regex '.*\(gcno\|gcda\|html\)$$' -delete

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
