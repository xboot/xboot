/*
 * libc/time/localtime.c
 */

#include <time.h>
#include <xboot/module.h>

struct tm * localtime(const time_t * t)
{
	static struct tm tm;
	return localtime_r(t, &tm);
}
EXPORT_SYMBOL(localtime);
