/*
 * libc/environ/putenv.c
 */

#include <runtime.h>
#include <string.h>
#include <malloc.h>
#include <environ.h>

int __put_env(char * str, size_t len, int overwrite)
{
	struct environ_t * environ = &(runtime_get()->__environ);
	struct environ_t * env;
	struct environ_t * p;

	if(!environ)
		return -1;

	for(p = environ->next; p != environ; p = p->next)
	{
		if (!strncmp(p->content, str, len))
		{
			if (!overwrite)
			{
				free(str);
			}
			else
			{
				free(p->content);
				p->content = str;
			}
			return 0;
		}
	}

	env = malloc(sizeof(struct environ_t));
	if(!env)
		return -1;

	env->content = str;
	env->prev = environ->prev;
	env->next = environ;
	environ->prev->next = env;
	environ->prev = env;

	return 0;
}

int putenv(const char * str)
{
	char * s;
	const char * e, * z;

	if(!str)
		return -1;

	e = NULL;
	for(z = str; *z; z++)
	{
		if(*z == '=')
			e = z;
	}

	if(!e)
		return -1;

	s = strdup(str);
	if(!s)
		return -1;

	return __put_env(s, e - str, 1);
}
EXPORT_SYMBOL(putenv);
