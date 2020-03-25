#!/bin/bash
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

make clean
EXIT_CODE=0
WHILE_EXECUTED=0
while IFS= read -r -d '' f; do
	if [ -f "$f" ]; then # git submodules are listed as files!!
		if [ "$(grep -c '[[:blank:]]$' "$f")" != "0" ]; then
			echo -e "\nFound whitespace in file: $f"
			grep -n --color="ALWAYS" '[[:blank:]]$' "$f"
			EXIT_CODE=1
		fi
	fi
	WHILE_EXECUTED=1
done < <(find . -type f -print0 | \
		grep -E --null-data -v '^\./(\.[^/]|in_library/musl)' |\
		grep -E --null-data '(/Makefile|\.([ch]|map|sh|md))$')

if [ "$WHILE_EXECUTED" != "1" ]; then
	echo "Unable to execute git ls-files"
	exit 100
fi
exit $EXIT_CODE
