#!/usr/bin/env python3
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
import argparse
import difflib
import os.path
import re
import subprocess
import sys

def return_surname_if_present(name_and_surname):
    m = name_and_surname.split()
    if len(m) > 1:
        return m[1]
    return m[0]

def generate_license(authors, commit_year, first_commit_year, license_type):
    license = []
    if license_type == 'hash':
        lyl = 0 # license copyright year lane
        prefix = '# Author: '
        with open('./ci/license_hash_style.txt') as f:
            license = f.readlines()
    else:
        lyl = 1
        prefix = '    Author: '
        with open('./ci/license_c_style.txt') as f:
            license = f.readlines()
        del license[-1] # remove license ending

    if commit_year == first_commit_year:
        license[lyl] = license[lyl][:16 + 2*lyl] + commit_year + license[lyl][20 + 2*lyl:]
    else:
        license[lyl] = license[lyl][:16 + 2*lyl] + first_commit_year + ' - ' + commit_year + license[lyl][20 + 2*lyl:]

    if license_type == 'hash':
        license.append('#\n')
    else:
        license.append('\n')

    sorted_authors = sorted(sorted(authors), key=return_surname_if_present)
    for author in sorted_authors:
        license.append(prefix + author + '\n')

    if license_type == 'hash':
        license.append('#\n')
    else:
        license.append('*/\n')

    return license

def print_diff(diff, colorize_output):
    if not colorize_output:
        sys.stdout.writelines(diff)
        return

    for line in diff:
        if line.startswith('+ '):
            sys.stdout.writelines("\u001b[32m" + line + "\u001b[0m")
        elif line.startswith('- '):
            sys.stdout.writelines("\u001b[31m" + line + "\u001b[0m")
        elif line.startswith('? '):
            sys.stdout.writelines("\u001b[34m" + line + "\u001b[0m")
        else:
            sys.stdout.writelines(line)


