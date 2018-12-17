/*
 * libc/exit/exit.c
 */

#include <exit.h>
#include <xboot/module.h>

void exit(int code)
{
	while(1);
}
EXPORT_SYMBOL(exit);
