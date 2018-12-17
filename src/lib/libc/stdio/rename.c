/*
 * libc/stdio/rename.c
 */

#include <stdio.h>
#include <vfs/vfs.h>
#include <xboot/module.h>

int rename(const char * old, const char * new)
{
	return vfs_rename(old, new);
}
EXPORT_SYMBOL(rename);
