/*
 * libc/environ/environ.c
 */

#include <environ.h>

struct environ_t __environ = {
	.content = NULL,
	.prev = &__environ,
	.next = &__environ,
};
