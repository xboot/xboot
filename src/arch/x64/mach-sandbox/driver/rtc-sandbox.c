/*
 * driver/rtc-sandbox.c
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
#include <sandbox.h>

#define LEAPS_THRU_END(y)	((y) / 4 - (y) / 100 + (y) / 400)
#define LEAP_YEAR(year)		((!(year % 4) && (year % 100)) || !(year % 400))

static int rtc_month_days(int year, int month)
{
	const unsigned char rtc_days_in_month[13] = {
		0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};
	return rtc_days_in_month[month] + (LEAP_YEAR(year) && (month == 2));
}

static void to_rtc_time(u32_t time, struct rtc_time_t * rt)
{
	u32_t month, year;
	int days;
	int newdays;

	days = time / 86400;
	time -= (u32_t)days * 86400;

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

static bool_t rtc_sandbox_settime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	return FALSE;
}

static bool_t rtc_sandbox_gettime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	to_rtc_time((u32_t)(sandbox_realtime() / 1000000000ULL), time);
	return TRUE;
}

static struct device_t * rtc_sandbox_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct rtc_t * rtc;
	struct device_t * dev;

	rtc = malloc(sizeof(struct rtc_t));
	if(!rtc)
		return NULL;

	rtc->name = alloc_device_name(dt_read_name(n), -1);
	rtc->settime = rtc_sandbox_settime;
	rtc->gettime = rtc_sandbox_gettime;
	rtc->priv = NULL;

	if(!(dev = register_rtc(rtc, drv)))
	{
		free_device_name(rtc->name);
		free(rtc->priv);
		free(rtc);
		return NULL;
	}
	return dev;
}

static void rtc_sandbox_remove(struct device_t * dev)
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

static void rtc_sandbox_suspend(struct device_t * dev)
{
}

static void rtc_sandbox_resume(struct device_t * dev)
{
}

static struct driver_t rtc_sandbox = {
	.name		= "rtc-sandbox",
	.probe		= rtc_sandbox_probe,
	.remove		= rtc_sandbox_remove,
	.suspend	= rtc_sandbox_suspend,
	.resume		= rtc_sandbox_resume,
};

static __init void rtc_sandbox_driver_init(void)
{
	register_driver(&rtc_sandbox);
}

static __exit void rtc_sandbox_driver_exit(void)
{
	unregister_driver(&rtc_sandbox);
}

driver_initcall(rtc_sandbox_driver_init);
driver_exitcall(rtc_sandbox_driver_exit);
