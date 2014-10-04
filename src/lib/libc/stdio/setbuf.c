/*
 * libc/stdio/setbuf.c
 */

#include <stdio.h>

void setbuf(FILE * f, char * buf)
{
	setvbuf(f, buf, buf ? _IOFBF : _IONBF, BUFSIZ);
}
EXPORT_SYMBOL(setbuf);
