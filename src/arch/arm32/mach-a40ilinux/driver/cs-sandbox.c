/*
 * driver/cs-sandbox.c
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
#include <clocksource/clocksource.h>
#include <sandbox.h>

static u64_t cs_sandbox_read(struct clocksource_t * cs)
{
	return sandbox_timer_count();
}

static struct device_t * cs_sandbox_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clocksource_t * cs;
	struct device_t * dev;

	cs = malloc(sizeof(struct clocksource_t));
	if(!cs)
		return NULL;

	if(sandbox_timer_frequency() != 1000000000ULL)
	{
		clocksource_calc_mult_shift(&cs->mult, &cs->shift, sandbox_timer_frequency(), 1000000000ULL, 60);
	}
	else
	{
		cs->mult = 1;
		cs->shift = 0;
	}
	cs->name = alloc_device_name(dt_read_name(n), -1);
	cs->mask = CLOCKSOURCE_MASK(63);
	cs->read = cs_sandbox_read;
	cs->priv = 0;

	if(!(dev = register_clocksource(cs, drv)))
	{
		free_device_name(cs->name);
		free(cs->priv);
		free(cs);
		return NULL;
	}
	return dev;
}

static void cs_sandbox_remove(struct device_t * dev)
{
	struct clocksource_t * cs = (struct clocksource_t *)dev->priv;

	if(cs)
	{
		unregister_clocksource(cs);
		free_device_name(cs->name);
		free(cs->priv);
		free(cs);
	}
}

static void cs_sandbox_suspend(struct device_t * dev)
{
}

static void cs_sandbox_resume(struct device_t * dev)
{
}

static struct driver_t cs_sandbox = {
	.name		= "cs-sandbox",
	.probe		= cs_sandbox_probe,
	.remove		= cs_sandbox_remove,
	.suspend	= cs_sandbox_suspend,
	.resume		= cs_sandbox_resume,
};

static __init void cs_sandbox_driver_init(void)
{
	register_driver(&cs_sandbox);
}

static __exit void cs_sandbox_driver_exit(void)
{
	unregister_driver(&cs_sandbox);
}

driver_initcall(cs_sandbox_driver_init);
driver_exitcall(cs_sandbox_driver_exit);
