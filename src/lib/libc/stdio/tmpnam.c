/*
 * libc/stdio/tmpname.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <vfs/vfs.h>
#include <xboot/module.h>

char * tmpnam(char * buf)
{
	static char internal[L_tmpnam];
	struct vfs_stat_t st;
	char path[VFS_MAX_PATH];

	do {
		sprintf(path, "%s/tmpnam_%d", "/tmp", rand());
	} while(vfs_stat(path, &st) < 0);

	return strcpy(buf ? buf : internal, path);
}
EXPORT_SYMBOL(tmpnam);
