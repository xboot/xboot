/*
 * libc/stdlib/atoi.c
 */

#include <stdlib.h>

int atoi(const char * s)
{
	return (int)strtol(s, 0, 10);
}
