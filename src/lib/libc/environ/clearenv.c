/*
 * libc/environ/clearenv.c
 */

#include <malloc.h>
#include <environ.h>
#include <xboot/module.h>

int clearenv(void)
{
	struct environ_t * xenv = &__xenviron;
	struct environ_t * p, * q;

	if (!xenv || !xenv->content)
		return -1;

	for(p = xenv->next; p != xenv;)
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
