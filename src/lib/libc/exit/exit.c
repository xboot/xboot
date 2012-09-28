/*
 * libc/exit/exit.c
 */

#include <xboot/module.h>
#include <exit.h>

void exit(int status)
{
	while(1);
}
EXPORT_SYMBOL(exit);
