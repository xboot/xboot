/*
 * libc/stdio/fclose.c
 */

#include <malloc.h>
#include <stdio.h>

int fclose(FILE * f)
{
	int r;
	int perm = f->flags & F_PERM;

	if (!perm)
	{
		OFLLOCK();
		if (f->prev) f->prev->next = f->next;
		if (f->next) f->next->prev = f->prev;
		//xxx if (libc.ofl_head == f) libc.ofl_head = f->next;
		OFLUNLOCK();
	}

	r = fflush(f);
	r |= f->close(f);

	if (!perm)
		free(f);

	return r;
}
