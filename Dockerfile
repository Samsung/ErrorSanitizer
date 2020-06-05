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
FROM ubuntu:20.04 as build

# Update apps on the base image
ARG DEBIAN_FRONTEND="noninteractive"

RUN apt-get -y update && \
	apt-get -y upgrade && \
	apt-get -y install curl gnupg2 && \
	curl -L -- https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
	echo "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-9 main" \
		>> /etc/apt/sources.list && \
	echo "deb-src http://apt.llvm.org/bionic/ llvm-toolchain-bionic-9 main" \
		>> /etc/apt/sources.list && \
	apt-get -y update && \
	apt-get -y install clang-9 make && \
	rm -rf /var/lib/apt/lists/*

RUN ln -s /usr/bin/clang-9 /usr/bin/cc

COPY ./*.c /errorsanitizer/
COPY ./Makefile /errorsanitizer/
COPY ./hooks /errorsanitizer/hooks/
COPY ./include /errorsanitizer/include/

WORKDIR /errorsanitizer

ARG CFLAGS="-03"
RUN make

FROM ubuntu:20.04
WORKDIR /errorsanitizer
COPY --from=build /errorsanitizer/error_sanitizer_preload.so /usr/lib/
CMD ["sh"]
