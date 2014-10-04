/*
 * libc/stdlib/strtoll.c
 */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

/*
 * Convert a string to a long long.
 *
 * Ignores 'locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
long long strtoll(const char * nptr, char ** endptr, int base)
{
	const char * s;
	long long acc, cutoff;
	int c;
	int neg, any, cutlim;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
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

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for long long is
	 * [-9223372036854775808..9223372036854775807] and the input base
	 * is 10, cutoff will be set to 922337203685477580 and cutlim to
	 * either 7 (neg==0) or 8 (neg==1), meaning that if we have
	 * accumulated a value > 922337203685477580, or equal but the
	 * next digit is > 7 (or 8), the number is too big, and we will
	 * return a range error.
	 *
	 * Set any if any 'digits' consumed; make it negative to indicate
	 * overflow.
	 */

	switch (base)
	{
	case 4:
		if (neg)
		{
			cutlim = LLONG_MIN % 4;
			cutoff = LLONG_MIN / 4;
		}
		else
		{
			cutlim = LLONG_MAX % 4;
			cutoff = LLONG_MAX / 4;
		}
		break;

	case 8:
		if (neg)
		{
			cutlim = LLONG_MIN % 8;
			cutoff = LLONG_MIN / 8;
		}
		else
		{
			cutlim = LLONG_MAX % 8;
			cutoff = LLONG_MAX / 8;
		}
		break;

	case 10:
		if (neg)
		{
			cutlim = LLONG_MIN % 10;
			cutoff = LLONG_MIN / 10;
		}
		else
		{
			cutlim = LLONG_MAX % 10;
			cutoff = LLONG_MAX / 10;
		}
		break;

	case 16:
		if (neg)
		{
			cutlim = LLONG_MIN % 16;
			cutoff = LLONG_MIN / 16;
		}
		else
		{
			cutlim = LLONG_MAX % 16;
			cutoff = LLONG_MAX / 16;
		}
		break;

	default:
		cutoff = neg ? LLONG_MIN : LLONG_MAX;
		cutlim = cutoff % base;
		cutoff /= base;
		break;
	}

	if (neg)
	{
		if (cutlim > 0)
		{
			cutlim -= base;
			cutoff += 1;
		}
		cutlim = -cutlim;
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

		if (neg)
		{
			if (acc < cutoff || (acc == cutoff && c > cutlim))
			{
				any = -1;
				acc = LLONG_MIN;
				errno = ERANGE;
			}
			else
			{
				any = 1;
				acc *= base;
				acc -= c;
			}
		}
		else
		{
			if (acc > cutoff || (acc == cutoff && c > cutlim))
			{
				any = -1;
				acc = LLONG_MAX;
				errno = ERANGE;
			}
			else
			{
				any = 1;
				acc *= base;
				acc += c;
			}
		}
	}

	if (endptr != 0)
		*endptr = (char *) (any ? s - 1 : nptr);

	return (acc);
}
EXPORT_SYMBOL(strtoll);
