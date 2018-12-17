/*
 * libc/exit/assert.c
 */

#include <stdio.h>
#include <assert.h>
#include <xboot/module.h>

void __assert_fail(const char * expr, const char * file, int line, const char * func)
{
	fprintf(stderr, "Assertion failed: %s (%s: %s: %d)\r\n", expr, file, func, line);
}
EXPORT_SYMBOL(__assert_fail);
