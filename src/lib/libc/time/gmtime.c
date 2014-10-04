/*
 * libc/time/gmtime.c
 */

#include <time.h>

struct tm * gmtime(const time_t * t)
{
	static struct tm tm;

	__time_to_tm(*t, &tm);
	tm.tm_isdst = 0;

	return &tm;
}
EXPORT_SYMBOL(gmtime);
