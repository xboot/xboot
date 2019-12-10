/*
 * libc/time/gmtime.c
 */

#include <time.h>
#include <xboot/module.h>

struct tm * gmtime(const time_t * t)
{
	static struct tm tm;

	if(__secs_to_tm(*t, &tm) < 0)
		return NULL;
	tm.tm_isdst = 0;
	tm.__tm_gmtoff = 0;
	tm.__tm_zone = "UTC";
	return &tm;
}
EXPORT_SYMBOL(gmtime);
