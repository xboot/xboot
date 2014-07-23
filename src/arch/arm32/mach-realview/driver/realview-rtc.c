/*
 * driver/realview_rtc.c
 *
 * realview rtc drivers, the primecell pl031 real time clock.
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
#include <realview-rtc.h>

#define LEAPS_THRU_END_OF(y)	((y)/4 - (y)/100 + (y)/400)
#define LEAP_YEAR(year)			((!(year % 4) && (year % 100)) || !(year % 400))

static const u8_t rtc_days_in_month[13] = {
	0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static u32_t rtc_month_days(u32_t year, u32_t month)
{
	return rtc_days_in_month[month] + (LEAP_YEAR(year) && month == 2);
}

static bool_t rtc_valid_time(struct rtc_time_t * rt)
{
	if (rt->year < 1970
		|| (rt->mon) > 12
		|| rt->day < 1
		|| rt->day > rtc_month_days(rt->year, rt->mon)
		|| (rt->hour) >= 24
		|| (rt->min) >= 60
		|| (rt->sec) >= 60)
		return FALSE;

	return TRUE;
}

static void rtc_to_time(unsigned long time, struct rtc_time_t * rt)
{
	u32_t month, year;
	s32_t days;
	s32_t newdays;

	days = time / 86400;
	time -= (u32_t) days * 86400;

	rt->week = (days + 4) % 7;

	year = 1970 + days / 365;
	days -= (year - 1970) * 365	+ LEAPS_THRU_END_OF(year - 1) - LEAPS_THRU_END_OF(1970 - 1);

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
	rt->mon = month;
	rt->day = days + 1;

	rt->hour = time / 3600;
	time -= rt->hour * 3600;
	rt->min = time / 60;
	rt->sec = time - rt->min * 60;
}

static u32_t time_to_rtc(struct rtc_time_t * rt)
{
	u32_t mon = rt->mon, year = rt->year;

	if (0 >= (int)(mon -= 2))
	{
		mon += 12;
		year -= 1;
	}

	return ((((u32_t)(year/4 - year/100 + year/400 + 367*mon/12 + rt->day) + year*365 - 719499)*24 + rt->hour)*60 + rt->min)*60 + rt->sec;
}

static void rtc_init(struct rtc_t * rtc)
{
}

static void rtc_exit(struct rtc_t * rtc)
{
}

static bool_t rtc_settime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct resource_t * res = (struct resource_t *)rtc->priv;
	struct realview_rtc_data_t * dat = (struct realview_rtc_data_t *)res->data;

	if(rtc_valid_time(time))
	{
		writel(dat->regbase + REALVIEW_RTC_OFFSET_LR, time_to_rtc(time));
		return TRUE;
	}
	return FALSE;
}

static bool_t rtc_gettime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct resource_t * res = (struct resource_t *)rtc->priv;
	struct realview_rtc_data_t * dat = (struct realview_rtc_data_t *)res->data;

	rtc_to_time(readl(dat->regbase + REALVIEW_RTC_OFFSET_DR), time);
	return TRUE;
}

static void rtc_suspend(struct rtc_t * rtc)
{
}

static void rtc_resume(struct rtc_t * rtc)
{
}

static bool_t realview_register_rtc(struct resource_t * res)
{
	struct rtc_t * rtc;
	char name[64];

	rtc = malloc(sizeof(struct rtc_t));
	if(!rtc)
		return FALSE;

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	rtc->name = strdup(name);
	rtc->init = rtc_init;
	rtc->exit = rtc_exit;
	rtc->settime = rtc_settime,
	rtc->gettime = rtc_gettime,
	rtc->suspend = rtc_suspend,
	rtc->resume	= rtc_resume,
	rtc->priv = res;

	if(register_rtc(rtc))
		return TRUE;

	free(rtc->name);
	free(rtc);
	return FALSE;
}

static bool_t realview_unregister_rtc(struct resource_t * res)
{
	struct rtc_t * rtc;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	rtc = search_rtc(name);
	if(!rtc)
		return FALSE;

	if(!unregister_rtc(rtc))
		return FALSE;

	free(rtc->name);
	free(rtc);
	return TRUE;
}

static __init void realview_rtc_init(void)
{
	resource_for_each_with_name("realview-rtc", realview_register_rtc);
}

static __exit void realview_rtc_exit(void)
{
	resource_for_each_with_name("realview-rtc", realview_unregister_rtc);
}

device_initcall(realview_rtc_init);
device_exitcall(realview_rtc_exit);
