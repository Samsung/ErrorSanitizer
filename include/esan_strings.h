/*
    Copyright (c) 2018 - 2020, Samsung Electronics Co., Ltd. All rights reserved.

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

    Author: Ernest Borowski <e.borowski@samsung.com>
    Author: Mateusz Nosek <m.nosek@samsung.com>
*/
#ifndef _ESAN_STRINGS_H
#define _ESAN_STRINGS_H

#include "attributes.h"

maybe_unused static unsigned long esan_const_strlen(const char *str)
{
	const char *cp = str;
	while (*str++ != '\0')
		;
	return str - cp;
}

maybe_unused static int esan_strncmp(const char *s1, const char *s2,
				     unsigned long n)
{
	unsigned long i;
	int tmp;

	for (i = 0; i < n; i++, s1++, s2++) {
		tmp = *s1 - *s2;
		if (tmp != 0)
			return tmp;
		if (*s1 == '\0')
			return 0;
	}

	return 0;
}

maybe_unused static char *esan_strnstr(char *s1, const char *s2,
				       unsigned long length)
{
	unsigned long it, len_s1, len_s2;

	if (s1 == 0 || s2 == 0)
		return 0;

	len_s1 = esan_const_strlen(s1);
	len_s2 = esan_const_strlen(s2);

	for (it = 0; (it + len_s2 < len_s1) && (it + len_s2 < length); ++it) {
		if (0 == esan_strncmp(s1 + it, s2, len_s2))
			return s1 + it;
	}
	return 0;
}

#endif
