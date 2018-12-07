/*
 * libc/environ/clearenv.c
 */

#include <environ.h>

int clearenv(void)
{
	struct environ_t * environ = &__environ;
	struct environ_t * p, * q;

	if (!environ || !environ->content)
		return -1;

	for(p = environ->next; p != environ;)
	{
		q = p;
		p = p->next;

		q->next->prev = q->prev;
		q->prev->next = q->next;
		free(q->content);
		free(q);
	}
	return 0;
}
EXPORT_SYMBOL(clearenv);
