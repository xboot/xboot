/*
 * libc/stdio/fopen.c
 */

#include <fs/fileio.h>
#include <stdio.h>

FILE * fopen(const char * filename, const char * mode)
{
	FILE * f;
	int fd;
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

	fd = open(filename, flags, (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH));
	if(fd < 0)
		return NULL;

	f = __fdopen(fd, mode);
	if(f)
		return f;

	close(fd);
	return 0;
}
