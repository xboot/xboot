/*
 * libc/exit/assert.c
 */

#include <xboot.h>
#include <assert.h>

void __assert_fail(const char * file, int line, const char * func, const char * expr)
{
	fprintf(stderr, "assertion \"%s\" failed: file \"%s\", line %d, function \"%s\"\n", expr, file, line, func);
}
