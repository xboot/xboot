/*
 * driver/rtc/rtc.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <rtc/rtc.h>

static inline int rtc_month_days(unsigned int year, unsigned int month)
{
	const int rtc_days_in_month[13] = {
		0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};
	if(month > 12)
		month = 0;
	return rtc_days_in_month[month] + (((!(year % 4) && (year % 100)) || !(year % 400)) && (month == 2)) ? 1 : 0;
}

static int rtc_time_is_valid(struct rtc_time_t * time)
{
	if((!time) || (time->year < 1970)
		|| (time->month < 1)
		|| (time->month > 12)
		|| (time->day < 1)
		|| (time->day > rtc_month_days(time->year, time->month))
		|| (time->hour >= 24)
		|| (time->minute >= 60)
		|| (time->second >= 60))
		return 0;
	return 1;
}

static ssize_t rtc_time_read(struct kobj_t * kobj, void * buf, size_t size)
{
	struct rtc_t * rtc = (struct rtc_t *)kobj->priv;
	struct rtc_time_t time;

	if(rtc && rtc->gettime)
		rtc->gettime(rtc, &time);
	return sprintf(buf, "%04u-%02u-%02u %02u:%02u:%02u %01u", (u32_t)time.year, (u32_t)time.month, (u32_t)time.day, (u32_t)time.hour, (u32_t)time.minute, (u32_t)time.second, (u32_t)time.week);
}

static ssize_t rtc_time_write(struct kobj_t * kobj, void * buf, size_t size)
{
	struct rtc_t * rtc = (struct rtc_t *)kobj->priv;
	struct rtc_time_t time;
	char * p = buf, * r, * v;
	int index = 0;

	while((r = strsep(&p, "-: ")) != NULL)
	{
		v = strim(r);
		if((*v != '\0'))
		{
			switch(index++)
			{
			case 0:
				time.year = strtoul(v, NULL, 0);
				break;
			case 1:
				time.month = strtoul(v, NULL, 0);
				break;
			case 2:
				time.day = strtoul(v, NULL, 0);
				break;
			case 3:
				time.hour = strtoul(v, NULL, 0);
				break;
			case 4:
				time.minute = strtoul(v, NULL, 0);
				break;
			case 5:
				time.second = strtoul(v, NULL, 0);
				break;
			case 6:
				time.week = strtoul(v, NULL, 0);
				break;
			default:
				break;
			}
		}
	}
	if(index >= 7)
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

struct device_t * register_rtc(struct rtc_t * rtc, struct driver_t * drv)
{
	struct device_t * dev;
	struct rtc_time_t time;

	if(!rtc || !rtc->name)
		return NULL;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return NULL;

	dev->name = strdup(rtc->name);
	dev->type = DEVICE_TYPE_RTC;
	dev->driver = drv;
	dev->priv = rtc;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "time", rtc_time_read, rtc_time_write, rtc);

	if(rtc_gettime(rtc, &time) && !rtc_time_is_valid(&time))
	{
		time.second = 0;
		time.minute = 0;
		time.hour = 0;
		time.week = 1;
		time.day = 1;
		time.month = 1;
		time.year = 2018;
		rtc_settime(rtc, &time);
	}

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return NULL;
	}
	return dev;
}

void unregister_rtc(struct rtc_t * rtc)
{
	struct device_t * dev;

	if(rtc && rtc->name)
	{
		dev = search_device(rtc->name, DEVICE_TYPE_RTC);
		if(dev && unregister_device(dev))
		{
			kobj_remove_self(dev->kobj);
			free(dev->name);
			free(dev);
		}
	}
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
