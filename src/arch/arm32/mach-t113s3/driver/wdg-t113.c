/*
 * driver/wdg-t113.c
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
#include <clk/clk.h>
#include <watchdog/watchdog.h>

enum {
	WDG_IRQ_EN		= 0x00,
	WDG_IRQ_STA		= 0x04,
	WDG_SOFT_RST	= 0x08,
	WDG_CTRL		= 0x10,
	WDG_CFG			= 0x14,
	WDG_MODE		= 0x18,
	WDG_OUTPUT_CFG	= 0x1c,
};

struct wdg_t113_pdata_t {
	virtual_addr_t virt;
	char * clk;
};

static const int wdt_timeout_map[] = {
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x0, 0x7, 0x0, 0x8, 0x0, 0x9, 0x0, 0xa, 0x0, 0xb,
};

static void wdg_t113_set(struct watchdog_t * watchdog, int timeout)
{
	struct wdg_t113_pdata_t * pdat = (struct wdg_t113_pdata_t *)watchdog->priv;
	u32_t val;

	if(timeout < 0)
		timeout = 0;
	if(timeout > 16)
		timeout = 16;

	if(timeout > 0)
	{
		if(wdt_timeout_map[timeout] == 0)
			timeout++;

		val = read32(pdat->virt + WDG_MODE) & 0xffff;
		val &= ~(0xf << 4);
		val |= (wdt_timeout_map[timeout] << 4) | (0x1 << 0);
		write32(pdat->virt + WDG_MODE, (0x16aa << 16) | val);
		write32(pdat->virt + WDG_CTRL, (0xa57 << 1) | (1 << 0));
	}
	else
	{
		write32(pdat->virt + WDG_MODE, (0x16aa << 16) | (0 << 0));
		write32(pdat->virt + WDG_CTRL, (0xa57 << 1) | (1 << 0));
	}
}

static int wdg_t113_get(struct watchdog_t * watchdog)
{
	return 0;
}

static struct device_t * wdg_t113_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct wdg_t113_pdata_t * pdat;
	struct watchdog_t * wdg;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct wdg_t113_pdata_t));
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
	wdg->set = wdg_t113_set;
	wdg->get = wdg_t113_get;
	wdg->priv = pdat;

	clk_enable(pdat->clk);
	write32(pdat->virt + WDG_IRQ_EN, 0x0);
	write32(pdat->virt + WDG_IRQ_STA, 0x1);
	write32(pdat->virt + WDG_CFG, (0x16aa << 16) | (0x1 << 0));
	write32(pdat->virt + WDG_MODE, (0x16aa << 16) | (0 << 0));
	write32(pdat->virt + WDG_CTRL, (0xa57 << 1) | (1 << 0));

	if(!(dev = register_watchdog(wdg, drv)))
	{
		write32(pdat->virt + WDG_MODE, (0x16aa << 16) | (0 << 0));
		write32(pdat->virt + WDG_CTRL, (0xa57 << 1) | (1 << 0));
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(wdg->name);
		free(wdg->priv);
		free(wdg);
		return NULL;
	}
	return dev;
}

static void wdg_t113_remove(struct device_t * dev)
{
	struct watchdog_t * wdg = (struct watchdog_t *)dev->priv;
	struct wdg_t113_pdata_t * pdat = (struct wdg_t113_pdata_t *)wdg->priv;

	if(wdg)
	{
		unregister_watchdog(wdg);
		write32(pdat->virt + WDG_MODE, 0x0);
		write32(pdat->virt + WDG_CTRL, (0xa57 << 1) | (1 << 0));
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(wdg->name);
		free(wdg->priv);
		free(wdg);
	}
}

static void wdg_t113_suspend(struct device_t * dev)
{
}

static void wdg_t113_resume(struct device_t * dev)
{
}

static struct driver_t wdg_t113 = {
	.name		= "wdg-t113",
	.probe		= wdg_t113_probe,
	.remove		= wdg_t113_remove,
	.suspend	= wdg_t113_suspend,
	.resume		= wdg_t113_resume,
};

static __init void wdg_t113_driver_init(void)
{
	register_driver(&wdg_t113);
}

static __exit void wdg_t113_driver_exit(void)
{
	unregister_driver(&wdg_t113);
}

driver_initcall(wdg_t113_driver_init);
driver_exitcall(wdg_t113_driver_exit);
