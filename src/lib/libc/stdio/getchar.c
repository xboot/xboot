/*
 * libc/stdio/getchar.c
 */

#include <stdio.h>
#include <xboot/module.h>

int getchar(void)
{
	return fgetc(stdin);
}
EXPORT_SYMBOL(getchar);
