/*
 * libc/stdlib/atol.c
 */

#include <stdlib.h>

long atol(const char * nptr)
{
	return (long)strtol(nptr, 0, 10);
}
