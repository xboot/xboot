/*
 * libc/errno/errno.c
 */

#include <runtime.h>
#include <errno.h>

volatile int * __runtime_errno_location(void)
{
	return &(runtime_get()->__errno);
}
EXPORT_SYMBOL(__runtime_errno_location);
