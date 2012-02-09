/*
 * libc/stdio/clearerr.c
 */

#include <stdio.h>

void clearerr(FILE * f)
{
	FLOCK(f);
	f->flags &= ~(F_EOF | F_ERR);
	FUNLOCK(f);
}
