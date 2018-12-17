/*
 * libc/stdio/remove.c
 */

#include <stdio.h>
#include <vfs/vfs.h>
#include <xboot/module.h>

int remove(const char * path)
{
	return vfs_unlink(path);
}
EXPORT_SYMBOL(remove);
