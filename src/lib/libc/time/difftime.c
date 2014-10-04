/*
 * libc/time/difftime.c
 */

#include <time.h>

double difftime(time_t t1, time_t t0)
{
	return (t1 - t0);
}
EXPORT_SYMBOL(difftime);
