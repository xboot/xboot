/*
 * libc/time/asctime.c
 */

#include <stddef.h>
#include <stdio.h>
#include <time.h>

static const char * week_days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char * month_days[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

char * asctime(const struct tm * tm)
{
	static char ascbuf[26];

	if(!tm)
		return NULL;

	snprintf(ascbuf, 26, "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
			week_days[tm->tm_wday], month_days[tm->tm_mon], tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec, 1900 + tm->tm_year);
	return ascbuf;
}
EXPORT_SYMBOL(asctime);
