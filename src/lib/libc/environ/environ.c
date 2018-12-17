/*
 * libc/environ/environ.c
 */

#include <stddef.h>
#include <environ.h>
#include <xboot/module.h>

struct environ_t __environ = {
	.content = NULL,
	.prev = &__environ,
	.next = &__environ,
};
