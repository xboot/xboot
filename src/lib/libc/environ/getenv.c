/*
 * libc/environ/getenv.c
 */

#include <stddef.h>
#include <string.h>
#include <environ.h>
#include <xboot/module.h>

char * getenv(const char * name)
{
	struct environ_t * xenv = &__xenviron;
	struct environ_t * p;
	int len;

	if(!xenv || !xenv->content)
		return NULL;

	len = strlen(name);
	for(p = xenv->next; p != xenv; p = p->next)
	{
		if(!strncmp(name, p->content, len) && (p->content[len] == '='))
			return p->content + (len + 1);
	}
	return NULL;
}
EXPORT_SYMBOL(getenv);
