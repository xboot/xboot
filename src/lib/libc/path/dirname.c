/*
 * libc/path/dirname.c
 */

#include <path.h>

char * dirname(char * path)
{
	int i;
	if(!path || !*path)
		return ".";
	i = strlen(path) - 1;
	for(; path[i]=='/'; i--)
	{
		if(!i)
			return "/";
	}
	for(; path[i]!='/'; i--)
	{
		if(!i)
			return ".";
	}
	for(; path[i]=='/'; i--)
	{
		if(!i)
			return "/";
	}
	path[i + 1] = 0;
	return path;
}
EXPORT_SYMBOL(dirname);
