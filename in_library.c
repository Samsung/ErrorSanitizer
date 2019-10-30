/*
    Copyright (c) 2018 - 2019, Samsung Electronics Co., Ltd. All rights reserved.

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
#define EOF 0x4
#define MAX_LIBS 100ul
#define BUFFER_SIZE 100

struct range {
	unsigned long min, max;
};
static struct range libraries[MAX_LIBS];

void esan_write_syscall(const char *c)
{
	(void)c;
}
void esan_exit_syscall(void)
{
}

static void esan_maps_parse_error(void)
{
}

static const char *path = "/proc/self/maps";
int maps_open(void);
long esan_read(int fd, void *buf, unsigned long size);
int esan_close(int fd);
void initialize(void);

static int esan_fd = -1;
static char current = 0;
static int esan_is_space(unsigned char c)
{
	return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' ||
	       c == '\v';
}
static int esan_is_hex_digit(unsigned char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}
static int esan_is_alpha(unsigned char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
static int esan_is_dac_char(unsigned char c)
{
	return c == 'r' || c == 'w' || c == 'x' || c == 'p';
}
static char esan_get_char()
{
	return current;
} // TODO maybe unsigned char
void esan_next_char();

static void esan_skip_whitespaces(void)
{
	while (esan_is_space(esan_get_char()))
		esan_next_char();
}

#define CONST_TEN 10 /* remove warnings */
static unsigned long esan_parse_hex_number(void)
{
	unsigned long ret = 0;
	unsigned char c;

	while (esan_is_hex_digit(c = esan_get_char())) {
		if (c >= '0' && c <= '9')
			c = c - '0';
		else
			c = c - 'a' + CONST_TEN;
		ret = (ret << 4U) + c; // TODO check overflow
		esan_next_char();
	}

	return ret;
}

static void esan_skip_dac(void)
{
	unsigned i;
	for (i = 0; i < 4; ++i) {
		if (!esan_is_dac_char(esan_get_char()))
			break;
		esan_next_char();
	}
}

static void esan_skip_one_char(unsigned char c)
{
	if (esan_get_char() != c)
		esan_maps_parse_error();
	esan_next_char();
}
static void esan_skip_one_space()
{
	esan_skip_one_char(' ');
}

static void esan_skip_number(void)
{
	if (!esan_is_hex_digit(esan_get_char()))
		esan_maps_parse_error();
	(void)esan_parse_hex_number();
}

static void skip_until_new_line(void)
{
	unsigned char c;
	while (((c = esan_get_char()) != '\n') && c != EOF)
		esan_next_char();
}

static void esan_skip_mid(void)
{
	esan_skip_one_space();
	esan_skip_dac();
	esan_skip_one_space();
	esan_skip_number(); /* offset */
	esan_skip_one_space();
	esan_skip_number(); /* dev1 */
	esan_skip_one_char(':');
	esan_skip_number(); /* dev2 */
	esan_skip_one_space();
	esan_skip_number(); /* inode */
	esan_skip_whitespaces();
}

/* returns 0 if it is not library for exclusion 1 otherwise */
static int esan_parse_lib_name(void)
{
	if (esan_get_char() != '/') {
		skip_until_new_line();
		return 0;
	}

	(void)esan_is_alpha(3);
	return -1;
	// TODO me
}

static struct range esan_parse_one_line(void)
{
	struct range ret = { 0, 0 };

	ret.min = esan_parse_hex_number();
	esan_skip_one_char('-');
	ret.max = esan_parse_hex_number();
	esan_skip_mid();

	if (!esan_parse_lib_name()) {
		ret.min = 0;
		ret.max = 0;
	}
	if (esan_get_char() != EOF)
		esan_skip_one_char('\n');

	return ret;
}

void esan_maps_parse(void)
{
	unsigned long i = 0;
	struct range r;

	while (i < MAX_LIBS) {
		if (esan_get_char() == EOF)
			break;
		r = esan_parse_one_line();
		if (r.min == r.max)
			continue;
		libraries[i] = r;
		++i;
	}
}

int in_library(const void *addr)
{
	unsigned long i, min, max, address;
	initialize();
	address = (unsigned long)addr;
	for (i = 0; i < MAX_LIBS; ++i) {
		min = libraries[i].min;
		max = libraries[i].max;
		if (min == max)
			break;

		if (address >= min && address <= max)
			return 1;
	}
	return 0;
}

void initialize(void)
{
	static int done = 1; // TODO change me = 0
	unsigned long i;

	if (done)
		return;

	for (i = 0; i < MAX_LIBS; ++i)
		libraries[i].min = libraries[i].max = 0;

	esan_fd = maps_open();
	if (esan_fd < 0) {
		esan_write_syscall("Error opening maps fail\n");
		done = 1;
		esan_exit_syscall();
		return;
	}

	esan_maps_parse();

	esan_close(esan_fd);
	esan_fd = -1;

	done = 1;
}

void esan_next_char()
{
	static char buf[BUFFER_SIZE];
	static unsigned long size = 0;
	long tmp;

	if (current == EOF)
		return;

	if (size == 0) {
		tmp = esan_read(esan_fd, buf, sizeof(buf));
		if (tmp <= 0) {
			current = EOF;
			return;
		}
		size = tmp;
	}

	--size;
	current = buf[size];
}

#if defined(__x86_64__)
int maps_open()
{
	(void)path;
	return -1;
}
long esan_read(int fd, void *buf, unsigned long size)
{
	(void)fd;
	(void)buf;
	(void)size;
	return -1;
}
int esan_close(int fd)
{
	(void)fd;
	return -1;
}
#else
#error strange architecture
#endif
