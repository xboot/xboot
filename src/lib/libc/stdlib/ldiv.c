/*
 * libc/stdlib/ldiv.c
 */

#include <stdlib.h>
#include <xboot/module.h>

ldiv_t ldiv(long num, long den)
{
	return (ldiv_t){ num / den, num % den };
}
EXPORT_SYMBOL(ldiv);
