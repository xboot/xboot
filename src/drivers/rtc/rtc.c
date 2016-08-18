/*
 * drivers/rtc/rtc.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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

#include <rtc/rtc.h>

static ssize_t rtc_time_read(struct kobj_t * kobj, void * buf, size_t size)
{
	struct rtc_t * rtc = (struct rtc_t *)kobj->priv;
	struct rtc_time_t time;

	if(rtc && rtc->gettime)
		rtc->gettime(rtc, &time);

	return sprintf(buf, "%04u-%02u-%02u %01u %02u:%02u:%02u", (u32_t)time.year, (u32_t)time.month, (u32_t)time.day, (u32_t)time.week, (u32_t)time.hour, (u32_t)time.minute, (u32_t)time.second);
}

static ssize_t rtc_time_write(struct kobj_t * kobj, void * buf, size_t size)
{
	struct rtc_t * rtc = (struct rtc_t *)kobj->priv;
	struct rtc_time_t time;

	if(sscanf(buf, "%04u-%02u-%02u %01u %02u:%02u:%02u", &time.year, &time.month, &time.day, &time.week, &time.hour, &time.minute, &time.second) == 7)
	{
		if(rtc && rtc->settime)
			rtc->settime(rtc, &time);
	}

	return size;
}

struct rtc_t * search_rtc(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_RTC);
	if(!dev)
		return NULL;

	return (struct rtc_t *)dev->priv;
}

struct rtc_t * search_first_rtc(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_RTC);
	if(!dev)
		return NULL;

	return (struct rtc_t *)dev->priv;
}

bool_t register_rtc(struct device_t ** device, struct rtc_t * rtc)
{
	struct device_t * dev;

	if(!rtc || !rtc->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(rtc->name);
	dev->type = DEVICE_TYPE_RTC;
	dev->priv = rtc;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "time", rtc_time_read, rtc_time_write, rtc);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_rtc(struct rtc_t * rtc)
{
	struct device_t * dev;

	if(!rtc || !rtc->name)
		return FALSE;

	dev = search_device(rtc->name, DEVICE_TYPE_RTC);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

static int rtc_month_days(int year, int month)
{
	const unsigned char rtc_days_in_month[13] = {
		0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};
	return rtc_days_in_month[month] + (((!(year % 4) && (year % 100)) || !(year % 400)) && (month == 2));
}

static int rtc_time_is_valid(struct rtc_time_t * time)
{
	if((!time) || (time->year < 1970)
		|| ((time->month) > 12)
		|| (time->day < 1)
		|| (time->day > rtc_month_days(time->year, time->month))
		|| ((time->hour) >= 24)
		|| ((time->minute) >= 60)
		|| ((time->second) >= 60))
		return 0;
	return 1;
}

bool_t rtc_settime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	if(rtc && rtc->settime && rtc_time_is_valid(time))
		return rtc->settime(rtc, time);
	return FALSE;
}

bool_t rtc_gettime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	if(rtc && rtc->gettime)
		return rtc->gettime(rtc, time);
	return FALSE;
}
