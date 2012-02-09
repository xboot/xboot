/*
 * libc/stdio/rewind.c
 */

#include <stdio.h>

void rewind(FILE * f)
{
	FLOCK(f);
	__fseeko_unlocked(f, 0, SEEK_SET);
	f->flags &= ~F_ERR;
	FUNLOCK(f);
}

