/*
 * drivers/rtc/rtc.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#include <xboot.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <xboot/printk.h>
#include <xboot/ioctl.h>
#include <time/xtime.h>
#include <rtc/rtc.h>

#define LEAPS_THRU_END_OF(y)			((y)/4 - (y)/100 + (y)/400)
#define LEAP_YEAR(year)					((!(year % 4) && (year % 100)) || !(year % 400))

static const u8_t rtc_days_in_month[13] = {
	0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const u16_t rtc_ydays[2][13] = {
	/* normal years */
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
	/* leap years */
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

/*
 * the number of days in the month.
 */
u32_t rtc_month_days(u32_t year, u32_t month)
{
	return rtc_days_in_month[month] + (LEAP_YEAR(year) && month == 2);
}

/*
 * the number of days since January 1. (0 to 365)
 */
u32_t rtc_year_days(u32_t year, u32_t month, u32_t day)
{
	return rtc_ydays[LEAP_YEAR(year)][month] + day-1;
}

/*
 * does the time represent a valid date/time?
 */
bool_t rtc_valid_time(struct xtime_t * tm)
{
	if (tm->year < 1970
		|| (tm->mon) > 12
		|| tm->day < 1
		|| tm->day > rtc_month_days(tm->year, tm->mon)
		|| (tm->hour) >= 24
		|| (tm->min) >= 60
		|| (tm->sec) >= 60)
		return FALSE;

	return TRUE;
}

/*
 * convert seconds since 01-01-1970 00:00:00 to gregorian date.
 */
void rtc_to_time(unsigned long time, struct xtime_t *tm)
{
	u32_t month, year;
	s32_t days;
	s32_t newdays;

	days = time / 86400;
	time -= (u32_t) days * 86400;

	/* day of the week, 1970-01-01 was a thursday */
	tm->week = (days + 4) % 7;

	year = 1970 + days / 365;
	days -= (year - 1970) * 365	+ LEAPS_THRU_END_OF(year - 1) - LEAPS_THRU_END_OF(1970 - 1);

	if(days < 0)
	{
		year -= 1;
		days += 365 + LEAP_YEAR(year);
	}
	tm->year = year;
	tm->day = days + 1;

	for(month = 1; month < 12; month++)
	{
		newdays = days - rtc_month_days(year, month);
		if(newdays < 0)
			break;
		days = newdays;
	}
	tm->mon = month;
	tm->day = days + 1;

	tm->hour = time / 3600;
	time -= tm->hour * 3600;
	tm->min = time / 60;
	tm->sec = time - tm->min * 60;
}

/*
 * convert gregorian date to seconds since 01-01-1970 00:00:00.
 */
u32_t time_to_rtc(struct xtime_t * tm)
{
	return xmktime(tm->year, tm->mon, tm->day, tm->hour, tm->min, tm->sec);
}

bool_t register_rtc(struct rtc_driver_t * drv)
{
	struct device_t * dev;

	if(!drv || !drv->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(drv->name);
	dev->type = DEVICE_TYPE_RTC;
	dev->driver = drv;

	if(!register_device(dev))
	{
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(drv->init)
		(drv->init)();

	return TRUE;
}

/*
 * unregister rtc driver
 */
bool_t unregister_rtc(struct rtc_driver_t * drv)
{
	struct device_t * dev;
	struct rtc_driver_t * driver;

	if(!drv || !drv->name)
		return FALSE;

	dev = search_device_with_type(drv->name, DEVICE_TYPE_RTC);
	if(!dev)
		return FALSE;

	driver = (struct rtc_driver_t *)(dev->driver);
	if(driver && driver->exit)
		(driver->exit)();

	if(!unregister_device(dev));
		return FALSE;

	free(dev->name);
	free(dev);
	return TRUE;
}
