/*
 * kernel/time/xtime.c
 *
 * some function copyed from linux kernel.
 * the calibrate_delay function be write by Linus Torvalds
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <macros.h>
#include <vsprintf.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/chrdev.h>
#include <xboot/proc.h>
#include <xboot/initcall.h>
#include <rtc/rtc.h>
#include <time/tick.h>
#include <time/timer.h>
#include <time/xtime.h>

/*
 * the soft time of kernel.
 */
static struct time xtime = {0, 0, 0, 1, 4, 1, 1970};

/*
 * xtime's update timer.
 */
static struct timer_list xtime_timer;

/*
 * the hardware rtc.
 */
static struct rtc_driver * rtc = NULL;

/*
 * xtime's timer function.
 */
static void xtime_timer_function(x_u32 data)
{
	static x_u32 times = 0;

	/* every 512s, sync xtime with hardware rtc */
	if(++times > 512)
	{
		times = 0;

		/* sync xtime with hardware rtc */
		if(rtc)
		{
			if(rtc->get_time)
				rtc->get_time(&xtime);
		}
	}

	/* update software clock */
	else
	{
		if(++xtime.sec > 59)
		{
			xtime.sec = 0;
			if(++xtime.min > 59)
			{
				xtime.min = 0;
				if(++xtime.hour > 23)
				{
					xtime.hour = 0;

					if(++xtime.week > 7)
						xtime.week = 1;

					if(++xtime.day > rtc_month_days(xtime.year, xtime.mon))
					{
						xtime.day = 1;
						if(++xtime.mon > 12)
						{
							xtime.mon = 1;
							++xtime.year;
						}
					}
				}
			}
		}
	}

	/* mod timer for next one second */
	mod_timer(&xtime_timer, jiffies + get_system_hz());
}

/*
 * do system xtime.
 */
void do_system_xtime(void)
{
	struct chrdev * dev;

	/* search hardware rtc for sync xtime */
	dev = search_chrdev_with_type(CONFIG_HARDWARE_RTC_NAME, CHR_DEV_RTC);
	if(dev)
	{
		rtc = (struct rtc_driver *)(dev->driver);

		/* sync xtime, first */
		if(rtc && rtc->get_time)
		{
			rtc->get_time(&xtime);
		}
	}
	else
	{
		LOG_E("the hardware rtc \"%s\" not found", CONFIG_HARDWARE_RTC_NAME);
	}

	/* setup timer for update xtime */
	setup_timer(&xtime_timer, xtime_timer_function, (x_u32)(&xtime));

	/* mod timer for one second */
	mod_timer(&xtime_timer, jiffies + get_system_hz());
}

/*
 * converts gregorian date to seconds since 1970-01-01 00:00:00.
 * assumes input in normal date format, i.e. 1980-12-31 23:59:59
 * => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
 *
 * WARNING: this function will overflow on 2106-02-07 06:28:16 on
 * machines where x_u32 is 32-bit!
 */
x_u32 mktime(const x_u32 year0, const x_u32 mon0, const x_u32 day, const x_u32 hour, const x_u32 min, const x_u32 sec)
{
	x_u32 mon = mon0, year = year0;

	/* 1..12 -> 11,12,1..10 */
	if (0 >= (x_s32)(mon -= 2))
	{
		/* Puts Feb last since it has leap day */
		mon += 12;
		year -= 1;
	}

	return ((((x_u32)(year/4 - year/100 + year/400 + 367*mon/12 + day) + year*365 - 719499)*24 + hour)*60 + min)*60 + sec;
}

/*
 * gregorian day of week
 *
 * 0 ~ 6 : Sun, Mon, Tue, Wed, Thu, Fri, Sat
 */
x_u8 mkweek(x_u32 year, x_u32 mon, x_u32 day)
{
	if( (mon == 1) || (mon == 2) )
	{
		mon += 12;
		year--;
	}

	return (day + 2 * mon + 3 * (mon + 1) / 5 + year + year / 4 - year / 100 + year / 400 + 1) % 7;
}

/*
 * get time stamp
 */
x_u32 get_time_stamp(void)
{
	x_u32 year, mon, day, hour, min, sec;

	year = xtime.year;
	mon = xtime.mon;
	day = xtime.day;
	hour = xtime.hour;
	min = xtime.min;
	sec = xtime.sec;

	return mktime(year, mon, day, hour, min, sec);
}

/*
 * xtime proc interface
 */
static x_s32 xtime_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	x_s8 tmp[64];
	x_s32 len;
	const char * week_days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

	len = sprintf(tmp, (const x_s8 *)"%04lu-%02lu-%02lu %s %02lu:%02lu:%02lu", (x_u32)xtime.year, (x_u32)xtime.mon, (x_u32)xtime.day, week_days[xtime.week], (x_u32)xtime.hour, (x_u32)xtime.min, (x_u32)xtime.sec);
	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, &tmp[offset], len);

	return len;
}

static struct proc xtime_proc = {
	.name	= "xtime",
	.read	= xtime_proc_read,
};

static __init void xtime_pure_sync_init(void)
{
	/* register xtime proc interface */
	proc_register(&xtime_proc);
}

static __exit void xtime_pure_sync_exit(void)
{
	/* unregister xtime proc interface */
	proc_unregister(&xtime_proc);
}

module_init(xtime_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(xtime_pure_sync_exit, LEVEL_PURE_SYNC);
