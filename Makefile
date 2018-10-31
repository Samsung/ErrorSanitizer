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

CFLAGS = -Wall -Wno-unused-variable -fPIC $(CFLAGS_LOCAL)
LFLAGS = $(LFLAGS_LOCAL)

PRELOAD_SRC     = error_sanitizer_preload.c $(wildcard hooks_*.c)
PRELOAD_OBJ		= $(PRELOAD_SRC:.c=.o)

DEPS            = Makefile $(wildcard *.h)

LIBS = error_sanitizer.so error_sanitizer_preload.so

############################################################################

all: $(LIBS)

error_sanitizer.so:
	$(CC) -fPIC -shared -ldl -o error_sanitizer.so error_sanitizer.c

error_sanitizer_preload.so: error_sanitizer.so $(PRELOAD_OBJ)
	$(CC) -fPIC -shared -ldl -o error_sanitizer_preload.so $(PRELOAD_OBJ)

clean:
	rm -f $(LIBS) $(PRELOAD_OBJ)

############################################################################

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

