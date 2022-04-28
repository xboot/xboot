/*
 * libc/time/mktime.c
 */

#include <time.h>
#include <xboot/module.h>

time_t mktime(struct tm * tm)
{
	return timegm(tm);
}
EXPORT_SYMBOL(mktime);
