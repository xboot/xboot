/*
 * libc/errno/errno.c
 */

#include <xboot/module.h>
#include <runtime.h>
#include <errno.h>

/*
 * Internal function returning the address of the thread-specific errno
 */
volatile int * __errno_location(void)
{
	return &(__get_runtime()->__errno);
}
EXPORT_SYMBOL(__errno_location);
