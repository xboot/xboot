/*
 * libc/stdio/feof.c
 */

#include <stdio.h>

int feof(FILE * fp)
{
	int ret;

	flockfile(fp);
	ret = __sfeof(fp);
	funlockfile(fp);

	return (ret);
}
