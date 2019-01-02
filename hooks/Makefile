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
# Author: Mateusz Nosek <m.nosek@samsung.com>
#
ifeq (${ESAN_PATH},)
    $(error Please set ESAN_PATH variable before using this Makefile!)
endif

CC ?= clang
INCLUDE_FLAGS ?= -I${ESAN_PATH}/include
CFLAGS = $(CFLAGS_LOCAL) ${INCLUDE_FLAGS} -Wno-unused-parameter -fPIC

SRC     = $(wildcard hooks_*.c) fail_common.c
OBJ		= $(SRC:.c=.o)
############################################################################

all: $(OBJ) hooks.o

fail_common.o: fail_common.c
	$(CC) -c -o $@ $< $(CFLAGS)

hooks_%.o: hooks_%.c 
	$(CC) -c -o $@ $< $(CFLAGS)

hooks.o: $(OBJ)
	ld -r $^ -o $@

clean:
	rm -f $(OBJ) hooks.o

