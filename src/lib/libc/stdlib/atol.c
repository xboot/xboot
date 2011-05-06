/*
 * libc/stdlib/atol.c
 */

#include <stdlib.h>

long atol(const char * s)
{
	return (long)strtol(s, 0, 10);
}
