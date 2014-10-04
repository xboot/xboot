/*
 * libc/stdlib/llabs.c
 */

#include <stdlib.h>

long long llabs(long long n)
{
	return ((n < 0LL) ? -n : n);
}
EXPORT_SYMBOL(llabs);
