/*
 * libc/errno/errno.c
 */

#include <runtime.h>
#include <errno.h>

volatile int * __errno_location(void)
{
	return &(__get_runtime()->__errno);
}
