/*
 * driver/rtc-pl031.c
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

struct rtc_pl031_pdata_t {
	virtual_addr_t virt;
};

static bool_t rtc_pl031_settime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_pl031_pdata_t * pdat = (struct rtc_pl031_pdata_t *)rtc->priv;

	write32(pdat->virt + RTC_LR, (u32_t)rtc_time_to_secs(time));
	return TRUE;
}

static bool_t rtc_pl031_gettime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_pl031_pdata_t * pdat = (struct rtc_pl031_pdata_t *)rtc->priv;

	secs_to_rtc_time(read32(pdat->virt + RTC_DR), time);
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

	rtc->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	rtc->settime = rtc_pl031_settime;
	rtc->gettime = rtc_pl031_gettime;
	rtc->priv = pdat;

	write32(pdat->virt + RTC_IMSC, 1);
	write32(pdat->virt + RTC_ICR, 1);
	write32(pdat->virt + RTC_CR, 1);

	if(!(dev = register_rtc(rtc, drv)))
	{
		free_device_name(rtc->name);
		free(rtc->priv);
		free(rtc);
		return NULL;
	}
	return dev;
}

static void rtc_pl031_remove(struct device_t * dev)
{
	struct rtc_t * rtc = (struct rtc_t *)dev->priv;
	struct rtc_pl031_pdata_t * pdat = (struct rtc_pl031_pdata_t *)rtc->priv;

	if(rtc)
	{
		unregister_rtc(rtc);
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
