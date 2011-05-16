/*
 * libc/stdio/fseek.c
 */

#include <xboot.h>
#include <types.h>
#include <stdarg.h>
#include <errno.h>
#include <malloc.h>
#include <fs/fsapi.h>
#include <stdio.h>

int fseek(FILE * fp, loff_t offset, int whence)
{
	//XXX
	return 0;
}
