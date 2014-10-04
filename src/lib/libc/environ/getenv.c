/*
 * libc/environ/getenv.c
 */

#include <runtime.h>
#include <stddef.h>
#include <string.h>
#include <environ.h>

char * getenv(const char * name)
{
	struct environ_t * environ = &(runtime_get()->__environ);
	struct environ_t * p;
	int len;

	if(!environ)
		return NULL;

	len = strlen(name);
	for(p = environ->next; p != environ; p = p->next)
	{
		if(!strncmp(name, p->content, len) && (p->content[len] == '='))
		{
			return p->content + (len + 1);
		}
	}

	return NULL;
}
EXPORT_SYMBOL(getenv);
