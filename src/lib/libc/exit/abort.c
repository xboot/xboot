/*
 * libc/exit/abort.c
 */

#include <exit.h>
#include <xboot/module.h>

void abort(void)
{
	 while(1);
}
EXPORT_SYMBOL(abort);
