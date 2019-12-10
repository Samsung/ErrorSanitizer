#!/bin/python3
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
# Author: Jakub Botwicz <j.botwicz@samsung.com>
# Author: Mateusz Nosek <m.nosek@samsung.com>
#
import sys
import os

def generate_hook(func_definition, func_name, func_return_value, func_full_params, func_name_params) :
    args = {'func_definition':func_definition, 'func_name':func_name, 'func_return_value':func_return_value,
        'func_full_params':func_full_params, 'func_name_params':func_name_params, 'func_name_upper':func_name.upper() }
    hook = '''/* PLEASE remember to add "{func_name}" to enum ESAN_FUNCTION_NAMES in error_sanitizer_preload.c and ESAN_{func_name_upper} to enum ESAN_FUNCTIONS in error_sanitizer.h in the SAME position! */

#include "hooks_include.h"

/* {func_definition} */

typedef {func_return_value}(*{func_name}_func_t) {func_full_params};

{func_return_value} esan_on_fail_{func_name}{func_full_params}
{{
    /* TODO implement below action performed on failure of hooked function */
    return ({func_return_value})NULL;
}}

{func_return_value} real_{func_name}{func_full_params}
{{
    static {func_name}_func_t {func_name}_func_ptr = NULL;
    if (NULL == {func_name}_func_ptr)
        {func_name}_func_ptr = ({func_name}_func_t)dlsym(RTLD_NEXT, "{func_name}");
    if (NULL != {func_name}_func_ptr)
        return (*{func_name}_func_ptr){func_name_params};

    ESAN_ERROR("Error in dlsym - in '{func_name}' wrapper\\n");
    exit(-1);
}}

{func_return_value} {func_name}{func_full_params}
{{
    ESAN_DEBUG("%s %s:%d\\n", __FILE__, __FUNCTION__, __LINE__);
    obj_stats[ESAN_{func_name_upper}].esan_nr_executions += 1;
    if (esan_should_I_fail()) {{
		fail_common();
        esan_fail_message("{func_name}");
        obj_stats[ESAN_{func_name_upper}].esan_nr_failed_executions += 1;
        return esan_on_fail_{func_name}{func_name_params};
    }} else {{
        return real_{func_name}{func_name_params};
    }}
}}

/**************************************************************************************/

'''.format(**args)
    return hook

def process_definition(definition):
    my_def = definition.rstrip(";")
    my_def = my_def.strip()
    def_split = my_def.split("(", 1)
    head_def = def_split[0]
    tail_def = def_split[1]
    head_split = head_def.rsplit(" ", 1)
    func_name = head_split[1].strip()
    func_return_value = head_split[0].strip()
    if "*" in func_name:
        func_name = func_name.lstrip("*")
        func_return_value += "*"
    print("function definition = " + definition.strip())
    print("function name = " + func_name)
    print("function return value = " + func_return_value)
    func_full_params = "(" + tail_def
    print("function full params = " + func_full_params)
    params = tail_def.rstrip(")")
    params = params.strip()
    params_split = params.split(",")
    short_param = "("
    for param in params_split:
        param = param.strip()
        param_split = param.rsplit()
        short_param += param_split[-1].lstrip("*") + ", "
    short_param = short_param.rstrip(", ")
    short_param += ")"
    print("function param names = " + short_param)
    if (short_param == "(void)"):
        short_param = "()"

    return generate_hook(definition.strip(), func_name, func_return_value, func_full_params, short_param)


if (len(sys.argv) != 3):
    print("Wrong number of input params\nUsage: %s input_file output_file" %sys.argv[0])
    sys.exit()

output_file = open(sys.argv[2], 'w')

with open(sys.argv[1], 'r') as input_file:
    line = input_file.readline()
    while line:
        line = line.strip()
        print(line)
        if line.startswith("#") or (len(line) == 0):
            line = input_file.readline()
            continue
        hook = process_definition(line)
        output_file.write(hook)
        line = input_file.readline()
        print("")

input_file.close()
output_file.close()

