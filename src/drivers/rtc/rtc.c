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
#include <xboot/chrdev.h>
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
bool_t rtc_valid_time(struct time * tm)
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
void rtc_to_time(unsigned long time, struct time *tm)
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
u32_t time_to_rtc(struct time * tm)
{
	return xmktime(tm->year, tm->mon, tm->day, tm->hour, tm->min, tm->sec);
}

/*
 * rtc open
 */
static int rtc_open(struct chrdev * dev)
{
	return 0;
}

/*
 * rtc read
 */
static ssize_t rtc_read(struct chrdev * dev, u8_t * buf, size_t count)
{
	const char * week_days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	struct rtc_driver * drv = (struct rtc_driver *)(dev->driver);
	struct time time;
	char tmp[64];
	int offset = 0;
	int len;

	if(drv->get_time)
	{
		if(drv->get_time(&time))
		{
			len = sprintf(tmp, "%04lu-%02lu-%02lu %s %02lu:%02lu:%02lu\r\n", (u32_t)time.year, (u32_t)time.mon, (u32_t)time.day, week_days[time.week], (u32_t)time.hour, (u32_t)time.min, (u32_t)time.sec);
			len -= offset;

			if(len < 0)
				len = 0;
			if(len > count)
				len = count;

			memcpy(buf, &tmp[offset], len);
			return len;
		}
	}

	return 0;
}

/*
 * rtc write.
 */
static ssize_t rtc_write(struct chrdev * dev, const u8_t * buf, size_t count)
{
	return 0;
}

/*
 * rtc ioctl
 */
static int rtc_ioctl(struct chrdev * dev, int cmd, void * arg)
{
	struct rtc_driver * drv = (struct rtc_driver *)(dev->driver);
	struct time * time;

	switch(cmd)
	{
	case IOCTL_SET_RTC_TIME:
		time = (struct time *)arg;
		if(drv->set_time && drv->set_time(time))
			return 0;
		break;

	case IOCTL_GET_RTC_TIME:
		time = (struct time *)arg;
		if(drv->get_time && drv->get_time(time))
			return 0;
		break;

	case IOCTL_SET_RTC_ALARM:
		time = (struct time *)arg;
		if(drv->set_alarm && drv->set_alarm(time))
			return 0;
		break;

	case IOCTL_GET_RTC_ALARM:
		time = (struct time *)arg;
		if(drv->get_alarm && drv->get_alarm(time))
			return 0;
		break;

	case IOCTL_ENABLE_RTC_ALARM:
		if(drv->alarm_enable && drv->alarm_enable(TRUE))
			return 0;
		break;

	case IOCTL_DISABLE_RTC_ALARM:
		if(drv->alarm_enable && drv->alarm_enable(FALSE))
			return 0;
		break;

	default:
		break;
	}

	return -1;
}

/*
 * rtc close
 */
static int rtc_close(struct chrdev * dev)
{
	return 0;
}

/*
 * register rtc driver, return true is successed.
 */
bool_t register_rtc(struct rtc_driver * drv)
{
	struct chrdev * dev;

	if(!drv || !drv->name || !drv->get_time)
		return FALSE;

	dev = malloc(sizeof(struct chrdev));
	if(!dev)
		return FALSE;

	dev->name		= drv->name;
	dev->type		= CHR_DEV_RTC;
	dev->open 		= rtc_open;
	dev->read 		= rtc_read;
	dev->write 		= rtc_write;
	dev->ioctl 		= rtc_ioctl;
	dev->close		= rtc_close;
	dev->driver 	= drv;

	if(!register_chrdev(dev))
	{
		free(dev);
		return FALSE;
	}

	if(search_chrdev_with_type(dev->name, CHR_DEV_RTC) == NULL)
	{
		unregister_chrdev(dev->name);
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
bool_t unregister_rtc(struct rtc_driver * drv)
{
	struct chrdev * dev;
	struct rtc_driver * driver;

	if(!drv || !drv->name)
		return FALSE;

	dev = search_chrdev_with_type(drv->name, CHR_DEV_RTC);
	if(!dev)
		return FALSE;

	driver = (struct rtc_driver *)(dev->driver);
	if(driver && driver->exit)
		(driver->exit)();

	if(!unregister_chrdev(dev->name));
		return FALSE;

	free(dev);
	return TRUE;
}
