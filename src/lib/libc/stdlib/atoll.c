/*
 * libc/stdlib/atoll.c
 */

#include <stdlib.h>

long long atoll(const char * s)
{
	return (long long)strtoll(s, 0, 10);
}
