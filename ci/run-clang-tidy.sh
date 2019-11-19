#!/bin/bash
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
set -o errexit
trap 'echo "Aborting due to errexit on line $LINENO. Exit code: $?" >&2' ERR
set -o errtrace
set -e -o pipefail

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  SCRIPT_DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$SCRIPT_DIR/$SOURCE"
  # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
SCRIPT_DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"

cd "$SCRIPT_DIR/../" #go to main repository folder

make # compile musl, it is needed because we don`t want to execute analysis for musl
make clean
bear make

if [ ! -f ci/run-clang-tidy.py ]; then
	curl -Sl "https://raw.githubusercontent.com/llvm-mirror/clang-tools-extra/release_90/clang-tidy/tool/run-clang-tidy.py" > \
		ci/run-clang-tidy.py
fi

python ci/run-clang-tidy.py -clang-tidy-binary /usr/bin/clang-tidy \
	-clang-apply-replacements-binary /usr/bin/clang-apply-replacements -quiet \
	-checks="-,abseil*,bugprone*,cert*,clang-analyzer*,cppcoreguidelines*,hicpp*,llvm*,misc*,modernize*,performance*,readability*,-hicpp-braces-around-statements,-readability-braces-around-statements,-readability-else-after-return,-readability-isolate-declaration" \
	| tee output.txt

if [ "$(grep -cE ".*[0-9]+:[0-9]+: (warning|error).*\[.*\]$" output.txt)" != "0" ]; then
	echo "clang-tidy found errors or warnings" 1>&2
	exit 1
fi
