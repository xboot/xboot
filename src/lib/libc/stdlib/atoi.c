/*
 * libc/stdlib/atoi.c
 */

#include <stdlib.h>

int atoi(const char * nptr)
{
	return (int)strtol(nptr, 0, 10);
}
