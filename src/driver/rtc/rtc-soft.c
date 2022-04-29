/*
 * driver/rtc/rtc-soft.c
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
#include <rtc/rtc.h>

struct rtc_soft_pdata_t {
	int64_t adjust;
};

static bool_t rtc_soft_settime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_soft_pdata_t * pdat = (struct rtc_soft_pdata_t *)rtc->priv;

	pdat->adjust = (int64_t)rtc_time_to_secs(time) * 1000000000ULL - ktime_to_ns(ktime_get());
	return TRUE;
}

static bool_t rtc_soft_gettime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_soft_pdata_t * pdat = (struct rtc_soft_pdata_t *)rtc->priv;

	secs_to_rtc_time((uint64_t)((ktime_to_ns(ktime_get()) + pdat->adjust) / 1000000000ULL), time);
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
