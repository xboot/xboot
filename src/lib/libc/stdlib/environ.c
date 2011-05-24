/*
 * libc/stdlib/environ.c
 */

#include <stdlib.h>

static char ** __environ = 0;

char *** __environ_location(void)
{
	return &__environ;
}
