/*
 * driver/cs-f1c100s-timer.c
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
#include <clocksource/clocksource.h>

#define TIMER_IRQ_EN	(0x00)
#define TIMER_IRQ_STA	(0x04)
#define TIMER_CTRL(x)	((x + 1) * 0x10 + 0x00)
#define TIMER_INTV(x)	((x + 1) * 0x10 + 0x04)
#define TIMER_CUR(x)	((x + 1) * 0x10 + 0x08)

struct cs_f1c100s_timer_pdata_t
{
	virtual_addr_t virt;
	char * clk;
};

static u64_t cs_f1c100s_timer_read(struct clocksource_t * cs)
{
	struct cs_f1c100s_timer_pdata_t * pdat = (struct cs_f1c100s_timer_pdata_t *)cs->priv;
	return (u64_t)(0xffffffff - read32(pdat->virt + TIMER_CUR(1)));
}

static struct device_t * cs_f1c100s_timer_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct cs_f1c100s_timer_pdata_t * pdat;
	struct clocksource_t * cs;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	u32_t val;

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct cs_f1c100s_timer_pdata_t));
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
	cs->mask = CLOCKSOURCE_MASK(32);
	cs->read = cs_f1c100s_timer_read;
	cs->priv = pdat;

	write32(pdat->virt + TIMER_IRQ_EN, read32(pdat->virt + TIMER_IRQ_EN) & ~(1 << 1));
	write32(pdat->virt + TIMER_IRQ_STA, 1 << 1);
	write32(pdat->virt + TIMER_INTV(1), 0xffffffff);
	val = read32(pdat->virt + TIMER_CTRL(1));
	val &= ~((0x1 << 7) | (0x3 << 0));
	val |= (0x0 << 7) | (0x1 << 0);
	write32(pdat->virt + TIMER_CTRL(1), val);

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

static void cs_f1c100s_timer_remove(struct device_t * dev)
{
	struct clocksource_t * cs = (struct clocksource_t *)dev->priv;
	struct cs_f1c100s_timer_pdata_t * pdat = (struct cs_f1c100s_timer_pdata_t *)cs->priv;

	if(cs && unregister_clocksource(cs))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(cs->name);
		free(cs->priv);
		free(cs);
	}
}

static void cs_f1c100s_timer_suspend(struct device_t * dev)
{
}

static void cs_f1c100s_timer_resume(struct device_t * dev)
{
}

static struct driver_t cs_f1c100s_timer = {
	.name		= "cs-f1c100s-timer",
	.probe		= cs_f1c100s_timer_probe,
	.remove		= cs_f1c100s_timer_remove,
	.suspend	= cs_f1c100s_timer_suspend,
	.resume		= cs_f1c100s_timer_resume,
};

static __init void cs_f1c100s_timer_driver_init(void)
{
	register_driver(&cs_f1c100s_timer);
}

static __exit void cs_f1c100s_timer_driver_exit(void)
{
	unregister_driver(&cs_f1c100s_timer);
}

driver_initcall(cs_f1c100s_timer_driver_init);
driver_exitcall(cs_f1c100s_timer_driver_exit);
