/*
 * driver/ce-clint-timer.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <riscv64.h>
#include <clk/clk.h>
#include <interrupt/interrupt.h>
#include <clockevent/clockevent.h>

extern void hook_core_interrupt(int cause, void (*func)(void *), void * data);

#define CLINT_MSIP(id)		(0x0000 + ((id) * 4))
#define CLINT_MTIMECMP(id)	(0x4000 + ((id) * 8))
#define CLINT_MTIME			(0xbff8)

struct ce_clint_timer_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int id;
};

static void ce_clint_timer_interrupt(void * data)
{
	struct clockevent_t * ce = (struct clockevent_t *)data;
	struct ce_clint_timer_pdata_t * pdat = (struct ce_clint_timer_pdata_t *)ce->priv;
	write64(pdat->virt + CLINT_MTIMECMP(pdat->id), 0xffffffffffffffff);
	csr_clear(mie, 1 << 7);
	ce->handler(ce, ce->data);
}

static bool_t ce_clint_timer_next(struct clockevent_t * ce, u64_t evt)
{
	struct ce_clint_timer_pdata_t * pdat = (struct ce_clint_timer_pdata_t *)ce->priv;
	u64_t last = read64(pdat->virt + CLINT_MTIME) + evt;
	write64(pdat->virt + CLINT_MTIMECMP(pdat->id), last);
	csr_set(mie, 1 << 7);
	return TRUE;
}

static struct device_t * ce_clint_timer_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ce_clint_timer_pdata_t * pdat;
	struct clockevent_t * ce;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct ce_clint_timer_pdata_t));
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
	pdat->id = csr_read_mhartid();

	clk_enable(pdat->clk);
	clockevent_calc_mult_shift(ce, clk_get_rate(pdat->clk), 10);
	ce->name = alloc_device_name(dt_read_name(n), -1);
	ce->min_delta_ns = clockevent_delta2ns(ce, 0x1);
	ce->max_delta_ns = clockevent_delta2ns(ce, 0xffffffffffffffff);
	ce->next = ce_clint_timer_next;
	ce->priv = pdat;

	hook_core_interrupt(7, ce_clint_timer_interrupt, ce);
	write64(pdat->virt + CLINT_MTIMECMP(pdat->id), 0xffffffffffffffff);
	csr_clear(mie, 1 << 7);
	csr_set(mstatus, (0x1 << 3));

	if(!register_clockevent(&dev, ce))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(ce->name);
		free(ce->priv);
		free(ce);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void ce_clint_timer_remove(struct device_t * dev)
{
	struct clockevent_t * ce = (struct clockevent_t *)dev->priv;
	struct ce_clint_timer_pdata_t * pdat = (struct ce_clint_timer_pdata_t *)ce->priv;

	if(ce && unregister_clockevent(ce))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(ce->name);
		free(ce->priv);
		free(ce);
	}
}

static void ce_clint_timer_suspend(struct device_t * dev)
{
}

static void ce_clint_timer_resume(struct device_t * dev)
{
}

static struct driver_t ce_clint_timer = {
	.name		= "ce-clint-timer",
	.probe		= ce_clint_timer_probe,
	.remove		= ce_clint_timer_remove,
	.suspend	= ce_clint_timer_suspend,
	.resume		= ce_clint_timer_resume,
};

static __init void ce_clint_timer_driver_init(void)
{
	register_driver(&ce_clint_timer);
}

static __exit void ce_clint_timer_driver_exit(void)
{
	unregister_driver(&ce_clint_timer);
}

driver_initcall(ce_clint_timer_driver_init);
driver_exitcall(ce_clint_timer_driver_exit);
