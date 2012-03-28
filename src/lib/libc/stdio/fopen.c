/*
 * libc/stdio/fopen.c
 */

#include <fs/fileio.h>
#include <stdio.h>

FILE * fopen(const char * filename, const char * mode)
{
	FILE * f;
	int flags;
	int plus = !!strchr(mode, '+');

	/*
	 * Check for valid initial mode character
	 */
	if(!strchr("rwa", *mode))
	{
		errno = EINVAL;
		return 0;
	}

	/*
	 * Allocate memory for hold FILE
	 */
	f = malloc(sizeof(FILE));
	if(!f)
	{
		errno = ENOMEM;
		return 0;
	}

	/*
	 * Compute the flags to pass to open()
	 */
	if(plus)
		flags = O_RDWR;
	else if (*mode == 'r')
		flags = O_RDONLY;
	else
		flags = O_WRONLY;

	if(*mode != 'r')
		flags |= O_CREAT;
	if(*mode == 'w')
		flags |= O_TRUNC;
	if(*mode == 'a')
		flags |= O_APPEND;

	f->fd = open(filename, flags, (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH));
	if(f->fd < 0)
	{
		free(f);
		return 0;
	}

	f->read = __file_read;
	f->write = __file_write;
	f->seek = __file_seek;
	f->close = __file_close;

	__stdio_init(f);

	f->pos = lseek(f->fd, 0, VFS_SEEK_CUR);
	//file->mode = _IOFBF;

	return f;
}