def process_files(repair_files, check_all_files, display_diff, colorize_output):
    if check_all_files:
        process = subprocess.run(['git', 'ls-files', '-z'], stdout=subprocess.PIPE,
                                universal_newlines=True)
    else:
        process = subprocess.run(['git', 'diff-tree', '--no-commit-id', '--name-only',
                                 '-r', 'HEAD', '-z'], stdout=subprocess.PIPE,
                                 universal_newlines=True)
    wrong_license_files = []
    wrong_extensions = ['.map', '.txt', '.gitignore', '.clang-format', '.clang-format-ignore',
                       '.gitmodules', '.md']
    tmp_files = set()
    for file in process.stdout.split('\0'):
        authors = set()
        wrong_ext = False
        for we in wrong_extensions:
            if file.endswith(we):
                wrong_ext = True
        if (not os.path.isfile(file)) or wrong_ext:
            continue
        process = subprocess.run(['git', 'log', '--format=%H Author: %aN <%aE>', '--follow', '--', file],
                                 stdout=subprocess.PIPE, universal_newlines=True)
        author_lines = process.stdout.split('\n')
        for author_line in author_lines:
            commit_hash = author_line.split(' ')[0]
            author_line = author_line.lstrip(commit_hash + ' ')
            if author_line.startswith('Author: '):
                author = author_line.lstrip('Author: ')
                process = subprocess.run(['git', 'diff-tree', '--no-commit-id', '--name-only',
                                          '-r', '--exit-code', commit_hash, '--', file],
                                         stdout=subprocess.PIPE, universal_newlines=True)
                # git returns non zero exit code if file was changed in this commit
                if process.returncode != 0 and not '<not.committed.yet>' in author:
                    authors.add(author)
        # find commit date
        process = subprocess.run(['git', 'show', '-s', '--format=%ai', 'HEAD'], stdout=subprocess.PIPE,
                                universal_newlines=True)
        commit_year = process.stdout[:4]
        process = subprocess.run(['git', 'rev-list', '--max-parents=0', 'HEAD'],
                                 stdout=subprocess.PIPE, universal_newlines=True)
        first_commit_id = process.stdout.split('\n')[0]
        process = subprocess.run(['git', 'show', '-s', '--format=%ai', first_commit_id],
                                 stdout=subprocess.PIPE, universal_newlines=True)
        first_commit_year = process.stdout[:4]

        if file.endswith('.c') or file.endswith('.h'):
            license_type = 'c_style'
        else:
            license_type = 'hash'
        license = generate_license(authors, commit_year, first_commit_year, license_type)

        with open(file) as f:
            lines = f.readlines()
            shebang = ''
            if file.endswith('.sh') or file.endswith('.py') and lines[0].startswith('#!'): #files with shebang
                shebang = lines[0]
                lines = lines[1:]
            license_len = len(license)
            file_len = len(lines)
            if license_len > file_len:
                print("license length missmatch, want >= {}, got {}".format(license_len, file_len))
            COPYRIGHT_LINE_REG=r'^[#| ]*Copyright \\(c\\) [0-9]{4}.*All rights reserved\.'
            comp = re.compile(COPYRIGHT_LINE_REG)
            copyright_pos = 0
            if license_type == 'c_style':
                copyright_pos += 1

            if comp.match(lines[copyright_pos]):
                print("file without license: {}".format(file))
                wrong_license_files.append(file)
            else:
                for it in range(0, license_len):
                    if lines[it] != license[it]:
                        wrong_license_files.append(file)
                        break
            if file in wrong_license_files:
                end_point = 0
                if license_type == 'c_style':
                    if lines[0].startswith('/*'):
                        for it in range(0, len(lines)):
                            if lines[it].endswith('*/\n'):
                                end_point = it + 1
                                break
                else:
                    for it in range(0, len(lines)):
                        if not (lines[it].isspace() or lines[it].startswith('#')):
                            end_point = it
                            break
                tmp_output = file + '.tmp'
                with open(tmp_output, 'w') as fw:
                    if shebang != '':
                        fw.write(shebang)
                    for line in license:
                        fw.write(line)
                    for it in range(end_point, len(lines)):
                        fw.write(lines[it])
                    tmp_files.add(tmp_output)
                    diff = list(difflib.context_diff(lines[:end_point], license,
                                                fromfile=file, tofile=tmp_output))
                    # do not count files with more authors as file with wrong license
                    # it may have more authors because file could be e.g. renamed, merged etc
                    only_more_authors = True
                    CHANGED_LINE_RE=r'^[-+?] .*'
                    AUTHOR_LINE_RE=r'- #?\s+Author: '
                    comp_changed = re.compile(CHANGED_LINE_RE)
                    comp_author = re.compile(AUTHOR_LINE_RE)
                    for line in diff:
                        if comp_changed.match(line) and (not comp_author.match(line)):
                            only_more_authors = False
                            break
                    if only_more_authors:
                        wrong_license_files.remove(file)
                    else:
                        if display_diff:
                            print_diff(diff, colorize_output)
                        if repair_files:
                            os.rename(tmp_output, file)

    got_error = False
    for wlf in wrong_license_files:
        print("File with wrong license: {}".format(wlf))
        got_error = True
    for tf in tmp_files:
        os.remove(tf)
    if got_error:
        exit(1)

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--inplace", help="Replace wrong files in place.", action='store_true')
    parser.add_argument("-d", "--diff", help="Display diff", action='store_true')
    parser.add_argument("-a", "--all",
                       help="Check all files known to git instead of only those present in last commit",
                       action='store_true')
    parser.add_argument("-c", "--color", help="Colorize output", action='store_true')
    args = parser.parse_args()
    script_dir = os.path.realpath(__file__)
    script_dir = os.path.dirname(script_dir)
    os.chdir(script_dir + '/../')

    process_files(args.inplace, args.all, args.diff, args.color)

parse_args()
