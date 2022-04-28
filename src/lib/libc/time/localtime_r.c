/*
 * libc/time/localtime_r.c
 */

#include <time.h>
#include <xboot/setting.h>
#include <xboot/module.h>

struct tm * localtime_r(const time_t * t, struct tm * tm)
{
	if(__secs_to_tm(*t + timezone(setting_get("timezone", NULL)), tm) < 0)
		return NULL;
	return tm;
}
EXPORT_SYMBOL(localtime_r);
