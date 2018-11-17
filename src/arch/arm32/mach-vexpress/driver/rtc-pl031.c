/*
 * driver/rtc-pl031.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

/*
 * PrimeCell PL031 - Real-Time Clock Interface For ARM AMBA
 *
 * Example:
 *   "rtc-pl031@0x10017000": {
 *   }
 */

enum {
	RTC_DR		= 0x00,		/* Data read register */
	RTC_MR		= 0x04,		/* Match register */
	RTC_LR		= 0x08,		/* Data load register */
	RTC_CR		= 0x0c,		/* Control register */
	RTC_IMSC	= 0x10,		/* Interrupt mask and set register */
	RTC_RIS		= 0x14,		/* Raw interrupt status register */
	RTC_MIS		= 0x18,		/* Masked interrupt status register */
	RTC_ICR		= 0x1c,		/* Interrupt clear register */
};

#define LEAPS_THRU_END(y)	((y)/4 - (y)/100 + (y)/400)
#define LEAP_YEAR(year)		((!(year % 4) && (year % 100)) || !(year % 400))

struct rtc_pl031_pdata_t {
	virtual_addr_t virt;
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

static bool_t rtc_pl031_settime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_pl031_pdata_t * pdat = (struct rtc_pl031_pdata_t *)rtc->priv;

	write32(pdat->virt + RTC_LR, from_rtc_time(time));
	return TRUE;
}

static bool_t rtc_pl031_gettime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_pl031_pdata_t * pdat = (struct rtc_pl031_pdata_t *)rtc->priv;

	to_rtc_time(read32(pdat->virt + RTC_DR), time);
	return TRUE;
}

static struct device_t * rtc_pl031_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct rtc_pl031_pdata_t * pdat;
	struct rtc_t * rtc;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	u32_t id = (((read32(virt + 0xfec) & 0xff) << 24) |
				((read32(virt + 0xfe8) & 0xff) << 16) |
				((read32(virt + 0xfe4) & 0xff) <<  8) |
				((read32(virt + 0xfe0) & 0xff) <<  0));

	if(((id >> 12) & 0xff) != 0x41 || (id & 0xfff) != 0x031)
		return NULL;

	pdat = malloc(sizeof(struct rtc_pl031_pdata_t));
	if(!pdat)
		return NULL;

	rtc = malloc(sizeof(struct rtc_t));
	if(!rtc)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;

	rtc->name = alloc_device_name(dt_read_name(n), -1);
	rtc->settime = rtc_pl031_settime;
	rtc->gettime = rtc_pl031_gettime;
	rtc->priv = pdat;

	write32(pdat->virt + RTC_IMSC, 1);
	write32(pdat->virt + RTC_ICR, 1);
	write32(pdat->virt + RTC_CR, 1);

	if(!register_rtc(&dev, rtc))
	{
		free_device_name(rtc->name);
		free(rtc->priv);
		free(rtc);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void rtc_pl031_remove(struct device_t * dev)
{
	struct rtc_t * rtc = (struct rtc_t *)dev->priv;
	struct rtc_pl031_pdata_t * pdat = (struct rtc_pl031_pdata_t *)rtc->priv;

	if(rtc && unregister_rtc(rtc))
	{
		write32(pdat->virt + RTC_IMSC, 1);
		write32(pdat->virt + RTC_ICR, 1);

		free_device_name(rtc->name);
		free(rtc->priv);
		free(rtc);
	}
}

static void rtc_pl031_suspend(struct device_t * dev)
{
}

static void rtc_pl031_resume(struct device_t * dev)
{
}

static struct driver_t rtc_pl031 = {
	.name		= "rtc-pl031",
	.probe		= rtc_pl031_probe,
	.remove		= rtc_pl031_remove,
	.suspend	= rtc_pl031_suspend,
	.resume		= rtc_pl031_resume,
};

static __init void rtc_pl031_driver_init(void)
{
	register_driver(&rtc_pl031);
}

static __exit void rtc_pl031_driver_exit(void)
{
	unregister_driver(&rtc_pl031);
}

driver_initcall(rtc_pl031_driver_init);
driver_exitcall(rtc_pl031_driver_exit);
