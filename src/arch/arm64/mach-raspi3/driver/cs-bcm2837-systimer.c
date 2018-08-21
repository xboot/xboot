/*
 * driver/cs-bcm2837-systimer.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <clocksource/clocksource.h>

enum {
	SYS_TIMER_CS	= 0x00,
	SYS_TIMER_CLO	= 0x04,
	SYS_TIMER_CHI	= 0x08,
	SYS_TIMER_C0	= 0x0c,
	SYS_TIMER_C1	= 0x10,
	SYS_TIMER_C2	= 0x14,
	SYS_TIMER_C3	= 0x18,
};

struct cs_bcm2837_systimer_pdata_t
{
	virtual_addr_t virt;
	char * clk;
};

static u64_t cs_bcm2837_systimer_read(struct clocksource_t * cs)
{
	struct cs_bcm2837_systimer_pdata_t * pdat = (struct cs_bcm2837_systimer_pdata_t *)cs->priv;
	return (u64_t)(((u64_t)read32(pdat->virt + SYS_TIMER_CHI) << 32) | read32(pdat->virt + SYS_TIMER_CLO));
}

static struct device_t * cs_bcm2837_systimer_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct cs_bcm2837_systimer_pdata_t * pdat;
	struct clocksource_t * cs;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct cs_bcm2837_systimer_pdata_t));
	if(!pdat)
		return NULL;

	cs = malloc(sizeof(struct clocksource_t));
	if(!cs)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);

	clk_enable(pdat->clk);
	clocksource_calc_mult_shift(&cs->mult, &cs->shift, clk_get_rate(pdat->clk), 1000000000ULL, 10);
	cs->name = alloc_device_name(dt_read_name(n), -1);
	cs->mask = CLOCKSOURCE_MASK(64);
	cs->read = cs_bcm2837_systimer_read;
	cs->priv = pdat;

	if(!register_clocksource(&dev, cs))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(cs->name);
		free(cs->priv);
		free(cs);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void cs_bcm2837_systimer_remove(struct device_t * dev)
{
	struct clocksource_t * cs = (struct clocksource_t *)dev->priv;
	struct cs_bcm2837_systimer_pdata_t * pdat = (struct cs_bcm2837_systimer_pdata_t *)cs->priv;

	if(cs && unregister_clocksource(cs))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(cs->name);
		free(cs->priv);
		free(cs);
	}
}

static void cs_bcm2837_systimer_suspend(struct device_t * dev)
{
}

static void cs_bcm2837_systimer_resume(struct device_t * dev)
{
}

static struct driver_t cs_bcm2837_systimer = {
	.name		= "cs-bcm2837-systimer",
	.probe		= cs_bcm2837_systimer_probe,
	.remove		= cs_bcm2837_systimer_remove,
	.suspend	= cs_bcm2837_systimer_suspend,
	.resume		= cs_bcm2837_systimer_resume,
};

static __init void cs_bcm2837_systimer_driver_init(void)
{
	register_driver(&cs_bcm2837_systimer);
}

static __exit void cs_bcm2837_systimer_driver_exit(void)
{
	unregister_driver(&cs_bcm2837_systimer);
}

driver_initcall(cs_bcm2837_systimer_driver_init);
driver_exitcall(cs_bcm2837_systimer_driver_exit);
