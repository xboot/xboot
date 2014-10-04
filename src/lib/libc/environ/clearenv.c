/*
 * libc/environ/clearenv.c
 */

#include <runtime.h>
#include <environ.h>

int clearenv(void)
{
	struct environ_t * environ = &(runtime_get()->__environ);
	struct environ_t * p, * q;

	if (!environ)
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
