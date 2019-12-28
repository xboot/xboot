/*
 * libc/exit/exit.c
 */

#include <exit.h>
#include <xboot/module.h>

void __attribute__((__noreturn__)) exit(int code)
{
	while(1);
}
EXPORT_SYMBOL(exit);
