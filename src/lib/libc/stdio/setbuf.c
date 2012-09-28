/*
 * libc/stdio/setbuf.c
 */

#include <xboot/module.h>
#include <stdio.h>

void setbuf(FILE * f, char * buf)
{
	setvbuf(f, buf, buf ? _IOFBF : _IONBF, BUFSIZ);
}
EXPORT_SYMBOL(setbuf);
