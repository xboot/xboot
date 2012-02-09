/*
 * libc/stdio/__stdio_seek.c
 */

#include <stdio.h>

off_t __stdio_seek(FILE * f, off_t off, int whence)
{
	off_t ret;
#ifdef SYS__llseek
	//xxx if (syscall(SYS__llseek, f->fd, off>>32, off, &ret, whence)<0)
		ret = -1;
#else
	//xxx ret = syscall(SYS_lseek, f->fd, off, whence);
#endif
	return ret;
}
