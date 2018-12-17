/*
 * libc/time/localtime.c
 */

#include <time.h>
#include <xboot/module.h>

struct tm * localtime(const time_t * t)
{
	return gmtime(t);
}
EXPORT_SYMBOL(localtime);
