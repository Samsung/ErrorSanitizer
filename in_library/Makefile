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
#
ifeq (${ESAN_PATH},)
    $(error Please set ESAN_PATH variable before using this Makefile!)
endif

GIT	:= $(shell which git)
LD	?= $(shell which ld)
OBJCOPY	:= $(shell which objcopy)
STRIP	:= $(shell which strip)
SYSROOT	:= $(shell realpath ./sysroot)

NAME := in_library
LINKED_NAME := in_library_linked.o
LINKED_NAME_HIDDEN := in_library_linked_hidden.o
LINKED_NAME_API := in_library_api.o
CONFIGURE_MUSL_SYNC_FILE := .configure-musl.sync
###
all: $(LINKED_NAME_API)


$(CONFIGURE_MUSL_SYNC_FILE):
	mkdir -p sysroot
	cd musl && CC="$(CC) -fPIC" ./configure --prefix=$(SYSROOT) --disable-shared && \
		touch $(CONFIGURE_MUSL_SYNC_FILE)

build-musl: $(CONFIGURE_MUSL_SYNC_FILE)
	cd musl && make -j$(shell nproc)

install-musl: build-musl
	mkdir -p $(SYSROOT)
	cd musl && make install

$(SYSROOT)/lib/libc.a: install-musl

### musl compilation end

CFLAGS = ${CFLAGS_LOCAL} -I${ESAN_PATH}/include

############################################################################

$(NAME).o: $(NAME).c install-musl
	$(CC) -nostdinc --sysroot=$(SYSROOT) -I$(SYSROOT)/include -fPIC -c ${CFLAGS} $<

$(LINKED_NAME): $(NAME).o
	$(LD) -nostdlib --sysroot=$(SYSROOT) -L$(SYSROOT)/lib -r -o $@ $< -lc

$(LINKED_NAME_HIDDEN): $(LINKED_NAME)
	$(OBJCOPY) --wildcard --localize-symbol='*' $< $@

$(LINKED_NAME_API): $(LINKED_NAME_HIDDEN)
	$(OBJCOPY) --globalize-symbol=in_library \
		--globalize-symbol=in_library_initialize $< $@

###

clean:
	rm -f $(NAME).o $(LINKED_NAME) $(LINKED_NAME_HIDDEN) $(LINKED_NAME_API) \
		$(CONFIGURE_MUSL_SYNC_FILE)
	rm -rf $(SYSROOT)

