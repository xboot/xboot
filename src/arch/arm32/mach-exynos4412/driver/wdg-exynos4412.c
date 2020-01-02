/*
 * driver/wdg-exynos4412.c
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
#include <clk/clk.h>
#include <watchdog/watchdog.h>

enum {
	WTCON		= 0x00,
	WTDAT		= 0x04,
	WTCNT		= 0x08,
	WTCLRINT	= 0x0c,
};

struct wdg_exynos4412_pdata_t {
	virtual_addr_t virt;
	char * clk;
};

static void wdg_exynos4412_set(struct watchdog_t * watchdog, int timeout)
{
	struct wdg_exynos4412_pdata_t * pdat = (struct wdg_exynos4412_pdata_t *)watchdog->priv;
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

static int wdg_exynos4412_get(struct watchdog_t * watchdog)
{
	struct wdg_exynos4412_pdata_t * pdat = (struct wdg_exynos4412_pdata_t *)watchdog->priv;
	u64_t rate = clk_get_rate(pdat->clk);
	u32_t val = read32(pdat->virt + WTCNT) & 0xffff;
	if(rate == 0)
		return 0;
	return (int)(val * (256 * 128) / rate);
}

static struct device_t * wdg_exynos4412_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct wdg_exynos4412_pdata_t * pdat;
	struct watchdog_t * wdg;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct wdg_exynos4412_pdata_t));
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
	wdg->set = wdg_exynos4412_set;
	wdg->get = wdg_exynos4412_get;
	wdg->priv = pdat;

	clk_enable(pdat->clk);
	write32(pdat->virt + WTCON, 0x0);
	write32(pdat->virt + WTDAT, 0x0);
	write32(pdat->virt + WTCNT, 0x0);

	if(!(dev = register_watchdog(wdg, drv)))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(wdg->name);
		free(wdg->priv);
		free(wdg);
		return NULL;
	}
	return dev;
}

static void wdg_exynos4412_remove(struct device_t * dev)
{
	struct watchdog_t * wdg = (struct watchdog_t *)dev->priv;
	struct wdg_exynos4412_pdata_t * pdat = (struct wdg_exynos4412_pdata_t *)wdg->priv;

	if(wdg)
	{
		unregister_watchdog(wdg);
		write32(pdat->virt + WTCON, 0x0);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(wdg->name);
		free(wdg->priv);
		free(wdg);
	}
}

static void wdg_exynos4412_suspend(struct device_t * dev)
{
}

static void wdg_exynos4412_resume(struct device_t * dev)
{
}

static struct driver_t wdg_exynos4412 = {
	.name		= "wdg-exynos4412",
	.probe		= wdg_exynos4412_probe,
	.remove		= wdg_exynos4412_remove,
	.suspend	= wdg_exynos4412_suspend,
	.resume		= wdg_exynos4412_resume,
};

static __init void wdg_exynos4412_driver_init(void)
{
	register_driver(&wdg_exynos4412);
}

static __exit void wdg_exynos4412_driver_exit(void)
{
	unregister_driver(&wdg_exynos4412);
}

driver_initcall(wdg_exynos4412_driver_init);
driver_exitcall(wdg_exynos4412_driver_exit);
