/*
 * libc/exit/abort.c
 */

#include <exit.h>
#include <xboot/module.h>

void __attribute__((__noreturn__)) abort(void)
{
	 while(1);
}
EXPORT_SYMBOL(abort);
