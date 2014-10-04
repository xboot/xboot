/*
 * libc/time/clock.c
 */

#include <types.h>
#include <time.h>

clock_t clock(void)
{
	struct timeval tv;

	if(gettimeofday(&tv, 0) != 0)
		return -1;
	return tv.tv_sec * 1000000 + tv.tv_usec;
}
EXPORT_SYMBOL(clock);
