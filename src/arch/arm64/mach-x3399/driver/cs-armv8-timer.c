/*
 * driver/cs-armv8-timer.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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
#include <clocksource/clocksource.h>

static u64_t cs_armv8_timer_read(struct clocksource_t * cs)
{
	return arm64_timer_read();
}

static struct device_t * cs_armv8_timer_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clocksource_t * cs;
	struct device_t * dev;
	s64_t rate = (s64_t)dt_read_long(n, "clock-frequency", -1);
	if(rate <= 0)
		rate = arm64_timer_frequecy();

	cs = malloc(sizeof(struct clocksource_t));
	if(!cs)
		return NULL;

	cs->name = alloc_device_name(dt_read_name(n), -1);
	cs->mask = CLOCKSOURCE_MASK(64),
	cs->read = cs_armv8_timer_read,
	cs->priv = NULL;
	clocksource_calc_mult_shift(&cs->mult, &cs->shift, (u64_t)rate, 1000000000ULL, 10);
	arm64_timer_start();

	if(!register_clocksource(&dev, cs))
	{
		free_device_name(cs->name);
		free(cs->priv);
		free(cs);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void cs_armv8_timer_remove(struct device_t * dev)
{
	struct clocksource_t * cs = (struct clocksource_t *)dev->priv;

	if(cs && unregister_clocksource(cs))
	{
		free_device_name(cs->name);
		free(cs->priv);
		free(cs);
	}
}

static void cs_armv8_timer_suspend(struct device_t * dev)
{
}

static void cs_armv8_timer_resume(struct device_t * dev)
{
}

static struct driver_t cs_armv8_timer = {
	.name		= "cs-armv8-timer",
	.probe		= cs_armv8_timer_probe,
	.remove		= cs_armv8_timer_remove,
	.suspend	= cs_armv8_timer_suspend,
	.resume		= cs_armv8_timer_resume,
};

static __init void cs_armv8_timer_driver_init(void)
{
	register_driver(&cs_armv8_timer);
}

static __exit void cs_armv8_timer_driver_exit(void)
{
	unregister_driver(&cs_armv8_timer);
}

driver_initcall(cs_armv8_timer_driver_init);
driver_exitcall(cs_armv8_timer_driver_exit);
