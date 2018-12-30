/*
 * driver/ce-armv8-timer.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
