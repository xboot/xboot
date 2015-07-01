/*
 * libc/time/gettimeofday.c
 */

#include <clocksource/clocksource.h>
#include <time.h>

int gettimeofday(struct timeval * tv, void * tz)
{
	u64_t t;

	if(!tv)
		return -1;

	t = clocksource_gettime();
	tv->tv_sec = t / 1000000000L;
	tv->tv_usec = t % 1000000000L;
	return 0;
}
EXPORT_SYMBOL(gettimeofday);
