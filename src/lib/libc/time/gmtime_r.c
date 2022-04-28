/*
 * libc/time/gmtime_r.c
 */

#include <time.h>
#include <xboot/module.h>

struct tm * gmtime_r(const time_t * t, struct tm * tm)
{
	if(__secs_to_tm(*t, tm) < 0)
		return NULL;
	return tm;
}
EXPORT_SYMBOL(gmtime_r);
