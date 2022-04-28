/*
 * libc/time/mktime.c
 */

#include <time.h>
#include <xboot/setting.h>
#include <xboot/module.h>

time_t mktime(struct tm * tm)
{
	return __tm_to_secs(tm) - timezone(setting_get("timezone", NULL));
}
EXPORT_SYMBOL(mktime);
