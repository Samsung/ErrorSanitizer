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

    Author: Mateusz Nosek <m.nosek@samsung.com>
*/
#ifndef ESAN_SIGNAL_H
#define ESAN_SIGNAL_H
#ifdef ESAN_NO_SIGNALS
/*
	Static inline versions of functions are to make sure no trace
	or performance impact is introduced while not using signals.
*/
static inline void esan_signal_initialize(void)
{
}
static inline void esan_signal_destructor(void)
{
}
#else
void esan_signal_initialize(void);
void esan_signal_destructor(void);
#endif /* ESAN_NO_SIGNALS */
#endif /* ESAN_SIGNAL_H */
