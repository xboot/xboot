/*
 * driver/cs-s5l8930-timer.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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
#include <clocksource/clocksource.h>

enum {
	TIMER_LOW	= 0x00,
	TIMER_HIGH	= 0x04,
};

struct cs_s5l8930_timer_pdata_t
{
	virtual_addr_t virt;
	char * clk;
};

static u64_t cs_s5l8930_timer_read(struct clocksource_t * cs)
{
	struct cs_s5l8930_timer_pdata_t * pdat = (struct cs_s5l8930_timer_pdata_t *)cs->priv;
	return (u64_t)(((u64_t)read32(pdat->virt + TIMER_HIGH) << 32) | read32(pdat->virt + TIMER_LOW));
}

static struct device_t * cs_s5l8930_timer_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct cs_s5l8930_timer_pdata_t * pdat;
	struct clocksource_t * cs;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct cs_s5l8930_timer_pdata_t));
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
	cs->read = cs_s5l8930_timer_read;
	cs->priv = pdat;

	write32(pdat->virt + TIMER_HIGH, 0);
	write32(pdat->virt + TIMER_LOW, 0);

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

static void cs_s5l8930_timer_remove(struct device_t * dev)
{
	struct clocksource_t * cs = (struct clocksource_t *)dev->priv;
	struct cs_s5l8930_timer_pdata_t * pdat = (struct cs_s5l8930_timer_pdata_t *)cs->priv;

	if(cs && unregister_clocksource(cs))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(cs->name);
		free(cs->priv);
		free(cs);
	}
}

static void cs_s5l8930_timer_suspend(struct device_t * dev)
{
}

static void cs_s5l8930_timer_resume(struct device_t * dev)
{
}

static struct driver_t cs_s5l8930_timer = {
	.name		= "cs-s5l8930-timer",
	.probe		= cs_s5l8930_timer_probe,
	.remove		= cs_s5l8930_timer_remove,
	.suspend	= cs_s5l8930_timer_suspend,
	.resume		= cs_s5l8930_timer_resume,
};

static __init void cs_s5l8930_timer_driver_init(void)
{
	register_driver(&cs_s5l8930_timer);
}

static __exit void cs_s5l8930_timer_driver_exit(void)
{
	unregister_driver(&cs_s5l8930_timer);
}

driver_initcall(cs_s5l8930_timer_driver_init);
driver_exitcall(cs_s5l8930_timer_driver_exit);
