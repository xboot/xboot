/*
 * driver/wdg-rk3128.c
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

struct wdg_rk3128_pdata_t {
	virtual_addr_t virt;
	char * clk;
};

static void wdg_rk3128_set(struct watchdog_t * watchdog, int timeout)
{
	struct wdg_rk3128_pdata_t * pdat = (struct wdg_rk3128_pdata_t *)watchdog->priv;
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

static int wdg_rk3128_get(struct watchdog_t * watchdog)
{
	struct wdg_rk3128_pdata_t * pdat = (struct wdg_rk3128_pdata_t *)watchdog->priv;
	return (int)(read32(pdat->virt + WDT_CCVR) / clk_get_rate(pdat->clk));
}

static struct device_t * wdg_rk3128_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct wdg_rk3128_pdata_t * pdat;
	struct watchdog_t * wdg;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct wdg_rk3128_pdata_t));
	if(!pdat)
		return NULL;

	wdg = malloc(sizeof(struct watchdog_t));
	if(!wdg)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);

	wdg->name = alloc_device_name(dt_read_name(n), -1);
	wdg->set = wdg_rk3128_set;
	wdg->get = wdg_rk3128_get;
	wdg->priv = pdat;

	clk_enable(pdat->clk);
	write32(pdat->virt + WDT_CRR, 0x76);
	write32(pdat->virt + WDT_CR, 0x0);

	if(!register_watchdog(&dev, wdg))
	{
		write32(pdat->virt + WDT_CRR, 0x76);
		write32(pdat->virt + WDT_CR, 0x0);
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(wdg->name);
		free(wdg->priv);
		free(wdg);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void wdg_rk3128_remove(struct device_t * dev)
{
	struct watchdog_t * wdg = (struct watchdog_t *)dev->priv;
	struct wdg_rk3128_pdata_t * pdat = (struct wdg_rk3128_pdata_t *)wdg->priv;

	if(wdg && unregister_watchdog(wdg))
	{
		write32(pdat->virt + WDT_CRR, 0x76);
		write32(pdat->virt + WDT_CR, 0x0);
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(wdg->name);
		free(wdg->priv);
		free(wdg);
	}
}

static void wdg_rk3128_suspend(struct device_t * dev)
{
}

static void wdg_rk3128_resume(struct device_t * dev)
{
}

static struct driver_t wdg_rk3128 = {
	.name		= "wdg-rk3128",
	.probe		= wdg_rk3128_probe,
	.remove		= wdg_rk3128_remove,
	.suspend	= wdg_rk3128_suspend,
	.resume		= wdg_rk3128_resume,
};

static __init void wdg_rk3128_driver_init(void)
{
	register_driver(&wdg_rk3128);
}

static __exit void wdg_rk3128_driver_exit(void)
{
	unregister_driver(&wdg_rk3128);
}

driver_initcall(wdg_rk3128_driver_init);
driver_exitcall(wdg_rk3128_driver_exit);
