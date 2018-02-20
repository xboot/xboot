/*
 * driver/wdog-f1c100s.c
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
	WDOG_IRQ_EN		= 0x00,
	WDOG_IRQ_STA	= 0x04,
	WDOG_CTRL		= 0x10,
	WDOG_CFG		= 0x14,
	WDOG_MODE		= 0x18,
};

struct wdog_f1c100s_pdata_t {
	virtual_addr_t virt;
	char * clk;
};

static const int wdt_timeout_map[] = {
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x0, 0x7, 0x0, 0x8, 0x0, 0x9, 0x0, 0xa, 0x0, 0xb,
};

static void wdog_f1c100s_set(struct watchdog_t * watchdog, int timeout)
{
	struct wdog_f1c100s_pdata_t * pdat = (struct wdog_f1c100s_pdata_t *)watchdog->priv;
	u32_t val;

	if(timeout < 0)
		timeout = 0;
	if(timeout > 16)
		timeout = 16;

	if(timeout > 0)
	{
		if(wdt_timeout_map[timeout] == 0)
			timeout++;

		val = read32(pdat->virt + WDOG_MODE);
		val &= ~(0xf << 4);
		val |= (wdt_timeout_map[timeout] << 4) | (0x1 << 0);
		write32(pdat->virt + WDOG_MODE, val);
		write32(pdat->virt + WDOG_CTRL, (0xa57 << 1) | (1 << 0));
	}
	else
	{
		write32(pdat->virt + WDOG_MODE, 0x0);
		write32(pdat->virt + WDOG_CTRL, (0xa57 << 1) | (1 << 0));
	}
}

static int wdog_f1c100s_get(struct watchdog_t * watchdog)
{
	return 0;
}

static struct device_t * wdog_f1c100s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct wdog_f1c100s_pdata_t * pdat;
	struct watchdog_t * wdog;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct wdog_f1c100s_pdata_t));
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
	wdog->set = wdog_f1c100s_set;
	wdog->get = wdog_f1c100s_get;
	wdog->priv = pdat;

	clk_enable(pdat->clk);
	write32(pdat->virt + WDOG_IRQ_EN, 0x0);
	write32(pdat->virt + WDOG_IRQ_STA, 0x1);
	write32(pdat->virt + WDOG_CFG, 0x1);
	write32(pdat->virt + WDOG_MODE, 0x0);
	write32(pdat->virt + WDOG_CTRL, (0xa57 << 1) | (1 << 0));

	if(!register_watchdog(&dev, wdog))
	{
		write32(pdat->virt + WDOG_MODE, 0x0);
		write32(pdat->virt + WDOG_CTRL, (0xa57 << 1) | (1 << 0));
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

static void wdog_f1c100s_remove(struct device_t * dev)
{
	struct watchdog_t * wdog = (struct watchdog_t *)dev->priv;
	struct wdog_f1c100s_pdata_t * pdat = (struct wdog_f1c100s_pdata_t *)wdog->priv;

	if(wdog && unregister_watchdog(wdog))
	{
		write32(pdat->virt + WDOG_MODE, 0x0);
		write32(pdat->virt + WDOG_CTRL, (0xa57 << 1) | (1 << 0));
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(wdog->name);
		free(wdog->priv);
		free(wdog);
	}
}

static void wdog_f1c100s_suspend(struct device_t * dev)
{
}

static void wdog_f1c100s_resume(struct device_t * dev)
{
}

static struct driver_t wdog_f1c100s = {
	.name		= "wdog-f1c100s",
	.probe		= wdog_f1c100s_probe,
	.remove		= wdog_f1c100s_remove,
	.suspend	= wdog_f1c100s_suspend,
	.resume		= wdog_f1c100s_resume,
};

static __init void wdog_f1c100s_driver_init(void)
{
	register_driver(&wdog_f1c100s);
}

static __exit void wdog_f1c100s_driver_exit(void)
{
	unregister_driver(&wdog_f1c100s);
}

driver_initcall(wdog_f1c100s_driver_init);
driver_exitcall(wdog_f1c100s_driver_exit);
