/*
 * libc/stdio/ferror.c
 */

#include <stdio.h>

int ferror(FILE * fp)
{
	int ret;

	flockfile(fp);
	ret = __sferror(fp);
	funlockfile(fp);

	return (ret);
}
