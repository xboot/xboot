/*
 * libc/time/asctime.c
 */

#include <time.h>
#include <xboot/module.h>

char * asctime(const struct tm * tm)
{
	static char buf[32];
	return asctime_r(tm, buf);
}
EXPORT_SYMBOL(asctime);
