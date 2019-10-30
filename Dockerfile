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
#
FROM ubuntu:18.04

# Update apps on the base image
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get -y update && apt-get -y upgrade

RUN apt-get -y install curl gnupg2

RUN curl -L -- https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -

RUN echo "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-9 main" \
    >> /etc/apt/sources.list
RUN echo "deb-src http://apt.llvm.org/bionic/ llvm-toolchain-bionic-9 main" \
    >> /etc/apt/sources.list

RUN apt-get -y update && apt-get -y upgrade

# install build dependencies
RUN apt-get -y install clang-9 make libssl-dev

# install CI dependencies
RUN apt-get -y install bear clang-format-9 clang-tidy-9 clang-tools-9 python3 libfindbin-libs-perl
RUN ln -s /usr/bin/clang-9 /usr/bin/cc
RUN ln -s /usr/bin/clang++-9 /usr/bin/cxx
RUN ln -s /usr/bin/clang-tidy-9 /usr/bin/clang-tidy
RUN ln -s /usr/bin/clang-format-9 /usr/bin/clang-format
RUN ln -s /usr/bin/scan-build-9 /usr/bin/scan-build
RUN ln -s /usr/bin/clang-apply-replacements-9 /usr/bin/clang-apply-replacements

COPY .clang-format /errorsanitizer/
COPY .clang-format-ignore /errorsanitizer/
COPY ./ci.sh /errorsanitizer/
COPY ./*.c /errorsanitizer/
COPY ./Makefile /errorsanitizer/
COPY ./hooks /errorsanitizer/hooks/
COPY ./include /errorsanitizer/include/
COPY ./tests /errorsanitizer/tests/
COPY ./ci /errorsanitizer/ci/

WORKDIR /errorsanitizer

CMD bash ci.sh #executes during docker run

