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
UNIT_TEST_PATH = ${ESAN_PATH}/unit_tests
HOOK_PATH = ${ESAN_PATH}/hooks
LIB_PATH = ${ESAN_PATH}/in_library
SYSROOT = ${LIB_PATH}/sysroot

CC ?= clang

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


CFLAGS = -I${ESAN_PATH}/include
CFLAGS += -fPIC -Wall -Wextra -Werror -std=gnu89 -DESAN_DISABLE_HOOKS_OPENSSL
CFLAGS += $(CFLAGS_LOCAL)
LDFLAGS = $(LDFLAGS_LOCAL)

HOOK_OBJ = ${HOOK_PATH}/hooks.o
CFLAGS_COVERAGE = -fprofile-arcs -ftest-coverage -ggdb
LDFLAGS_COVERAGE = -lgcov --coverage

PRELOAD_SRC = error_sanitizer_preload.c esan_stats.c esan_fail.c error_sanitizer.c in_library.c \
			esan_mutex.c esan_signal.c

LIB = error_sanitizer_preload.so
############################################################################

all: ev

rebuild: clean
	$(MAKE) all

run: ev
	cd $(TEST_PATH) && ESAN_PATH=$(ESAN_PATH) CFLAGS_LOCAL="${CFLAGS}" \
		LDFLAGS_LOCAL="${LDFLAGS}" CC=${CC} $(MAKE) tests
	cd $(UNIT_TEST_PATH) && ESAN_PATH=$(ESAN_PATH) CFLAGS_LOCAL="${CFLAGS}" \
		LDFLAGS_LOCAL="${LDFLAGS}" CC=${CC} $(MAKE)

ev: hook $(LIB)

coverage_compile:
	CC="gcc" CFLAGS_LOCAL="$(CFLAGS_COVERAGE) $(CFLAGS_LOCAL)" \
		LDFLAGS_LOCAL="$(LDFLAGS_COVERAGE) $(LDFLAGS_LOCAL)" \
		$(MAKE) rebuild

coverage:
	CFLAGS_LOCAL="$(CFLAGS_LOCAL)" LDFLAGS_LOCAL="$(LDFLAGS_LOCAL)" $(MAKE) coverage_compile
	CFLAGS_LOCAL="$(CFLAGS_LOCAL)" LDFLAGS_LOCAL="$(LDFLAGS_LOCAL)" $(MAKE) run
	gcovr -r "${ESAN_PATH}" --delete --print-summary
	find -type f -regex '.*\(gcno\|gcda\)$$' -delete
coverage_html:
	CFLAGS_LOCAL="$(CFLAGS_LOCAL)" LDFLAGS_LOCAL="$(LDFLAGS_LOCAL)" $(MAKE) coverage_compile
	CFLAGS_LOCAL="$(CFLAGS_LOCAL)" LDFLAGS_LOCAL="$(LDFLAGS_LOCAL)" $(MAKE) run
	gcovr -r "${ESAN_PATH}" --delete --print-summary --html --html-details -o coverage.html
	find -type f -regex '.*\(gcno\|gcda\)$$' -delete

error_sanitizer_preload.so: $(HOOK_OBJ) ${PRELOAD_SRC}
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -shared -ldl

clean: hook_clean test_clean unit_test_clean
	rm -f $(LIB)
	find -type f -regex '.*\(gcno\|gcda\|html\)$$' -delete

hook: ${HOOK_PATH}/hooks.o

${HOOK_OBJ}: ${HOOK_PATH}
	cd ${HOOK_PATH} && ESAN_PATH=${ESAN_PATH} CFLAGS_LOCAL="${CFLAGS}" \
		LDFLAGS_LOCAL="${LDFLAGS}" CC=${CC} $(MAKE)

hook_clean:
	cd ${HOOK_PATH} && ESAN_PATH=${ESAN_PATH} $(MAKE) clean

test: $(LIBS)
	cd ${TEST_PATH} && ESAN_PATH=${ESAN_PATH} CFLAGS_LOCAL="${CFLAGS}" \
		LDFLAGS_LOCAL="${LDFLAGS}" CC=${CC} $(MAKE)
test_clean:
	cd ${TEST_PATH} && ESAN_PATH=${ESAN_PATH} $(MAKE) clean

unit_test_clean:
	cd ${UNIT_TEST_PATH} && ESAN_PATH=${ESAN_PATH} $(MAKE) clean
