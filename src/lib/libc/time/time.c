/*
 * libc/time/time.c
 */

#include <time.h>

time_t time(time_t * t)
{
	struct timeval tv;

	if(gettimeofday(&tv, 0) != 0)
		return -1;

	if(t)
		*t = tv.tv_sec;

	return tv.tv_sec;
}
EXPORT_SYMBOL(time);
