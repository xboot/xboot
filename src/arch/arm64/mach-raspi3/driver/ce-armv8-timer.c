/*
 * driver/ce-armv8-timer.c
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
#include <arm64.h>
#include <interrupt/interrupt.h>
#include <clockevent/clockevent.h>

struct ce_armv8_timer_pdata_t
{
	int irq;
};

static void ce_armv8_timer_interrupt(void * data)
{
	struct clockevent_t * ce = (struct clockevent_t *)data;
	ce->handler(ce, ce->data);
}

static bool_t ce_armv8_timer_next(struct clockevent_t * ce, u64_t evt)
{
	arm64_timer_compare(evt);
	arm64_timer_interrupt_enable();
	arm64_timer_start();
	return TRUE;
}

static struct device_t * ce_armv8_timer_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ce_armv8_timer_pdata_t * pdat;
	struct clockevent_t * ce;
	struct device_t * dev;
	s64_t rate = (s64_t)dt_read_long(n, "clock-frequency", -1);
	int irq = dt_read_int(n, "interrupt", -1);

	if(!irq_is_valid(irq))
		return NULL;

	if(rate <= 0)
		rate = arm64_timer_frequecy();

	pdat = malloc(sizeof(struct ce_armv8_timer_pdata_t));
	if(!pdat)
		return NULL;

	ce = malloc(sizeof(struct clockevent_t));
	if(!ce)
	{
		free(pdat);
		return NULL;
	}

	pdat->irq = irq;

	clockevent_calc_mult_shift(ce, rate, 10);
	ce->name = alloc_device_name(dt_read_name(n), -1);
	ce->min_delta_ns = clockevent_delta2ns(ce, 0x1);
	ce->max_delta_ns = clockevent_delta2ns(ce, 0xffffffff);
	ce->next = ce_armv8_timer_next;
	ce->priv = pdat;

	if(!request_irq(pdat->irq, ce_armv8_timer_interrupt, IRQ_TYPE_NONE, ce))
	{
		free(ce->priv);
		free(ce);
		return NULL;
	}
	arm64_timer_compare(0xffffffff);
	arm64_timer_interrupt_disable();
	arm64_timer_stop();

	if(!register_clockevent(&dev, ce))
	{
		free_irq(pdat->irq);

		free_device_name(ce->name);
		free(ce->priv);
		free(ce);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void ce_armv8_timer_remove(struct device_t * dev)
{
	struct clockevent_t * ce = (struct clockevent_t *)dev->priv;
	struct ce_armv8_timer_pdata_t * pdat = (struct ce_armv8_timer_pdata_t *)ce->priv;

	if(ce && unregister_clockevent(ce))
	{
		free_irq(pdat->irq);

		free_device_name(ce->name);
		free(ce->priv);
		free(ce);
	}
}

static void ce_armv8_timer_suspend(struct device_t * dev)
{
}

static void ce_armv8_timer_resume(struct device_t * dev)
{
}

static struct driver_t ce_armv8_timer = {
	.name		= "ce-armv8-timer",
	.probe		= ce_armv8_timer_probe,
	.remove		= ce_armv8_timer_remove,
	.suspend	= ce_armv8_timer_suspend,
	.resume		= ce_armv8_timer_resume,
};

static __init void ce_armv8_timer_driver_init(void)
{
	register_driver(&ce_armv8_timer);
}

static __exit void ce_armv8_timer_driver_exit(void)
{
	unregister_driver(&ce_armv8_timer);
}

driver_initcall(ce_armv8_timer_driver_init);
driver_exitcall(ce_armv8_timer_driver_exit);
