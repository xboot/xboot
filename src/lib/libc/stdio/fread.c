/*
 * libc/stdio/fread.c
 */

#include <xboot.h>
#include <types.h>
#include <stdarg.h>
#include <malloc.h>
#include <fs/fileio.h>
#include <stdio.h>

size_t fread(void * buf, size_t size, size_t count, FILE * fp)
{
	loff_t len;
	loff_t ret;

	if(!fp)
		return 0;

	len = size * count;
	if(len == 0)
		return 0;

	ret = read(fp->fd, buf, len);
	ret = ret / size;

	return (size_t)ret;
}
