/*
 * libc/stdlib/labs.c
 */

#include <xboot/module.h>
#include <stdlib.h>

long labs(long n)
{
	return ((n < 0L) ? -n : n);
}
EXPORT_SYMBOL(labs);
