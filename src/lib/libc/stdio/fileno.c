/*
 * libc/stdio/fileno.c
 */

#include <stdio.h>

int fileno(FILE * fp)
{
	int ret;

	flockfile(fp);
	ret = __sfileno(fp);
	funlockfile(fp);

	return (ret);
}
