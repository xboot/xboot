/*
 * libc/stdio/tmpfile.c
 */

#include <stdio.h>

FILE * tmpfile(void)
{
	struct stat st;
	char path[MAX_PATH];

	do {
		sprintf(path, "%s/tmp.%d", "/tmp", rand());
	} while(stat(path, &st) == 0);

	return fopen(path, "wb+");
}
