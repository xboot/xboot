/*
 * libc/stdio/fclose.c
 */

#include <xboot.h>
#include <types.h>
#include <stdarg.h>
#include <errno.h>
#include <malloc.h>
#include <fs/fsapi.h>
#include <stdio.h>

int fclose(FILE * fp)
{
	int ret;

	if(!fp)
	{
		errno = EBADF;
		return -1;
	}

	ret = close(fp->fd);

	free(fp);
	return ret;
}
