/*
 * libc/stdio/fopen.c
 */

#include <xboot.h>
#include <types.h>
#include <stdarg.h>
#include <malloc.h>
#include <fs/fileio.h>
#include <stdio.h>

FILE * fopen(const char * file, const char * mode)
{
	int flags = O_RDONLY;
	int plus = 0;
	FILE * fp;
	int fd;

	while(*mode)
	{
		switch(*mode++)
		{
		case 'r':
			flags = O_RDONLY;
			break;
		case 'w':
			flags = O_WRONLY | O_CREAT | O_TRUNC;
			break;
		case 'a':
			flags = O_WRONLY | O_CREAT | O_APPEND;
			break;
		case '+':
			plus = 1;
			break;
		}
	}

	if(plus)
		flags = (flags & ~(O_RDONLY | O_WRONLY)) | O_RDWR;

	fp = malloc(sizeof(FILE));
	if(!fp)
		return NULL;

	fd = open(file, flags, (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH));
	if(fd < 0)
	{
		free(fp);
		return NULL;
	}

	fp->fd = fd;

	return fp;
}
