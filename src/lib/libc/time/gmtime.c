/*
 * libc/time/gmtime.c
 */

#include <time.h>
#include <xboot/module.h>

struct tm * gmtime(const time_t * t)
{
	static struct tm tm;
	return gmtime_r(t, &tm);
}
EXPORT_SYMBOL(gmtime);
