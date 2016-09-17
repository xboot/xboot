/*
 * driver/cs-samsung-timer.c
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
#include <samsung-timer.h>

struct cs_samsung_timer_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int channel;
};

static u64_t cs_samsung_timer_read(struct clocksource_t * cs)
{
	struct cs_samsung_timer_pdata_t * pdat = (struct cs_samsung_timer_pdata_t *)cs->priv;
	return (u64_t)(0xffffffff - samsung_timer_read(pdat->virt, pdat->channel));
}

static struct device_t * cs_samsung_timer_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct cs_samsung_timer_pdata_t * pdat;
	struct clocksource_t * cs;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int channel = dt_read_int(n, "timer-channel", -1);
	u64_t rate;

	if(!search_clk(clk))
		return NULL;

	if(channel < 0 || channel > 3)
		return NULL;

	pdat = malloc(sizeof(struct cs_samsung_timer_pdata_t));
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
	pdat->channel = channel;

	clk_enable(pdat->clk);
	rate = samsung_timer_calc_tin(pdat->virt, pdat->clk, pdat->channel, 13);
	clocksource_calc_mult_shift(&cs->mult, &cs->shift, rate, 1000000000ULL, 10);
	cs->name = alloc_device_name(dt_read_name(n), -1);
	cs->mask = CLOCKSOURCE_MASK(32),
	cs->read = cs_samsung_timer_read,
	cs->priv = pdat;

	samsung_timer_enable(pdat->virt, pdat->channel, 0);
	samsung_timer_count(pdat->virt, pdat->channel, 0xffffffff);
	samsung_timer_start(pdat->virt, pdat->channel, 0);

	if(!register_clocksource(&dev, cs))
	{
		samsung_timer_stop(pdat->virt, pdat->channel);
		samsung_timer_disable(pdat->virt, pdat->channel);
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

static void cs_samsung_timer_remove(struct device_t * dev)
{
	struct clocksource_t * cs = (struct clocksource_t *)dev->priv;
	struct cs_samsung_timer_pdata_t * pdat = (struct cs_samsung_timer_pdata_t *)cs->priv;

	if(cs && unregister_clocksource(cs))
	{
		samsung_timer_stop(pdat->virt, pdat->channel);
		samsung_timer_disable(pdat->virt, pdat->channel);
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(cs->name);
		free(cs->priv);
		free(cs);
	}
}

static void cs_samsung_timer_suspend(struct device_t * dev)
{
}

static void cs_samsung_timer_resume(struct device_t * dev)
{
}

static struct driver_t cs_samsung_timer = {
	.name		= "cs-samsung-timer",
	.probe		= cs_samsung_timer_probe,
	.remove		= cs_samsung_timer_remove,
	.suspend	= cs_samsung_timer_suspend,
	.resume		= cs_samsung_timer_resume,
};

static __init void cs_samsung_timer_driver_init(void)
{
	register_driver(&cs_samsung_timer);
}

static __exit void cs_samsung_timer_driver_exit(void)
{
	unregister_driver(&cs_samsung_timer);
}

driver_initcall(cs_samsung_timer_driver_init);
driver_exitcall(cs_samsung_timer_driver_exit);
