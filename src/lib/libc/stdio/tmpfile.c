/*
 * libc/stdio/tmpfile.c
 */

#include <fs/fileio.h>
#include <stdio.h>

FILE * tmpfile(void)
{
	struct stat st;
	char path[MAX_PATH];

	do {
		sprintf(path, "%s/tmpfile_%d", "/tmp", rand());
	} while(stat(path, &st) == 0);

	return fopen(path, "wb+");
}
EXPORT_SYMBOL(tmpfile);
