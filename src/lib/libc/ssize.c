/*
 * libc/stdio/ssize.c
 */

#include <types.h>
#include <stdarg.h>
#include <ssize.h>

int ssize(char * buf, u64_t size)
{
	const char * unit[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
	int count = 0;

	while( ((size >> 10) > 0) && (count < 8) )
	{
		size = size >> 10;
		count++;
	}

	return( sprintf(buf, "%llu%s", size, unit[count]) );
}
