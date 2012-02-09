/*
 * libc/stdio/__stdio_close.c
 */

#include <stdio.h>

int __stdio_close(FILE * f)
{
	return 0; //xxx syscall(SYS_close, f->fd);
}
