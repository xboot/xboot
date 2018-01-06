/*
 * driver/ce-sp804.c
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
#include <interrupt/interrupt.h>
#include <clockevent/clockevent.h>

#define TIMER_LOAD(x)	(((x) * 0x20) + 0x00)
#define TIMER_VALUE(x)	(((x) * 0x20) + 0x04)
#define TIMER_CTRL(x)	(((x) * 0x20) + 0x08)
#define TIMER_ICLR(x)	(((x) * 0x20) + 0x0c)
#define TIMER_RIS(x)	(((x) * 0x20) + 0x10)
#define TIMER_MIS(x)	(((x) * 0x20) + 0x14)
#define TIMER_BGLOAD(x)	(((x) * 0x20) + 0x18)

struct ce_sp804_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int irq;
	int channel;
};

static void ce_sp804_interrupt(void * data)
{
	struct clockevent_t * ce = (struct clockevent_t *)data;
	struct ce_sp804_pdata_t * pdat = (struct ce_sp804_pdata_t *)ce->priv;
	write32(pdat->virt + TIMER_ICLR(pdat->channel), 0x0);
	ce->handler(ce, ce->data);
}

static bool_t ce_sp804_next(struct clockevent_t * ce, u64_t evt)
{
	struct ce_sp804_pdata_t * pdat = (struct ce_sp804_pdata_t *)ce->priv;

	write32(pdat->virt + TIMER_LOAD(pdat->channel), (evt & 0xffffffff));
	write32(pdat->virt + TIMER_VALUE(pdat->channel), (evt & 0xffffffff));
	write32(pdat->virt + TIMER_CTRL(pdat->channel), read32(pdat->virt + TIMER_CTRL(pdat->channel)) | (1 << 7));
	return TRUE;
}

static struct device_t * ce_sp804_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ce_sp804_pdata_t * pdat;
	struct clockevent_t * ce;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int irq = dt_read_int(n, "interrupt", -1);
	int channel = dt_read_int(n, "timer-channel", -1);
	u32_t id = (((read32(virt + 0xfec) & 0xff) << 24) |
				((read32(virt + 0xfe8) & 0xff) << 16) |
				((read32(virt + 0xfe4) & 0xff) <<  8) |
				((read32(virt + 0xfe0) & 0xff) <<  0));

	if(((id >> 12) & 0xff) != 0x41 || (id & 0xfff) != 0x804)
		return NULL;

	if(!search_clk(clk))
		return NULL;

	if(!irq_is_valid(irq))
		return NULL;

	if(channel < 0 || channel > 1)
		return NULL;

	pdat = malloc(sizeof(struct ce_sp804_pdata_t));
	if(!pdat)
		return NULL;

	ce = malloc(sizeof(struct clockevent_t));
	if(!ce)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->irq = irq;
	pdat->channel = channel;

	clk_enable(pdat->clk);
	clockevent_calc_mult_shift(ce, clk_get_rate(pdat->clk), 10);
	ce->name = alloc_device_name(dt_read_name(n), -1);
	ce->min_delta_ns = clockevent_delta2ns(ce, 0x1);
	ce->max_delta_ns = clockevent_delta2ns(ce, 0xffffffff);
	ce->next = ce_sp804_next,
	ce->priv = pdat;

	request_irq(pdat->irq, ce_sp804_interrupt, IRQ_TYPE_NONE, ce);
	write32(pdat->virt + TIMER_CTRL(pdat->channel), 0);
	write32(pdat->virt + TIMER_LOAD(pdat->channel), 0);
	write32(pdat->virt + TIMER_VALUE(pdat->channel), 0);
	write32(pdat->virt + TIMER_CTRL(pdat->channel), (1 << 0) | (1 << 1) | (0 << 2) | (1 << 5) | (0 << 6));

	if(!register_clockevent(&dev, ce))
	{
		write32(pdat->virt + TIMER_CTRL(pdat->channel), 0);
		clk_disable(pdat->clk);
		free_irq(pdat->irq);
		free(pdat->clk);

		free_device_name(ce->name);
		free(ce->priv);
		free(ce);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void ce_sp804_remove(struct device_t * dev)
{
	struct clockevent_t * ce = (struct clockevent_t *)dev->priv;
	struct ce_sp804_pdata_t * pdat = (struct ce_sp804_pdata_t *)ce->priv;

	if(ce && unregister_clockevent(ce))
	{
		write32(pdat->virt + TIMER_CTRL(pdat->channel), 0);
		clk_disable(pdat->clk);
		free_irq(pdat->irq);
		free(pdat->clk);

		free_device_name(ce->name);
		free(ce->priv);
		free(ce);
	}
}

static void ce_sp804_suspend(struct device_t * dev)
{
}

static void ce_sp804_resume(struct device_t * dev)
{
}

static struct driver_t ce_sp804 = {
	.name		= "ce-sp804",
	.probe		= ce_sp804_probe,
	.remove		= ce_sp804_remove,
	.suspend	= ce_sp804_suspend,
	.resume		= ce_sp804_resume,
};

static __init void ce_sp804_driver_init(void)
{
	register_driver(&ce_sp804);
}

static __exit void ce_sp804_driver_exit(void)
{
	unregister_driver(&ce_sp804);
}

driver_initcall(ce_sp804_driver_init);
driver_exitcall(ce_sp804_driver_exit);
