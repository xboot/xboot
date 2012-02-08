/*
 * libc/stdio/clearerr.c
 */

#include <stdio.h>

void clearerr(FILE * fp)
{
	flockfile(fp);
	__sclearerr(fp);
	funlockfile(fp);
}
