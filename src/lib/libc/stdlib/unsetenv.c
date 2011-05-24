/*
 * libc/stdlib/unsetenv.c
 */

#include <stdlib.h>

int unsetenv(const char * name)
{
	return putenv(name);
}
