/*
 * libc/time/localtime_r.c
 */

#include <time.h>
#include <xboot/setting.h>
#include <xboot/module.h>

struct tm * localtime_r(const time_t * t, struct tm * tm)
{
	if(__secs_to_tm(*t + timezone(setting_get("timezone", NULL)) * 60, tm) < 0)
		return NULL;
	tm->__tm_gmtoff = 0;
	tm->__tm_zone = "UTC";
	return tm;
}
EXPORT_SYMBOL(localtime_r);
