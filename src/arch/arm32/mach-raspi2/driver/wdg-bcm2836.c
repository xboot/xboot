/*
 * driver/wdg-bcm2836.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
#include <watchdog/watchdog.h>

/*
 * WatchDog - Broadcom BCM2836 watchdog timer
 *
 * Example:
 *   "wdg-bcm2836@0x3f100000": {
 *   }
 */

#define PM_PASSWORD					0x5a000000
#define PM_WDOG_TIME_SET			0x000fffff
#define PM_RSTC_WRCFG_CLR			0xffffffcf
#define PM_RSTC_WRCFG_SET			0x00000030
#define PM_RSTC_WRCFG_FULL_RESET	0x00000020
#define PM_RSTC_RESET				0x00000102
#define PM_RSTS_HADWRH_SET			0x00000040
#define PM_RSTS_RASPBERRYPI_HALT	0x00000555

enum {
	PM_RSTC = 0x1c,
	PM_RSTS = 0x20,
	PM_WDOG = 0x24,
};

struct wdg_bcm2836_pdata_t {
	virtual_addr_t virt;
	int start;
};

static void wdg_bcm2836_set(struct watchdog_t * watchdog, int timeout)
{
	struct wdg_bcm2836_pdata_t * pdat = (struct wdg_bcm2836_pdata_t *)watchdog->priv;
	u32_t val;

	if(timeout < 0)
		timeout = 0;
	else if(timeout > 15)
		timeout = 15;

	if(timeout > 0)
	{
		write32(pdat->virt + PM_WDOG, PM_PASSWORD | ((timeout << 16) & PM_WDOG_TIME_SET));
		val = read32(pdat->virt + PM_RSTC);
		write32(pdat->virt + PM_RSTC, PM_PASSWORD | (val & PM_RSTC_WRCFG_CLR) | PM_RSTC_WRCFG_FULL_RESET);
		pdat->start = 1;
	}
	else
	{
		write32(pdat->virt + PM_RSTC, PM_PASSWORD | PM_RSTC_RESET);
		pdat->start = 0;
	}
}

static int wdg_bcm2836_get(struct watchdog_t * watchdog)
{
	struct wdg_bcm2836_pdata_t * pdat = (struct wdg_bcm2836_pdata_t *)watchdog->priv;
	return (pdat->start != 0) ? ((read32(pdat->virt + PM_WDOG) & PM_WDOG_TIME_SET) >> 16) : 0;
}

static struct device_t * wdg_bcm2836_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct wdg_bcm2836_pdata_t * pdat;
	struct watchdog_t * wdg;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));

	pdat = malloc(sizeof(struct wdg_bcm2836_pdata_t));
	if(!pdat)
		return NULL;

	wdg = malloc(sizeof(struct watchdog_t));
	if(!wdg)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->start = 0;

	wdg->name = alloc_device_name(dt_read_name(n), -1);
	wdg->set = wdg_bcm2836_set;
	wdg->get = wdg_bcm2836_get;
	wdg->priv = pdat;

	if(!(dev = register_watchdog(wdg, drv)))
	{
		free_device_name(wdg->name);
		free(wdg->priv);
		free(wdg);
		return NULL;
	}
	return dev;
}

static void wdg_bcm2836_remove(struct device_t * dev)
{
	struct watchdog_t * wdg = (struct watchdog_t *)dev->priv;

	if(wdg)
	{
		unregister_watchdog(wdg);
		free_device_name(wdg->name);
		free(wdg->priv);
		free(wdg);
	}
}

static void wdg_bcm2836_suspend(struct device_t * dev)
{
}

static void wdg_bcm2836_resume(struct device_t * dev)
{
}

static struct driver_t wdg_bcm2836 = {
	.name		= "wdg-bcm2836",
	.probe		= wdg_bcm2836_probe,
	.remove		= wdg_bcm2836_remove,
	.suspend	= wdg_bcm2836_suspend,
	.resume		= wdg_bcm2836_resume,
};

static __init void wdg_bcm2836_driver_init(void)
{
	register_driver(&wdg_bcm2836);
}

static __exit void wdg_bcm2836_driver_exit(void)
{
	unregister_driver(&wdg_bcm2836);
}

driver_initcall(wdg_bcm2836_driver_init);
driver_exitcall(wdg_bcm2836_driver_exit);
