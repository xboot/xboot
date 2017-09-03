/*
 * libc/path/basename.c
 */

#include <path.h>

char * basename(char * path)
{
	int i;
	if(!path || !*path)
		return ".";
	i = strlen(path) - 1;
	for(; i &&path [i] == '/'; i--)
		path[i] = 0;
	for(; i &&path [i-1] != '/'; i--);
	return path + i;
}
EXPORT_SYMBOL(basename);
