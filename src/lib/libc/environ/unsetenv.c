/*
 * libc/environ/unsetenv.c
 */

#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <environ.h>
#include <xboot/module.h>

int unsetenv(const char * name)
{
	struct environ_t * xenv = &__xenviron;
	struct environ_t * p;
	size_t len;
	const char * z;

	if(!name || !name[0])
		return -1;

	len = 0;
	for(z = name; *z; z++)
	{
		len++;
		if(*z == '=')
			return -1;
	}

	if(!xenv || !xenv->content)
		return 0;

	for(p = xenv->next; p != xenv; p = p->next)
	{
		if(!strncmp(name, p->content, len) && (p->content[len] == '='))
		{
			p->next->prev = p->prev;
			p->prev->next = p->next;

			free(p->content);
			free(p);
			break;
		}
	}
	return 0;
}
EXPORT_SYMBOL(unsetenv);
