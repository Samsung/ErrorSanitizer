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
#ifdef RAND_VERSION

#include "error_sanitizer.h"
#include<time.h>
#include <limits.h>

extern unsigned long esan_total_nr_execs;

#ifndef CHANCE
#error "CHANCE not defined\n"
#endif
#if CHANCE < 0 || CHANCE > 100
#error "Bad CHANCE definition. It should be between 0 and 100\n"
#endif

int esan_should_I_fail(void)
{
    static int initialized=0;
    if (!initialized)
    {
	++initialized;
    	srand(time(0));
    }
    ++esan_total_nr_execs;

    if (esan_total_nr_execs > (ULONG_MAX >> 4))
        return 1;

    return ((rand()%100) < CHANCE);
}
#endif
