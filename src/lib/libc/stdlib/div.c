/*
 * libc/stdlib/div.c
 */

#include <xboot/module.h>
#include <stdlib.h>

div_t div(int num, int den)
{
	return (div_t){ num / den, num % den };
}
EXPORT_SYMBOL(div);
