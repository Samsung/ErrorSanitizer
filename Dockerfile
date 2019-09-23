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

RUN apt-get -y install clang make libssl-dev

COPY ./ci.sh /errorsanitizer/
COPY ./*.c /errorsanitizer/
COPY ./Makefile /errorsanitizer/
COPY ./hooks /errorsanitizer/hooks/
COPY ./include /errorsanitizer/include/
COPY ./tests /errorsanitizer/tests/

WORKDIR /errorsanitizer
CMD bash ci.sh #executes during docker run

