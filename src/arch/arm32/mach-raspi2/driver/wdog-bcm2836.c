/*
 * driver/wdog-bcm2836.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <watchdog/watchdog.h>

/*
 * WatchDog - Broadcom BCM2836 watchdog timer
 *
 * Example:
 *   "wdog-bcm2836@0x3f100000": {
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

struct wdog_bcm2836_pdata_t {
	virtual_addr_t virt;
	int start;
};

static void wdog_bcm2836_set(struct watchdog_t * watchdog, int timeout)
{
	struct wdog_bcm2836_pdata_t * pdat = (struct wdog_bcm2836_pdata_t *)watchdog->priv;
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

static int wdog_bcm2836_get(struct watchdog_t * watchdog)
{
	struct wdog_bcm2836_pdata_t * pdat = (struct wdog_bcm2836_pdata_t *)watchdog->priv;
	return (pdat->start != 0) ? ((read32(pdat->virt + PM_WDOG) & PM_WDOG_TIME_SET) >> 16) : 0;
}

static struct device_t * wdog_bcm2836_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct wdog_bcm2836_pdata_t * pdat;
	struct watchdog_t * wdog;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));

	pdat = malloc(sizeof(struct wdog_bcm2836_pdata_t));
	if(!pdat)
		return NULL;

	wdog = malloc(sizeof(struct watchdog_t));
	if(!wdog)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->start = 0;

	wdog->name = alloc_device_name(dt_read_name(n), -1);
	wdog->set = wdog_bcm2836_set;
	wdog->get = wdog_bcm2836_get,
	wdog->priv = pdat;

	if(!register_watchdog(&dev, wdog))
	{
		free_device_name(wdog->name);
		free(wdog->priv);
		free(wdog);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void wdog_bcm2836_remove(struct device_t * dev)
{
	struct watchdog_t * wdog = (struct watchdog_t *)dev->priv;

	if(wdog && unregister_watchdog(wdog))
	{
		free_device_name(wdog->name);
		free(wdog->priv);
		free(wdog);
	}
}

static void wdog_bcm2836_suspend(struct device_t * dev)
{
}

static void wdog_bcm2836_resume(struct device_t * dev)
{
}

static struct driver_t wdog_bcm2836 = {
	.name		= "wdog-bcm2836",
	.probe		= wdog_bcm2836_probe,
	.remove		= wdog_bcm2836_remove,
	.suspend	= wdog_bcm2836_suspend,
	.resume		= wdog_bcm2836_resume,
};

static __init void wdog_bcm2836_driver_init(void)
{
	register_driver(&wdog_bcm2836);
}

static __exit void wdog_bcm2836_driver_exit(void)
{
	unregister_driver(&wdog_bcm2836);
}

driver_initcall(wdog_bcm2836_driver_init);
driver_exitcall(wdog_bcm2836_driver_exit);
