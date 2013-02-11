/*
 * libc/exit/abort.c
 */

#include <xboot/module.h>
#include <exit.h>

void abort(void)
{
	 while(1);
}
EXPORT_SYMBOL(abort);
