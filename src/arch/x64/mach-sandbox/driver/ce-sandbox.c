/*
 * driver/ce-sandbox.c
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
#include <clockevent/clockevent.h>
#include <sandbox.h>

static void ce_sandbox_interrupt(void * data)
{
	struct clockevent_t * ce = (struct clockevent_t *)data;
	ce->handler(ce, ce->data);
}

static bool_t ce_sandbox_next(struct clockevent_t * ce, u64_t evt)
{
	sandbox_timer_next(evt & 0x7fffffffffffffff, ce_sandbox_interrupt, ce);
	return TRUE;
}

static struct device_t * ce_sandbox_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clockevent_t * ce;
	struct device_t * dev;

	ce = malloc(sizeof(struct clockevent_t));
	if(!ce)
		return NULL;

	if(sandbox_timer_frequency() != 1000000000ULL)
	{
		clockevent_calc_mult_shift(ce, sandbox_timer_frequency(), 60);
	}
	else
	{
		ce->mult = 1;
		ce->shift = 0;
	}
	ce->name = alloc_device_name(dt_read_name(n), -1);
	ce->min_delta_ns = clockevent_delta2ns(ce, 0x1);
	ce->max_delta_ns = clockevent_delta2ns(ce, 0x7fffffffffffffff);
	ce->next = ce_sandbox_next;
	ce->priv = 0;
	sandbox_timer_init();

	if(!(dev = register_clockevent(ce, drv)))
	{
		sandbox_timer_exit();
		free_device_name(ce->name);
		free(ce->priv);
		free(ce);
		return NULL;
	}
	return dev;
}

static void ce_sandbox_remove(struct device_t * dev)
{
	struct clockevent_t * ce = (struct clockevent_t *)dev->priv;

	if(ce)
	{
		unregister_clockevent(ce);
		sandbox_timer_exit();
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
