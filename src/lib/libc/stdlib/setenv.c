/*
 * libc/stdlib/setenv.c
 */

#include <string.h>
#include <malloc.h>
#include <stdlib.h>

int setenv(const char * name, const char * value, int overwrite)
{
	if (getenv(name))
	{
		if (!overwrite)
			return 0;
		unsetenv(name);
	}
	{
		char * c = malloc(strlen(name) + strlen(value) + 2);
		strcpy(c, name);
		strcat(c, "=");
		strcat(c, value);
		return putenv(c);
	}
}
