/*
 * libc/string/memscan.c
 */

#include <types.h>
#include <string.h>

/*
 * Find a character in an area of memory
 */
void * memscan(void * addr, int c, size_t size)
{
	unsigned char * p = addr;

	while (size)
	{
		if (*p == c)
			return (void *)p;
		p++;
		size--;
	}
  	return (void *)p;
}
EXPORT_SYMBOL(memscan);
