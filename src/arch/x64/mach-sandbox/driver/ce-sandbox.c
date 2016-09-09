/*
 * driver/ce-sandbox.c
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
#include <clockevent/clockevent.h>
#include <sandbox.h>

static void ce_sandbox_interrupt(void * data)
{
	struct clockevent_t * ce = (struct clockevent_t *)data;
	ce->handler(ce, ce->data);
}

static bool_t ce_sandbox_next(struct clockevent_t * ce, u64_t evt)
{
	sandbox_sdl_timer_set_next((evt & 0x7fffffff), ce_sandbox_interrupt, ce);
	return TRUE;
}

static struct device_t * ce_sandbox_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clockevent_t * ce;
	struct device_t * dev;

	ce = malloc(sizeof(struct clockevent_t));
	if(!ce)
		return NULL;

	clockevent_calc_mult_shift(ce, 1000, 10);
	ce->name = alloc_device_name(dt_read_name(n), -1);
	ce->min_delta_ns = clockevent_delta2ns(ce, 0x1);
	ce->max_delta_ns = clockevent_delta2ns(ce, 0x7fffffff);
	ce->next = ce_sandbox_next,
	ce->priv = 0;
	sandbox_sdl_timer_init();

	if(!register_clockevent(&dev, ce))
	{
		free_device_name(ce->name);
		free(ce->priv);
		free(ce);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void ce_sandbox_remove(struct device_t * dev)
{
	struct clockevent_t * ce = (struct clockevent_t *)dev->priv;

	if(ce && unregister_clockevent(ce))
	{
		free_device_name(ce->name);
		free(ce->priv);
		free(ce);
	}
}

static void ce_sandbox_suspend(struct device_t * dev)
{
}

static void ce_sandbox_resume(struct device_t * dev)
{
}

static struct driver_t ce_sandbox = {
	.name		= "ce-sandbox",
	.probe		= ce_sandbox_probe,
	.remove		= ce_sandbox_remove,
	.suspend	= ce_sandbox_suspend,
	.resume		= ce_sandbox_resume,
};

static __init void ce_sandbox_driver_init(void)
{
	register_driver(&ce_sandbox);
}

static __exit void ce_sandbox_driver_exit(void)
{
	unregister_driver(&ce_sandbox);
}

driver_initcall(ce_sandbox_driver_init);
driver_exitcall(ce_sandbox_driver_exit);
