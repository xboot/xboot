/*
 * driver/pl031-rtc.c
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

#include <pl031-rtc.h>

#define RTC_DR		(0x00)	/* Data read register */
#define RTC_MR		(0x04)	/* Match register */
#define RTC_LR		(0x08)	/* Data load register */
#define RTC_CR		(0x0c)	/* Control register */
#define RTC_IMSC	(0x10)	/* Interrupt mask and set register */
#define RTC_RIS		(0x14)	/* Raw interrupt status register */
#define RTC_MIS		(0x18)	/* Masked interrupt status register */
#define RTC_ICR		(0x1c)	/* Interrupt clear register */

struct pl031_rtc_private_data_t {
	virtual_addr_t regbase;
};

#define LEAPS_THRU_END(y)	((y)/4 - (y)/100 + (y)/400)
#define LEAP_YEAR(year)		((!(year % 4) && (year % 100)) || !(year % 400))

static int rtc_month_days(int year, int month)
{
	const unsigned char rtc_days_in_month[13] = {
		0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};
	return rtc_days_in_month[month] + (LEAP_YEAR(year) && (month == 2));
}

static bool_t rtc_valid_time(struct rtc_time_t * rt)
{
	if((rt->year < 1970)
		|| ((rt->mon) > 12)
		|| (rt->day < 1)
		|| (rt->day > rtc_month_days(rt->year, rt->mon))
		|| ((rt->hour) >= 24)
		|| ((rt->min) >= 60)
		|| ((rt->sec) >= 60))
		return FALSE;

	return TRUE;
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
	rt->mon = month;
	rt->day = days + 1;

	rt->hour = time / 3600;
	time -= rt->hour * 3600;
	rt->min = time / 60;
	rt->sec = time - rt->min * 60;
}

static u32_t from_rtc_time(struct rtc_time_t * rt)
{
	int mon = rt->mon, year = rt->year;

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
	struct pl031_rtc_private_data_t * dat = (struct pl031_rtc_private_data_t *)rtc->priv;

	if(!rtc_valid_time(time))
		return FALSE;
	write32(dat->regbase + RTC_LR, from_rtc_time(time));
	return TRUE;
}

static bool_t rtc_gettime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct pl031_rtc_private_data_t * dat = (struct pl031_rtc_private_data_t *)rtc->priv;

	to_rtc_time(read32(dat->regbase + RTC_DR), time);
	return TRUE;
}

static void rtc_suspend(struct rtc_t * rtc)
{
}

static void rtc_resume(struct rtc_t * rtc)
{
}

static bool_t pl031_register_rtc(struct resource_t * res)
{
	struct pl031_rtc_data_t * rdat = (struct pl031_rtc_data_t *)res->data;
	struct pl031_rtc_private_data_t * dat;
	struct rtc_t * rtc;
	char name[64];

	dat = malloc(sizeof(struct pl031_rtc_private_data_t));
	if(!dat)
		return FALSE;

	rtc = malloc(sizeof(struct rtc_t));
	if(!rtc)
	{
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	dat->regbase = phys_to_virt(rdat->regbase);
	rtc->name = strdup(name);
	rtc->init = rtc_init;
	rtc->exit = rtc_exit;
	rtc->settime = rtc_settime,
	rtc->gettime = rtc_gettime,
	rtc->suspend = rtc_suspend,
	rtc->resume	= rtc_resume,
	rtc->priv = dat;

	if(register_rtc(rtc))
		return TRUE;

	free(rtc->priv);
	free(rtc->name);
	free(rtc);
	return FALSE;
}

static bool_t pl031_unregister_rtc(struct resource_t * res)
{
	struct rtc_t * rtc;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	rtc = search_rtc(name);
	if(!rtc)
		return FALSE;

	if(!unregister_rtc(rtc))
		return FALSE;

	free(rtc->priv);
	free(rtc->name);
	free(rtc);
	return TRUE;
}

static __init void pl031_rtc_init(void)
{
	resource_for_each_with_name("pl031-rtc", pl031_register_rtc);
}

static __exit void pl031_rtc_exit(void)
{
	resource_for_each_with_name("pl031-rtc", pl031_unregister_rtc);
}

device_initcall(pl031_rtc_init);
device_exitcall(pl031_rtc_exit);
