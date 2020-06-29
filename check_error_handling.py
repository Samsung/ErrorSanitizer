# -*- coding: utf-8 -*-
"""Tool for automated testing of applications using Error Sanitizer."""

# Copyright (c) 2020, Samsung Electronics Co., Ltd. All rights reserved.
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

import argparse
import os
from subprocess import Popen, PIPE
import sys

MAIN_LINE_SEPARATOR = "\n" + 80 * "=" + "\n"
"""String used as major separator."""

LINE_SEPARATOR = "\n" + 80 * "-" + "\n"
"""String used as minor separator."""

MAX_FAIL_NR = 9999999
"""Maximum number of failures in ESAN map."""

MAP_SIZE_BYTES = int(MAX_FAIL_NR / 8)
"""Size of ESAN failure map in bytes."""


def create_map_file(map_size, fail_num, data, filename):
    """Create file with generated ESAN map."""
    byte_nr = int(fail_num / 8)
    byte_pos = int(fail_num % 8)
    result = 1 << (byte_pos)
    output = (
        b"\x00" * (byte_nr)
        + result.to_bytes(1, "big")
        + b"\x00" * (map_size - byte_nr)
        + bytes("XXXX", "ascii")
        + data
    )
    with open(filename, "wb") as the_file:
        the_file.write(output)


def run_process_popen_gdb(esan_path, app_path, test_map, before_args="", after_args=""):
    """Run gdb of tested application with given parameters."""
    app_string = (
        "gdb -batch -ex='run' -ex='backtrace' -ex='backtrace full' --args env LD_PRELOAD="
        + esan_path
        + " "
        + app_path
        + " "
        + before_args
        + " "
        + test_map
        + " "
        + after_args
    )
    app_string = app_string.replace("  ", " ").strip()
    print(f"\nReproduction string: {app_string}\n")
    os.system(app_string + " > gdb_dump.txt")
    with open("gdb_dump.txt", "r") as the_file:
        gdb_result = the_file.read()
    os.remove("gdb_dump.txt")
    print(f"gdb output: {LINE_SEPARATOR}{gdb_result}{LINE_SEPARATOR}")
    return (None, None, 0)


def run_process_popen(esan_path, app_path, test_map, before_args="", after_args=""):
    """Run tested application with given parameters."""
    app_string = app_path + " " + before_args + " " + test_map + " " + after_args
    app_string = app_string.replace("  ", " ").strip()
    print(f"Reproduction string: LD_PRELOAD={esan_path} {app_string}\n")

    process = Popen(
        app_string.split(" "),
        bufsize=0,
        stdout=PIPE,
        stderr=PIPE,
        shell=False,
        env={"LD_PRELOAD": esan_path},
    )

    (stdout_pipe, stderr_pipe) = process.communicate()
    exit_code = process.wait()

    if stdout_pipe:
        print(f"stdout:{LINE_SEPARATOR}{stdout_pipe.decode('ascii')}{LINE_SEPARATOR}")
    if stderr_pipe:
        print(f"stderr:{LINE_SEPARATOR}{stderr_pipe.decode('ascii')}{LINE_SEPARATOR}")

    print(f"Exit_code: {exit_code}\n")
    return (stdout_pipe, stderr_pipe, exit_code)


def perform_testing(
    esan_path,
    app_path,
    testfile_path,
    before_args="",
    after_args="",
    start_iteration=0,
    finish_iteration=sys.maxsize,
):
    """Perform testing of application using ESAN."""
    print(f"\nApplication in test: {app_path}\n{MAIN_LINE_SEPARATOR}")

    with open(testfile_path, "rb") as the_file:
        test_payload = the_file.read()

    print("Starting dry run (with always_succeed map):\n")

    create_map_file(MAX_FAIL_NR, MAX_FAIL_NR + 1, test_payload, "temp.cur_input")
    (_, stderr_pipe, exit_code) = run_process_popen(
        esan_path,
        app_path,
        "temp.cur_input",
        after_args=after_args,
        before_args=before_args,
    )

    output_str = str(stderr_pipe)
    total_index = output_str.rfind("TOTAL")
    if total_index == -1:
        print("Could not find ESAN stats!")
        sys.exit(-1)
    total_str = output_str[total_index:]
    total_value = int(total_str.split("|")[1].strip())
    print(f"Total numbers of executions to test: {total_value}\n{MAIN_LINE_SEPARATOR}")

    for i in range(start_iteration, min(total_value, finish_iteration)):
        print(f"{MAIN_LINE_SEPARATOR}\nStarting run nr {i}:\n")
        test_map = "temp_" + str(i) + ".cur_input"
        create_map_file(10 * total_value, i, test_payload, test_map)
        (_, stderr_pipe, exit_code) = run_process_popen(
            esan_path,
            app_path,
            test_map,
            after_args=after_args,
            before_args=before_args,
        )
        if exit_code != 0:
            print(
                f"Tested application failed with map nr {i} - exit code: {exit_code}."
            )
            (_, stderr_pipe, exit_code) = run_process_popen_gdb(
                esan_path,
                app_path,
                test_map,
                after_args=after_args,
                before_args=before_args,
            )


def main(args):
    """Get input parameters and initiate testing."""
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter,
        description="Tool for checking error handling using the Error Sanitizer.\n\n"
        + "At the beginning the tool verifies number of executions of hooked "
        + "functions using the always_succeed map.\nAfterwards it checks single "
        + "failure maps starting from the first occurence to the last one.\n"
        + "In case of a crash, the application is executed again using gdb, "
        + "to receive full stack dump before the crash.\n\n"
        + "For example, if you want to check execution of the following application: "
        + "'diff -a -d test_000.xml test_001.xml -l'\n"
        + "You should use: python check_error_handling.py $(ESAN_PATH)/error_sanitizer_preload.so "
        + "diff test_000.xml -B '-a -d' -A 'test_001.xml -l'",
    )
    parser.add_argument(
        "esan_path",
        metavar="ESAN_PATH",
        type=str,
        help="Full path to ESAN executable (error_sanitizer_preload.so)",
    )
    parser.add_argument(
        "app_path", metavar="APP_PATH", type=str, help="Path to tested executable"
    )
    parser.add_argument(
        "test_file",
        metavar="TESTFILE_PATH",
        type=str,
        help="Path to main test file (file that will be used for provided failure map)",
    )
    parser.add_argument(
        "--before_args",
        "-B",
        type=str,
        default="",
        help="Parameters for tested executable provided BEFORE main test file",
    )
    parser.add_argument(
        "--after_args",
        "-A",
        type=str,
        default="",
        help="Parameters for tested executable provided AFTER main test file",
    )
    parser.add_argument(
        "--start",
        "-S",
        metavar="START_NR",
        type=int,
        default=0,
        help="Start value for failure map iteration (nr of hook that will fail) - default: 0",
    )
    parser.add_argument(
        "--final",
        "-F",
        metavar="FINAL_NR",
        type=int,
        default=MAX_FAIL_NR,
        help=f"Final value for failure map iteration (see: --start) - default: {MAX_FAIL_NR}",
    )

    args = parser.parse_args()
    perform_testing(
        args.esan_path,
        args.app_path,
        args.test_file,
        args.before_args,
        args.after_args,
        args.start,
        args.final,
    )


if __name__ == "__main__":
    main(sys.argv[1:])
