/*
 * driver/rtc/rtc-soft.c
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

#define LEAPS_THRU_END(y)	((y) / 4 - (y) / 100 + (y) / 400)
#define LEAP_YEAR(year)		((!(year % 4) && (year % 100)) || !(year % 400))

struct rtc_soft_pdata_t {
	int64_t adjust;
};

static int rtc_month_days(int year, int month)
{
	const unsigned char rtc_days_in_month[13] = {
		0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};
	return rtc_days_in_month[month] + (LEAP_YEAR(year) && (month == 2));
}

static void secs_to_rtc_time(uint32_t time, struct rtc_time_t * rt)
{
	uint32_t month, year;
	int days;
	int newdays;

	days = time / 86400;
	time -= (uint32_t)days * 86400;

	rt->week = (days + 4) % 7;
	year = 1970 + days / 365;
	days -= (year - 1970) * 365	+ LEAPS_THRU_END(year - 1) - LEAPS_THRU_END(1970 - 1);

	if(days < 0)
	{
		year -= 1;
		days += 365 + LEAP_YEAR(year);
	}
	rt->year = year;
	rt->day = days + 1;

	for(month = 1; month < 12; month++)
	{
		newdays = days - rtc_month_days(year, month);
		if(newdays < 0)
			break;
		days = newdays;
	}
	rt->month = month;
	rt->day = days + 1;
	rt->hour = time / 3600;
	time -= rt->hour * 3600;
	rt->minute = time / 60;
	rt->second = time - rt->minute * 60;
}

static uint32_t rtc_time_to_secs(struct rtc_time_t * rt)
{
	int month = rt->month, year = rt->year;

	if(0 >= (int)(month -= 2))
	{
		month += 12;
		year -= 1;
	}
	return ((((uint32_t)(year / 4 - year / 100 + year / 400 + 367 * month / 12 + rt->day) + year * 365 - 719499) * 24 + rt->hour) * 60 + rt->minute) * 60 + rt->second;
}

static bool_t rtc_soft_settime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_soft_pdata_t * pdat = (struct rtc_soft_pdata_t *)rtc->priv;

	pdat->adjust = (int64_t)rtc_time_to_secs(time) * 1000000000ULL - ktime_to_ns(ktime_get());
	return TRUE;
}

static bool_t rtc_soft_gettime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_soft_pdata_t * pdat = (struct rtc_soft_pdata_t *)rtc->priv;

	secs_to_rtc_time((uint32_t)((ktime_to_ns(ktime_get()) + pdat->adjust) / 1000000000ULL), time);
	return TRUE;
}

static struct device_t * rtc_soft_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct rtc_soft_pdata_t * pdat;
	struct rtc_t * rtc;
	struct device_t * dev;

	pdat = malloc(sizeof(struct rtc_soft_pdata_t));
	if(!pdat)
		return NULL;

	rtc = malloc(sizeof(struct rtc_t));
	if(!rtc)
	{
		free(pdat);
		return NULL;
	}

	pdat->adjust = (int64_t)dt_read_long(n, "adjust", 0);

	rtc->name = alloc_device_name(dt_read_name(n), -1);
	rtc->settime = rtc_soft_settime;
	rtc->gettime = rtc_soft_gettime;
	rtc->priv = pdat;

	if(!(dev = register_rtc(rtc, drv)))
	{
		free_device_name(rtc->name);
		free(rtc->priv);
		free(rtc);
		return NULL;
	}
	return dev;
}

static void rtc_soft_remove(struct device_t * dev)
{
	struct rtc_t * rtc = (struct rtc_t *)dev->priv;

	if(rtc)
	{
		unregister_rtc(rtc);
		free_device_name(rtc->name);
		free(rtc->priv);
		free(rtc);
	}
}

static void rtc_soft_suspend(struct device_t * dev)
{
}

static void rtc_soft_resume(struct device_t * dev)
{
}

static struct driver_t rtc_soft = {
	.name		= "rtc-soft",
	.probe		= rtc_soft_probe,
	.remove		= rtc_soft_remove,
	.suspend	= rtc_soft_suspend,
	.resume		= rtc_soft_resume,
};

static __init void rtc_soft_driver_init(void)
{
	register_driver(&rtc_soft);
}

static __exit void rtc_soft_driver_exit(void)
{
	unregister_driver(&rtc_soft);
}

driver_initcall(rtc_soft_driver_init);
driver_exitcall(rtc_soft_driver_exit);
