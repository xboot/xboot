/*
 * libc/time/timegm.c
 */

#include <time.h>
#include <xboot/module.h>

time_t timegm(struct tm * tm)
{
	return __tm_to_secs(tm);
}
EXPORT_SYMBOL(timegm);
