/*
 * libc/stdlib/clearenv.c
 */

#include <stdlib.h>

int clearenv(void)
{
	static char * const __null_environ[] = { NULL };

	environ = (char **)__null_environ;
	return 0;
}
