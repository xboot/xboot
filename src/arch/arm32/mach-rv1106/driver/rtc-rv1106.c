/*
 * driver/rtc-rv1106.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
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
	RTC_SET_SECONDS		= 0x00,
	RTC_SET_MINUTES		= 0x04,
	RTC_SET_HOURS		= 0x08,
	RTC_SET_DAYS		= 0x0c,
	RTC_SET_MONTHS		= 0x10,
	RTC_SET_YEARL		= 0x14,
	RTC_SET_YEARH		= 0x18,
	RTC_SET_WEEKS		= 0x1c,
	RTC_ALARM_SECONDS	= 0x20,
	RTC_ALARM_MINUTES	= 0x24,
	RTC_ALARM_HOURS		= 0x28,
	RTC_ALARM_DAYS		= 0x2c,
	RTC_ALARM_MONTHS	= 0x30,
	RTC_ALARM_YEARL		= 0x34,
	RTC_ALARM_YEARH		= 0x38,
	RTC_CTRL			= 0x3C,
	RTC_STATUS0			= 0x40,
	RTC_STATUS1			= 0x44,
	RTC_INT0_EN			= 0x48,
	RTC_INT1_EN			= 0x4c,
	RTC_MSEC_CTRL		= 0x50,
	RTC_MSEC_CNT		= 0x54,
	RTC_COMP_H			= 0x58,
	RTC_COMP_D			= 0x5c,
	RTC_COMP_M			= 0x60,
	RTC_ANALOG_CTRL		= 0x64,
	RTC_ANALOG_TEST		= 0x68,
	RTC_LDO_CTRL		= 0x6c,
	RTC_XO_TRIM0		= 0x70,
	RTC_XO_TRIM1		= 0x74,
	RTC_VPTAT_TRIM		= 0x78,
	RTC_ANALOG_EN		= 0x7c,
	RTC_CLK32K_TEST		= 0x80,
	RTC_TEST_ST			= 0x84,
	RTC_TEST_LEN		= 0x88,
	RTC_CNT_0			= 0x8c,
	RTC_CNT_1			= 0x90,
	RTC_CNT_2			= 0x94,
	RTC_CNT_3			= 0x98,
};

struct rtc_rv1106_pdata_t {
	virtual_addr_t virt;
	char * clk;
};

static void rtc_update_bits(virtual_addr_t virt, u32_t offset, u32_t mask, u32_t set)
{
	u32_t val;

	val = read32(virt + offset);
	write32(virt + offset, (val & ~mask) | set | 0xc4522900);
}

static bool_t rtc_rv1106_settime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_rv1106_pdata_t * pdat = (struct rtc_rv1106_pdata_t *)rtc->priv;
	u32_t rtc_data[8];
	u32_t status;
	int yearh, yearl;
	int i;

	rtc_data[0] = bin2bcd(time->second) | 0xc4522900;
	rtc_data[1] = bin2bcd(time->minute) | 0xc4522900;
	rtc_data[2] = bin2bcd(time->hour) | 0xc4522900;
	rtc_data[3] = bin2bcd(time->day) | 0xc4522900;
	rtc_data[4] = bin2bcd(time->month) | 0xc4522900;
	if(time->year > 199)
	{
		yearh = (time->year - 100) / 100;
		yearl = time->year - 100 - yearh * 100;
	}
	else
	{
		yearh = 0;
		yearl = time->year - 100 - yearh * 100;
	}
	rtc_data[5] = bin2bcd(yearl) | 0xc4522900;
	rtc_data[6] = bin2bcd(yearh) | 0xc4522900;
	rtc_data[7] = bin2bcd(time->week) | 0xc4522900;

	rtc_update_bits(pdat->virt, RTC_CTRL, (1 << 0), 0);
	do {
		status = read32(pdat->virt + RTC_STATUS1);
		udelay(1);
	} while(status & (1 << 0));
	for(i = 0; i < 8; i++)
		write32(pdat->virt + RTC_SET_SECONDS + (i << 2), rtc_data[i]);
	rtc_update_bits(pdat->virt, RTC_CTRL, (1 << 7) | (1 << 0), (1 << 7) | (1 << 0));
	do {
		status = read32(pdat->virt + RTC_STATUS1);
		udelay(1);
	} while(!(status & (1 << 0)));

	return TRUE;
}

static bool_t rtc_rv1106_gettime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_rv1106_pdata_t * pdat = (struct rtc_rv1106_pdata_t *)rtc->priv;
	u32_t rtc_data[8];
	int i;

	for(i = 0; i < 8; i++)
		rtc_data[i] = read32(pdat->virt + RTC_SET_SECONDS + (i << 2));
	for(i = 0; i < 8; i++)
		rtc_data[i] = read32(pdat->virt + RTC_SET_SECONDS + (i << 2));
	for(i = 0; i < 8; i++)
		rtc_data[i] = read32(pdat->virt + RTC_SET_SECONDS + (i << 2));

	time->second = bcd2bin(rtc_data[0] & 0x7f);
	time->minute = bcd2bin(rtc_data[1] & 0x7f);
	time->hour = bcd2bin(rtc_data[2] & 0x3f);
	time->day = bcd2bin(rtc_data[3] & 0x3f);
	time->month = bcd2bin(rtc_data[4] & 0x1f);
	time->year = (bcd2bin(rtc_data[6] & 0xff)) * 100 + (bcd2bin(rtc_data[5] & 0xff)) + 100;
	time->week = bcd2bin(rtc_data[7] & 0x07);

	return TRUE;
}

static struct device_t * rtc_rv1106_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct rtc_rv1106_pdata_t * pdat;
	struct rtc_t * rtc;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct rtc_rv1106_pdata_t));
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
	rtc->settime = rtc_rv1106_settime;
	rtc->gettime = rtc_rv1106_gettime;
	rtc->priv = pdat;

	clk_enable(pdat->clk);
	write32(phys_to_virt(0xff000000) + 0x50000, ((1 << 6) << 16) | (1 << 6));
	rtc_update_bits(pdat->virt, RTC_VPTAT_TRIM, (1 << 4), (4 << 1));
	rtc_update_bits(pdat->virt, RTC_ANALOG_EN, (1 << 1), 0x00);
	rtc_update_bits(pdat->virt, RTC_LDO_CTRL, (1 << 0), (1 << 0));
	rtc_update_bits(pdat->virt, RTC_ANALOG_EN, (1 << 5), (1 << 5));
	rtc_update_bits(pdat->virt, RTC_CTRL, (1 << 0) | (1 << 7), (1 << 0) | (1 << 7));

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

static void rtc_rv1106_remove(struct device_t * dev)
{
	struct rtc_t * rtc = (struct rtc_t *)dev->priv;
	struct rtc_rv1106_pdata_t * pdat = (struct rtc_rv1106_pdata_t *)rtc->priv;

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

static void rtc_rv1106_suspend(struct device_t * dev)
{
}

static void rtc_rv1106_resume(struct device_t * dev)
{
}

static struct driver_t rtc_rv1106 = {
	.name		= "rtc-rv1106",
	.probe		= rtc_rv1106_probe,
	.remove		= rtc_rv1106_remove,
	.suspend	= rtc_rv1106_suspend,
	.resume		= rtc_rv1106_resume,
};

static __init void rtc_rv1106_driver_init(void)
{
	register_driver(&rtc_rv1106);
}

static __exit void rtc_rv1106_driver_exit(void)
{
	unregister_driver(&rtc_rv1106);
}

driver_initcall(rtc_rv1106_driver_init);
driver_exitcall(rtc_rv1106_driver_exit);
