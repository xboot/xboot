/*
 * libc/errno/errno.c
 */

#include <xboot/module.h>
#include <runtime.h>
#include <errno.h>

/*
 * Internal function returning the address of the thread-specific errno
 */
volatile int * __runtime_errno_location(void)
{
	return &(__get_runtime()->__errno);
}
EXPORT_SYMBOL(__runtime_errno_location);
