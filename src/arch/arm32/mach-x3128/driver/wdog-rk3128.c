/*
 * driver/wdog-rk3128.c
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
#include <clk/clk.h>
#include <watchdog/watchdog.h>

enum {
	WDT_CR		= 0x00,
	WDT_TORR	= 0x04,
	WDT_CCVR	= 0x08,
	WDT_CRR		= 0x0c,
	WDT_STAT	= 0x10,
	WDT_EOI		= 0x14,
};

struct wdog_rk3128_pdata_t {
	virtual_addr_t virt;
	char * clk;
};

static void wdog_rk3128_set(struct watchdog_t * watchdog, int timeout)
{
	struct wdog_rk3128_pdata_t * pdat = (struct wdog_rk3128_pdata_t *)watchdog->priv;
	u64_t rate = clk_get_rate(pdat->clk);
	int maxtime = 0x80000000 / clk_get_rate(pdat->clk) + 1;

	if(timeout < 0)
		timeout = 0;
	if(timeout > maxtime)
		timeout = maxtime;

	if(timeout > 0)
	{
		u32_t torr = 0, acc = 1;
		u32_t count = (timeout * rate) / 0x10000;

		while(acc < count)
		{
			acc *= 2;
			torr++;
		}
		if(torr > 15)
			torr = 15;

		write32(pdat->virt + WDT_CR, 0);
		write32(pdat->virt + WDT_TORR, torr);
		write32(pdat->virt + WDT_CRR, 0x76);
		write32(pdat->virt + WDT_CR, (0x3 << 2) | (0x0 << 1) | (0x1 << 0));
	}
	else
	{
		write32(pdat->virt + WDT_CRR, 0x76);
		write32(pdat->virt + WDT_CR, 0x0);
	}
}

static int wdog_rk3128_get(struct watchdog_t * watchdog)
{
	struct wdog_rk3128_pdata_t * pdat = (struct wdog_rk3128_pdata_t *)watchdog->priv;
	return (int)(read32(pdat->virt + WDT_CCVR) / clk_get_rate(pdat->clk));
}

static struct device_t * wdog_rk3128_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct wdog_rk3128_pdata_t * pdat;
	struct watchdog_t * wdog;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct wdog_rk3128_pdata_t));
	if(!pdat)
		return NULL;

	wdog = malloc(sizeof(struct watchdog_t));
	if(!wdog)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);

	wdog->name = alloc_device_name(dt_read_name(n), -1);
	wdog->set = wdog_rk3128_set;
	wdog->get = wdog_rk3128_get,
	wdog->priv = pdat;

	clk_enable(pdat->clk);
	write32(pdat->virt + WDT_CRR, 0x76);
	write32(pdat->virt + WDT_CR, 0x0);

	if(!register_watchdog(&dev, wdog))
	{
		write32(pdat->virt + WDT_CRR, 0x76);
		write32(pdat->virt + WDT_CR, 0x0);
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(wdog->name);
		free(wdog->priv);
		free(wdog);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void wdog_rk3128_remove(struct device_t * dev)
{
	struct watchdog_t * wdog = (struct watchdog_t *)dev->priv;
	struct wdog_rk3128_pdata_t * pdat = (struct wdog_rk3128_pdata_t *)wdog->priv;

	if(wdog && unregister_watchdog(wdog))
	{
		write32(pdat->virt + WDT_CRR, 0x76);
		write32(pdat->virt + WDT_CR, 0x0);
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(wdog->name);
		free(wdog->priv);
		free(wdog);
	}
}

static void wdog_rk3128_suspend(struct device_t * dev)
{
}

static void wdog_rk3128_resume(struct device_t * dev)
{
}

static struct driver_t wdog_rk3128 = {
	.name		= "wdog-rk3128",
	.probe		= wdog_rk3128_probe,
	.remove		= wdog_rk3128_remove,
	.suspend	= wdog_rk3128_suspend,
	.resume		= wdog_rk3128_resume,
};

static __init void wdog_rk3128_driver_init(void)
{
	register_driver(&wdog_rk3128);
}

static __exit void wdog_rk3128_driver_exit(void)
{
	unregister_driver(&wdog_rk3128);
}

driver_initcall(wdog_rk3128_driver_init);
driver_exitcall(wdog_rk3128_driver_exit);
