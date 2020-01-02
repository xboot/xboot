/*
 * driver/cs-sp804.c
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
#include <clk/clk.h>
#include <clocksource/clocksource.h>

#define TIMER_LOAD(x)	(((x) * 0x20) + 0x00)
#define TIMER_VALUE(x)	(((x) * 0x20) + 0x04)
#define TIMER_CTRL(x)	(((x) * 0x20) + 0x08)
#define TIMER_ICLR(x)	(((x) * 0x20) + 0x0c)
#define TIMER_RIS(x)	(((x) * 0x20) + 0x10)
#define TIMER_MIS(x)	(((x) * 0x20) + 0x14)
#define TIMER_BGLOAD(x)	(((x) * 0x20) + 0x18)

struct cs_sp804_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int channel;
};

static u64_t cs_sp804_read(struct clocksource_t * cs)
{
	struct cs_sp804_pdata_t * pdat = (struct cs_sp804_pdata_t *)cs->priv;
	return (u64_t)(0xffffffff - read32(pdat->virt + TIMER_VALUE(pdat->channel)));
}

static struct device_t * cs_sp804_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct cs_sp804_pdata_t * pdat;
	struct clocksource_t * cs;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int channel = dt_read_int(n, "timer-channel", -1);
	u32_t id = (((read32(virt + 0xfec) & 0xff) << 24) |
				((read32(virt + 0xfe8) & 0xff) << 16) |
				((read32(virt + 0xfe4) & 0xff) <<  8) |
				((read32(virt + 0xfe0) & 0xff) <<  0));

	if(((id >> 12) & 0xff) != 0x41 || (id & 0xfff) != 0x804)
		return NULL;

	if(!search_clk(clk))
		return NULL;

	if(channel < 0 || channel > 1)
		return NULL;

	pdat = malloc(sizeof(struct cs_sp804_pdata_t));
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
	cs->name = alloc_device_name(dt_read_name(n), -1);
	cs->mask = CLOCKSOURCE_MASK(32);
	cs->shift = 20;
	cs->mult = clocksource_hz2mult(clk_get_rate(pdat->clk), cs->shift);
	cs->read = cs_sp804_read;
	cs->priv = pdat;

	write32(pdat->virt + TIMER_CTRL(pdat->channel), 0);
	write32(pdat->virt + TIMER_LOAD(pdat->channel), 0xffffffff);
	write32(pdat->virt + TIMER_VALUE(pdat->channel), 0xffffffff);
	write32(pdat->virt + TIMER_CTRL(pdat->channel), (1 << 1) | (1 << 6) | (1 << 7));

	if(!(dev = register_clocksource(cs, drv)))
	{
		write32(pdat->virt + TIMER_CTRL(pdat->channel), 0);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(cs->name);
		free(cs->priv);
		free(cs);
		return NULL;
	}
	return dev;
}

static void cs_sp804_remove(struct device_t * dev)
{
	struct clocksource_t * cs = (struct clocksource_t *)dev->priv;
	struct cs_sp804_pdata_t * pdat = (struct cs_sp804_pdata_t *)cs->priv;

	if(cs)
	{
		unregister_clocksource(cs);
		write32(pdat->virt + TIMER_CTRL(pdat->channel), 0);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(cs->name);
		free(cs->priv);
		free(cs);
	}
}

static void cs_sp804_suspend(struct device_t * dev)
{
}

static void cs_sp804_resume(struct device_t * dev)
{
}

static struct driver_t cs_sp804 = {
	.name		= "cs-sp804",
	.probe		= cs_sp804_probe,
	.remove		= cs_sp804_remove,
	.suspend	= cs_sp804_suspend,
	.resume		= cs_sp804_resume,
};

static __init void cs_sp804_driver_init(void)
{
	register_driver(&cs_sp804);
}

static __exit void cs_sp804_driver_exit(void)
{
	unregister_driver(&cs_sp804);
}

driver_initcall(cs_sp804_driver_init);
driver_exitcall(cs_sp804_driver_exit);
