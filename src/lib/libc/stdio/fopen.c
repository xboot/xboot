/*
 * libc/stdio/fopen.c
 */

#include <fs/fileio.h>
#include <stdio.h>

FILE * fopen(const char * filename, const char * mode)
{
	FILE *f;
	int fd;
	int flags;
	int plus = !!strchr(mode, '+');

	/* Check for valid initial mode character */
	if (!strchr("rwa", *mode)) {
		errno = EINVAL;
		return 0;
	}

	/* Compute the flags to pass to open() */
	if (plus) flags = O_RDWR;
	else if (*mode == 'r') flags = O_RDONLY;
	else flags = O_WRONLY;
	if (*mode != 'r') flags |= O_CREAT;
	if (*mode == 'w') flags |= O_TRUNC;
	if (*mode == 'a') flags |= O_APPEND;

	//xxx fd = syscall(SYS_open, filename, flags|O_LARGEFILE, 0666);
	if (fd < 0) return 0;

	//xxx f = __fdopen(fd, mode);
	if (f) return f;

	//xxx __syscall(SYS_close, fd);
	return 0;
}
