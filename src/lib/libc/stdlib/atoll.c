/*
 * libc/stdlib/atoll.c
 */

#include <stdlib.h>

long long atoll(const char * nptr)
{
	return (long long)strtoll(nptr, 0, 10);
}
