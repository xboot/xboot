/*
 * libc/stdio/rename.c
 */

#include <vfs/vfs.h>
#include <stdio.h>

int rename(const char * old, const char * new)
{
	return vfs_rename(old, new);
}
EXPORT_SYMBOL(rename);
