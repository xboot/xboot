/*
 * libc/path/fileext.c
 */

#include <string.h>
#include <path.h>
#include <xboot/module.h>

const char * fileext(const char * filename)
{
	const char * ret = NULL;
	const char * p;

	if(filename != NULL)
	{
		ret = p = strchr(filename, '.');
		while(p != NULL)
		{
			p = strchr(p + 1, '.');
			if(p != NULL)
				ret = p;
		}
		if(ret != NULL)
			ret++;
	}
	return ret;
}
EXPORT_SYMBOL(fileext);
