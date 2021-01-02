/*
 * driver/wdg-s5l8930.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
	WDT_CNT		= 0x00,
	WDT_RDAT	= 0x04,
	WDT_IDAT	= 0x08,
	WDT_CON		= 0x0c,
};

struct wdg_s5l8930_pdata_t {
	virtual_addr_t virt;
	char * clk;
};

static void wdg_s5l8930_set(struct watchdog_t * watchdog, int timeout)
{
	struct wdg_s5l8930_pdata_t * pdat = (struct wdg_s5l8930_pdata_t *)watchdog->priv;
	u64_t rate = clk_get_rate(pdat->clk);
	int maxtime = 0x80000000 / rate;

	if(timeout < 0)
		timeout = 0;
	if(timeout > maxtime)
		timeout = maxtime;

	if(timeout > 0)
	{
		write32(pdat->virt + WDT_CNT, 0);
		write32(pdat->virt + WDT_RDAT, timeout * rate);
		write32(pdat->virt + WDT_CON, (0x1 << 2));
	}
	else
	{
		write32(pdat->virt + WDT_CON, 0x0);
	}
}

static int wdg_s5l8930_get(struct watchdog_t * watchdog)
{
	struct wdg_s5l8930_pdata_t * pdat = (struct wdg_s5l8930_pdata_t *)watchdog->priv;
	u32_t con = read32(pdat->virt + WDT_CON);
	u32_t dat = read32(pdat->virt + WDT_RDAT);
	u32_t cnt = read32(pdat->virt + WDT_CNT);
	if(con && (dat > cnt))
		return (int)((dat - cnt) / clk_get_rate(pdat->clk));
	return 0;
}

static struct device_t * wdg_s5l8930_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct wdg_s5l8930_pdata_t * pdat;
	struct watchdog_t * wdg;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct wdg_s5l8930_pdata_t));
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
	wdg->set = wdg_s5l8930_set;
	wdg->get = wdg_s5l8930_get;
	wdg->priv = pdat;

	clk_enable(pdat->clk);
	write32(pdat->virt + WDT_CON, 0x0);

	if(!(dev = register_watchdog(wdg, drv)))
	{
		write32(pdat->virt + WDT_CON, 0x0);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(wdg->name);
		free(wdg->priv);
		free(wdg);
		return NULL;
	}
	return dev;
}

static void wdg_s5l8930_remove(struct device_t * dev)
{
	struct watchdog_t * wdg = (struct watchdog_t *)dev->priv;
	struct wdg_s5l8930_pdata_t * pdat = (struct wdg_s5l8930_pdata_t *)wdg->priv;

	if(wdg)
	{
		unregister_watchdog(wdg);
		write32(pdat->virt + WDT_CON, 0x0);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(wdg->name);
		free(wdg->priv);
		free(wdg);
	}
}

static void wdg_s5l8930_suspend(struct device_t * dev)
{
}

static void wdg_s5l8930_resume(struct device_t * dev)
{
}

static struct driver_t wdg_s5l8930 = {
	.name		= "wdg-s5l8930",
	.probe		= wdg_s5l8930_probe,
	.remove		= wdg_s5l8930_remove,
	.suspend	= wdg_s5l8930_suspend,
	.resume		= wdg_s5l8930_resume,
};

static __init void wdg_s5l8930_driver_init(void)
{
	register_driver(&wdg_s5l8930);
}

static __exit void wdg_s5l8930_driver_exit(void)
{
	unregister_driver(&wdg_s5l8930);
}

driver_initcall(wdg_s5l8930_driver_init);
driver_exitcall(wdg_s5l8930_driver_exit);
