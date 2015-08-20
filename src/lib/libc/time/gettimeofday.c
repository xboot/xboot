/*
 * libc/time/gettimeofday.c
 */

#include <clocksource/clocksource.h>
#include <time.h>

int gettimeofday(struct timeval * tv, void * tz)
{
	if(!tv)
		return -1;

	ktime_t kt = ktime_get();
	tv->tv_sec = ktime_to_ns(kt) / 1000000000ULL;
	tv->tv_usec = (ktime_to_ns(kt) % 1000000000ULL) / 1000;
	return 0;
}
EXPORT_SYMBOL(gettimeofday);
