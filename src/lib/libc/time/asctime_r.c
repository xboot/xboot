/*
 * libc/time/asctime_r.c
 */

#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <xboot/module.h>

static const char * week_days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char * month_days[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

char * asctime_r(const struct tm * tm, char * buf)
{
	if(!tm || !buf)
		return NULL;

	sprintf(buf, "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
			week_days[tm->tm_wday], month_days[tm->tm_mon], tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec, 1900 + tm->tm_year);
	return buf;
}
EXPORT_SYMBOL(asctime_r);
