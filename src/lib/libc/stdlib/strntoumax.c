/*
 * libc/stdlib/strntoumax.c
 */

#include <stddef.h>
#include <ctype.h>
#include <stdlib.h>

static inline int digitval(int ch)
{
    unsigned d;

    d = (unsigned)(ch - '0');
    if (d < 10) return (int)d;

    d = (unsigned)(ch - 'a');
    if (d < 6) return (int)(d+10);

    d = (unsigned)(ch - 'A');
    if (d < 6) return (int)(d+10);

    return -1;
}

uintmax_t strntoumax(const char * nptr, char ** endptr, int base, size_t n)
{
	const unsigned char * p = (const unsigned char *) nptr;
	const unsigned char * end = p + n;
	int minus = 0;
	uintmax_t v = 0;
	int d;

	/* skip leading space */
	while (p < end && isspace(*p))
		p++;

	/* Single optional + or - */
	if (p < end)
	{
		char c = p[0];
		if (c == '-' || c == '+')
		{
			minus = (c == '-');
			p++;
		}
	}

	if (base == 0)
	{
		if (p + 2 < end && p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
		{
			p += 2;
			base = 16;
		}
		else if (p + 1 < end && p[0] == '0')
		{
			p += 1;
			base = 8;
		}
		else
		{
			base = 10;
		}
	}
	else if (base == 16)
	{
		if (p + 2 < end && p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
		{
			p += 2;
		}
	}

	while (p < end && (d = digitval(*p)) >= 0 && d < base)
	{
		v = v * base + d;
		p += 1;
	}

	if (endptr)
		*endptr = (char *) p;

	return minus ? -v : v;
}
EXPORT_SYMBOL(strntoumax);
