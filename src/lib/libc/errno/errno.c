/*
 * libc/errno/errno.c
 */

#include <errno.h>
#include <xboot/task.h>
#include <xboot/module.h>

volatile int * __task_errno_location(void)
{
	return &(task_self()->__errno);
}
EXPORT_SYMBOL(__task_errno_location);
