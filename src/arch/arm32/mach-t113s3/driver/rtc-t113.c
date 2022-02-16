/*
 * driver/rtc-t113.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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
#include <clk/clk.h>
#include <rtc/rtc.h>

enum {
	RTC_LOSC_CTRL				= 0x000,
	RTC_LOSC_AUTO_SWT_STA		= 0x004,
	RTC_INTOSC_CLK_PRESCAL		= 0x008,
	RTC_RTC_DAY					= 0x010,
	RTC_RTC_HH_MM_SS			= 0x014,
	RTC_ALARM0_COUNTER			= 0x020,
	RTC_ALARM0_CUR_VLU			= 0x024,
	RTC_ALARM0_ENABLE			= 0x028,
	RTC_ALARM0_IRQ_EN			= 0x02c,
	RTC_ALARM0_IRQ_STA			= 0x030,
	RTC_ALARM_CONFIG			= 0x050,
	RTC_FANOUT_GATING			= 0x060,
	RTC_GP_DATA					= 0x100,
	RTC_FROOT_INFO_REG0			= 0x120,
	RTC_FROOT_INFO_REG1			= 0x124,
	RTC_DCXO_CTRL				= 0x160,
	RTC_RTC_VIO					= 0x190,
	RTC_IC_CHARA				= 0x1f0,
	RTC_VDDOFF_GATING_SOF		= 0x1f4,
	RTC_EFUSE_HV_PWRSWT_CTRL	= 0x204,
	RTC_SPI_CLK_CTRL_REG		= 0x310,
};

#define LEAPS_THRU_END(y)	((y) / 4 - (y) / 100 + (y) / 400)
#define LEAP_YEAR(year)		((!(year % 4) && (year % 100)) || !(year % 400))

struct rtc_t113_pdata_t {
	virtual_addr_t virt;
	char * clk;
};

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

static u32_t from_rtc_time(struct rtc_time_t * rt)
{
	int month = rt->month, year = rt->year;

	if (0 >= (int)(month -= 2))
	{
		month += 12;
		year -= 1;
	}
	return ((((u32_t)(year/4 - year/100 + year/400 + 367*month/12 + rt->day) + year*365 - 719499)*24 + rt->hour)*60 + rt->minute)*60 + rt->second;
}

static int rtc_t113_wait(struct rtc_t113_pdata_t * pdat, u32_t offset, u32_t mask, u32_t ms)
{
	ktime_t timeout = ktime_add_ms(ktime_get(), ms);
	u32_t val;

	do {
		val = read32(pdat->virt + offset);
		val &= mask;
		if(val != mask)
			return 1;
	} while(ktime_before(ktime_get(), timeout));
	return 0;
}

static bool_t rtc_t113_settime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_t113_pdata_t * pdat = (struct rtc_t113_pdata_t *)rtc->priv;
	u32_t d, t;

	t = (time->second << 0) | (time->minute << 8) | (time->hour << 16);
	d = from_rtc_time(time) / (24 * 3600);

	if(!rtc_t113_wait(pdat, RTC_LOSC_CTRL, 1 << 8, 50))
		return FALSE;
	mdelay(2);
	write32(pdat->virt + RTC_RTC_HH_MM_SS, t);
	if(!rtc_t113_wait(pdat, RTC_LOSC_CTRL, 1 << 8, 50))
		return FALSE;
	mdelay(2);

	if(!rtc_t113_wait(pdat, RTC_LOSC_CTRL, 1 << 7, 50))
		return FALSE;
	mdelay(2);
	write32(pdat->virt + RTC_RTC_DAY, d);
	if(!rtc_t113_wait(pdat, RTC_LOSC_CTRL, 1 << 7, 50))
		return FALSE;
	mdelay(2);

	return TRUE;
}

static bool_t rtc_t113_gettime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_t113_pdata_t * pdat = (struct rtc_t113_pdata_t *)rtc->priv;
	u32_t d, t, tmp;

	do {
		d = read32(pdat->virt + RTC_RTC_DAY);
		t = read32(pdat->virt + RTC_RTC_HH_MM_SS);
	} while((d != read32(pdat->virt + RTC_RTC_DAY)) || (t != read32(pdat->virt + RTC_RTC_HH_MM_SS)));

	tmp = ((t >> 0) & 0x3f) + ((t >> 8) & 0x3f) * 60 + ((t >> 16) & 0x1f) * 3600 + d * 24 * 3600;
	to_rtc_time(tmp, time);
	return TRUE;
}

static struct device_t * rtc_t113_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct rtc_t113_pdata_t * pdat;
	struct rtc_t * rtc;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	u32_t val;

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct rtc_t113_pdata_t));
	if(!pdat)
		return NULL;

	rtc = malloc(sizeof(struct rtc_t));
	if(!rtc)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);

	rtc->name = alloc_device_name(dt_read_name(n), -1);
	rtc->settime = rtc_t113_settime;
	rtc->gettime = rtc_t113_gettime;
	rtc->priv = pdat;

	clk_enable(pdat->clk);
	write32(pdat->virt + RTC_LOSC_CTRL, (1 << 14) | (1 << 4));
	val = read32(pdat->virt + RTC_LOSC_CTRL);
	val &= ~(0x3 << 0);
	val |= (0x16aa << 16);
	write32(pdat->virt + RTC_LOSC_CTRL, val);

	if(!(dev = register_rtc(rtc, drv)))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(rtc->name);
		free(rtc->priv);
		free(rtc);
		return NULL;
	}
	return dev;
}

static void rtc_t113_remove(struct device_t * dev)
{
	struct rtc_t * rtc = (struct rtc_t *)dev->priv;
	struct rtc_t113_pdata_t * pdat = (struct rtc_t113_pdata_t *)rtc->priv;

	if(rtc)
	{
		unregister_rtc(rtc);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(rtc->name);
		free(rtc->priv);
		free(rtc);
	}
}

static void rtc_t113_suspend(struct device_t * dev)
{
}

static void rtc_t113_resume(struct device_t * dev)
{
}

static struct driver_t rtc_t113 = {
	.name		= "rtc-t113",
	.probe		= rtc_t113_probe,
	.remove		= rtc_t113_remove,
	.suspend	= rtc_t113_suspend,
	.resume		= rtc_t113_resume,
};

static __init void rtc_t113_driver_init(void)
{
	register_driver(&rtc_t113);
}

static __exit void rtc_t113_driver_exit(void)
{
	unregister_driver(&rtc_t113);
}

driver_initcall(rtc_t113_driver_init);
driver_exitcall(rtc_t113_driver_exit);
