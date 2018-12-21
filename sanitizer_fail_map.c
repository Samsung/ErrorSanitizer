/*
    Copyright (c) 2018, Samsung Electronics Co., Ltd. All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

    Author: Mateusz Nosek <m.nosek@samsung.com>
*/
#ifndef RAND_VERSION
#include "error_sanitizer.h"

extern unsigned long esan_total_nr_execs;

int esan_should_I_fail(void)
{
    unsigned int index_byte = esan_total_nr_execs / 8;
    unsigned int index_bit = esan_total_nr_execs % 8;
    ++esan_total_nr_execs;

    if (esan_always_succeed)
        return 0;

    if (index_byte >= esan_error_bitmap_size)
        return 1;

    return ((1<<index_bit) & esan_error_bitmap[index_byte]);

}
#endif

