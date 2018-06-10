/*
 * driver/wdg-s5pv210.c
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
	WTCON		= 0x00,
	WTDAT		= 0x04,
	WTCNT		= 0x08,
	WTCLRINT	= 0x0c,
};

struct wdg_s5pv210_pdata_t {
	virtual_addr_t virt;
	char * clk;
};

static void wdg_s5pv210_set(struct watchdog_t * watchdog, int timeout)
{
	struct wdg_s5pv210_pdata_t * pdat = (struct wdg_s5pv210_pdata_t *)watchdog->priv;
	u64_t rate = clk_get_rate(pdat->clk);
	u32_t max = (int)(65535 * (256 * 128) / rate);
	u32_t val;

	if(timeout < 0)
		timeout = 0;
	if(timeout > max)
		timeout = max;

	if(timeout > 0)
	{
		val = (timeout * rate / (256 * 128)) & 0xffff;
		write32(pdat->virt + WTDAT, val);
		write32(pdat->virt + WTCNT, val);
		val = (0xff << 8) | (1 << 5) | (0x3 << 3) | (0 << 2) | (1 << 0);
		write32(pdat->virt + WTCON, val);
	}
	else
	{
		write32(pdat->virt + WTCON, 0x0);
	}
}

static int wdg_s5pv210_get(struct watchdog_t * watchdog)
{
	struct wdg_s5pv210_pdata_t * pdat = (struct wdg_s5pv210_pdata_t *)watchdog->priv;
	u64_t rate = clk_get_rate(pdat->clk);
	u32_t val = read32(pdat->virt + WTCNT) & 0xffff;
	if(rate == 0)
		return 0;
	return (int)(val * (256 * 128) / rate);
}

static struct device_t * wdg_s5pv210_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct wdg_s5pv210_pdata_t * pdat;
	struct watchdog_t * wdg;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct wdg_s5pv210_pdata_t));
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
	wdg->set = wdg_s5pv210_set;
	wdg->get = wdg_s5pv210_get;
	wdg->priv = pdat;

	clk_enable(pdat->clk);
	write32(pdat->virt + WTCON, 0x0);
	write32(pdat->virt + WTDAT, 0x0);
	write32(pdat->virt + WTCNT, 0x0);

	if(!register_watchdog(&dev, wdg))
	{
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

static void wdg_s5pv210_remove(struct device_t * dev)
{
	struct watchdog_t * wdg = (struct watchdog_t *)dev->priv;
	struct wdg_s5pv210_pdata_t * pdat = (struct wdg_s5pv210_pdata_t *)wdg->priv;

	if(wdg && unregister_watchdog(wdg))
	{
		write32(pdat->virt + WTCON, 0x0);
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(wdg->name);
		free(wdg->priv);
		free(wdg);
	}
}

static void wdg_s5pv210_suspend(struct device_t * dev)
{
}

static void wdg_s5pv210_resume(struct device_t * dev)
{
}

static struct driver_t wdg_s5pv210 = {
	.name		= "wdg-s5pv210",
	.probe		= wdg_s5pv210_probe,
	.remove		= wdg_s5pv210_remove,
	.suspend	= wdg_s5pv210_suspend,
	.resume		= wdg_s5pv210_resume,
};

static __init void wdg_s5pv210_driver_init(void)
{
	register_driver(&wdg_s5pv210);
}

static __exit void wdg_s5pv210_driver_exit(void)
{
	unregister_driver(&wdg_s5pv210);
}

driver_initcall(wdg_s5pv210_driver_init);
driver_exitcall(wdg_s5pv210_driver_exit);
