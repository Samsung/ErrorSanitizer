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
FROM ubuntu:20.04

# Update apps on the base image
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get -y update && \
	apt-get -y upgrade && \
	apt-get -y install curl gnupg2 && \
	curl -L -- https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
	echo "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-9 main" \
		>> /etc/apt/sources.list && \
	echo "deb-src http://apt.llvm.org/bionic/ llvm-toolchain-bionic-9 main" \
		>> /etc/apt/sources.list && \
	apt-get -y update && \
	apt-get -y install clang-9 make libssl-dev bear clang-format-9 clang-tidy-9 \
		clang-tools-9 git python3 libfindbin-libs-perl bash gcovr && \
	rm -rf /var/lib/apt/lists/*

RUN rm /usr/bin/cc && \
	ln -s /usr/bin/clang-9 /usr/bin/cc && \
	ln -s /usr/bin/clang++-9 /usr/bin/cxx && \
	ln -s /usr/bin/clang-tidy-9 /usr/bin/clang-tidy && \
	ln -s /usr/bin/clang-format-9 /usr/bin/clang-format && \
	ln -s /usr/bin/scan-build-9 /usr/bin/scan-build && \
	ln -s /usr/bin/clang-apply-replacements-9 /usr/bin/clang-apply-replacements

COPY . /errorsanitizer/

WORKDIR /errorsanitizer

RUN make

CMD bash ci.sh #executes during docker run

