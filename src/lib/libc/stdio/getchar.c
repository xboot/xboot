/*
 * libc/stdio/getchar.c
 */

#include <xboot/module.h>
#include <stdio.h>

int getchar(void)
{
	return fgetc(stdin);
}
EXPORT_SYMBOL(getchar);
