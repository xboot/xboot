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
#include <rtc/rtc.h>

static void rtc_suspend(struct device_t * dev)
{
	struct rtc_t * rtc;

	if(!dev || dev->type != DEVICE_TYPE_RTC)
		return;

	rtc = (struct rtc_t *)(dev->driver);
	if(!rtc)
		return;

	if(rtc->suspend)
		rtc->suspend(rtc);
}

static void rtc_resume(struct device_t * dev)
{
	struct rtc_t * rtc;

	if(!dev || dev->type != DEVICE_TYPE_RTC)
		return;

	rtc = (struct rtc_t *)(dev->driver);
	if(!rtc)
		return;

	if(rtc->resume)
		rtc->resume(rtc);
}

static ssize_t rtc_time_read(struct kobj_t * kobj, void * buf, size_t size)
{
	struct rtc_t * rtc = (struct rtc_t *)kobj->priv;
	struct rtc_time_t time;

	if(rtc && rtc->gettime)
		rtc->gettime(rtc, &time);

	return sprintf(buf, "%04u-%02u-%02u %01u %02u:%02u:%02u", (u32_t)time.year, (u32_t)time.mon, (u32_t)time.day, (u32_t)time.week, (u32_t)time.hour, (u32_t)time.min, (u32_t)time.sec);
}

static ssize_t rtc_time_write(struct kobj_t * kobj, void * buf, size_t size)
{
	struct rtc_t * rtc = (struct rtc_t *)kobj->priv;
	struct rtc_time_t time;

	if(sscanf(buf, "%04u-%02u-%02u %01u %02u:%02u:%02u", &time.year, &time.mon, &time.day, &time.week, &time.hour, &time.min, &time.sec) == 7)
	{
		if(rtc && rtc->settime)
			rtc->settime(rtc, &time);
	}

	return size;
}

struct rtc_t * search_rtc(const char * name)
{
	struct device_t * dev;

	dev = search_device_with_type(name, DEVICE_TYPE_RTC);
	if(!dev)
		return NULL;

	return (struct rtc_t *)dev->driver;
}

bool_t register_rtc(struct rtc_t * rtc)
{
	struct device_t * dev;

	if(!rtc || !rtc->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(rtc->name);
	dev->type = DEVICE_TYPE_RTC;
	dev->suspend = rtc_suspend;
	dev->resume = rtc_resume;
	dev->driver = rtc;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "time", rtc_time_read, rtc_time_write, rtc);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(rtc->init)
		(rtc->init)(rtc);

	return TRUE;
}

bool_t unregister_rtc(struct rtc_t * rtc)
{
	struct device_t * dev;
	struct rtc_t * driver;

	if(!rtc || !rtc->name)
		return FALSE;

	dev = search_device_with_type(rtc->name, DEVICE_TYPE_RTC);
	if(!dev)
		return FALSE;

	driver = (struct rtc_t *)(dev->driver);
	if(driver && driver->exit)
		(driver->exit)(rtc);

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}
