/*
 * libc/errno/errno.c
 */

#include <errno.h>

/*
 * global error number
 */
static int __errno = 0;

volatile int *__errno_location(void)
{
	return &__errno;
}
