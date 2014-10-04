/*
 * libc/exit/assert.c
 */

#include <stdio.h>
#include <assert.h>

void __assert_fail(const char * expr, const char * file, int line, const char * func)
{
	fprintf(stderr, "Assertion failed: %s (%s: %s: %d)\n", expr, file, func, line);
}
EXPORT_SYMBOL(__assert_fail);
