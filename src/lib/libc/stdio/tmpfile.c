/*
 * libc/stdio/tmpfile.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <vfs/vfs.h>
#include <xboot/module.h>

FILE * tmpfile(void)
{
	struct vfs_stat_t st;
	char path[VFS_MAX_PATH];

	do {
		sprintf(path, "%s/tmpfile_%d", "/tmp", rand());
	} while(vfs_stat(path, &st) < 0);

	return fopen(path, "wb+");
}
EXPORT_SYMBOL(tmpfile);
