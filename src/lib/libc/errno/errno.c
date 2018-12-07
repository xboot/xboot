/*
 * libc/errno/errno.c
 */

#include <xboot.h>
#include <errno.h>

volatile int * __task_errno_location(void)
{
	return &(task_self()->__errno);
}
EXPORT_SYMBOL(__task_errno_location);
