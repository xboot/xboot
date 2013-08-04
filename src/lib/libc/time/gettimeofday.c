/*
 * libc/time/gettimeofday.c
 */

#include <xboot/module.h>
#include <time.h>

int gettimeofday(struct timeval * tv, void * tz)
{
	if(!tv)
		return -1;
//TODO
	tv->tv_sec = get_time_stamp();
	tv->tv_usec = 0;
	return 0;
}
EXPORT_SYMBOL(gettimeofday);
