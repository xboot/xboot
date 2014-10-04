/*
 * libc/stdlib/lldiv.c
 */

#include <stdlib.h>

lldiv_t lldiv(long long num, long long den)
{
	return (lldiv_t){ num / den, num % den };
}
EXPORT_SYMBOL(lldiv);
