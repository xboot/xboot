/*
 * driver/wdg-rk3399.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
	WDT_CR		= 0x00,
	WDT_TORR	= 0x04,
	WDT_CCVR	= 0x08,
	WDT_CRR		= 0x0c,
	WDT_STAT	= 0x10,
	WDT_EOI		= 0x14,
};

struct wdg_rk3399_pdata_t {
	virtual_addr_t virt;
	char * clk;
};

static void wdg_rk3399_set(struct watchdog_t * watchdog, int timeout)
{
	struct wdg_rk3399_pdata_t * pdat = (struct wdg_rk3399_pdata_t *)watchdog->priv;
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

static int wdg_rk3399_get(struct watchdog_t * watchdog)
{
	struct wdg_rk3399_pdata_t * pdat = (struct wdg_rk3399_pdata_t *)watchdog->priv;
	return (int)(read32(pdat->virt + WDT_CCVR) / clk_get_rate(pdat->clk));
}

static struct device_t * wdg_rk3399_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct wdg_rk3399_pdata_t * pdat;
	struct watchdog_t * wdg;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct wdg_rk3399_pdata_t));
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
	wdg->set = wdg_rk3399_set;
	wdg->get = wdg_rk3399_get;
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

static void wdg_rk3399_remove(struct device_t * dev)
{
	struct watchdog_t * wdg = (struct watchdog_t *)dev->priv;
	struct wdg_rk3399_pdata_t * pdat = (struct wdg_rk3399_pdata_t *)wdg->priv;

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

static void wdg_rk3399_suspend(struct device_t * dev)
{
}

static void wdg_rk3399_resume(struct device_t * dev)
{
}

static struct driver_t wdg_rk3399 = {
	.name		= "wdg-rk3399",
	.probe		= wdg_rk3399_probe,
	.remove		= wdg_rk3399_remove,
	.suspend	= wdg_rk3399_suspend,
	.resume		= wdg_rk3399_resume,
};

static __init void wdg_rk3399_driver_init(void)
{
	register_driver(&wdg_rk3399);
}

static __exit void wdg_rk3399_driver_exit(void)
{
	unregister_driver(&wdg_rk3399);
}

driver_initcall(wdg_rk3399_driver_init);
driver_exitcall(wdg_rk3399_driver_exit);
