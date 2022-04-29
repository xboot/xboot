/*
 * libc/time/settimeofday.c
 */

#include <time.h>
#include <rtc/rtc.h>
#include <clocksource/clocksource.h>
#include <xboot/module.h>

extern int64_t time_of_day_adjust;

int settimeofday(struct timeval * tv, void * tz)
{
	if(!tv)
		return -1;
	int64_t ns = tv->tv_sec * 1000000000ULL + tv->tv_usec * 1000;
	time_of_day_adjust = ns - ktime_to_ns(ktime_get());
	struct rtc_t * rtc = search_first_rtc();
	if(rtc)
	{
		struct rtc_time_t t;
		secs_to_rtc_time((uint64_t)tv->tv_sec, &t);
		rtc_settime(rtc, &t);
	}
	return 0;
}
EXPORT_SYMBOL(settimeofday);
