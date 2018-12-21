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
# Author: Mateusz Nosek <m.nosek@samsung.com>
#
ESAN_PATH = $(shell pwd)

CHANCE ?= 70;
RAND = "RAND_VERSION"
CC ?= clang

CFLAGS = -Wall -Wno-unused-variable -fPIC $(CFLAGS_LOCAL)
LFLAGS = $(LFLAGS_LOCAL)
CFLAGS_LIB = -fPIC -shared -ldl

PRELOAD_SRC     = error_sanitizer_preload.c $(wildcard hooks_*.c) $(wildcard sanitizer_fail_*.c)
PRELOAD_OBJ		= $(PRELOAD_SRC:.c=.o)
PRELOAD_OBJ_RAND 	= $(PRELOAD_SRC:.c=_RAND.o)

DEPS            = Makefile $(wildcard *.h)

LIBS = error_sanitizer.so error_sanitizer_preload.so error_sanitizer_preload_RAND.so

############################################################################

all: $(LIBS)

error_sanitizer.so:
	$(CC) $(CFLAGS_LIB) -o error_sanitizer.so error_sanitizer.c

error_sanitizer_preload.so: error_sanitizer.so $(PRELOAD_OBJ)
	$(CC) $(CFLAGS_LIB) -o error_sanitizer_preload.so $(PRELOAD_OBJ)

error_sanitizer_preload_RAND.so: error_sanitizer.so $(PRELOAD_OBJ_RAND)
	$(CC) $(CFLAGS_LIB) -o error_sanitizer_preload_RAND.so $(PRELOAD_OBJ_RAND) -D$(RAND) -DCHANCE=$(CHANCE)

clean:
	rm -f $(LIBS) $(PRELOAD_OBJ) $(PRELOAD_OBJ_RAND)

############################################################################
%_RAND.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) -D$(RAND) -DCHANCE=$(CHANCE)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

