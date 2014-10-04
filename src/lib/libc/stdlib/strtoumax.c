/*
 * libc/stdlib/strtoumax.c
 */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

/*
 * Convert a string to a uintmax_t.
 *
 * Ignores 'locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
uintmax_t strtoumax(const char * nptr, char ** endptr, int base)
{
	const char * s;
	uintmax_t acc, cutoff;
	int c;
	int neg, any, cutlim;

	s = nptr;
	do {
		c = (unsigned char) *s++;
	} while (isspace(c));

	if (c == '-')
	{
		neg = 1;
		c = *s++;
	}
	else
	{
		neg = 0;
		if (c == '+')
			c = *s++;
	}

	if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X'))
	{
		c = s[1];
		s += 2;
		base = 16;
	}

	if (base == 0)
		base = c == '0' ? 8 : 10;

	switch (base)
	{
    case 8:
		cutoff = UINTMAX_MAX / 8;
		cutlim = UINTMAX_MAX % 8;
		break;

    case 10:
		cutoff = UINTMAX_MAX / 10;
		cutlim = UINTMAX_MAX % 10;
		break;

    case 16:
		cutoff = UINTMAX_MAX / 16;
		cutlim = UINTMAX_MAX % 16;
		break;

	default:
		cutoff = UINTMAX_MAX / base;
		cutlim = UINTMAX_MAX % base;
		break;
	}

	for (acc = 0, any = 0;; c = (unsigned char) *s++)
	{
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;

		if (c >= base)
			break;

		if (any < 0)
			continue;

		if (acc > cutoff || (acc == cutoff && c > cutlim))
		{
			any = -1;
			acc = UINTMAX_MAX;
			errno = ERANGE;
		}
		else
		{
			any = 1;
			acc *= (uintmax_t) base;
			acc += c;
		}
	}

	if (neg && any > 0)
		acc = -acc;

	if (endptr != 0)
		*endptr = (char *) (any ? s - 1 : nptr);

	return (acc);
}
EXPORT_SYMBOL(strtoumax);

