/*
 * libc/stdlib/atof.c
 */

#include <stdlib.h>

double atof(const char * nptr)
{
	return (double)strtod(nptr, 0);
}
EXPORT_SYMBOL(atof);
