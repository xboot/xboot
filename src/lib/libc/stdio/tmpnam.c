/*
 * libc/stdio/tmpname.c
 */

#include <fs/fileio.h>
#include <stdio.h>

char * tmpnam(char * buf)
{
	static char internal[L_tmpnam];
	struct stat st;
	char path[MAX_PATH];

	do {
		sprintf(path, "%s/tmpnam_%d", "/tmp", rand());
	} while(stat(path, &st) == 0);

	return strcpy(buf ? buf : internal, path);
}
EXPORT_SYMBOL(tmpnam);
