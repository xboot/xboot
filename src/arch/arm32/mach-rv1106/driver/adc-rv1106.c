/*
 * driver/adc-rv1106.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
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
#include <reset/reset.h>
#include <adc/adc.h>

enum {
	SARADC_CONV_CON 	= 0x000,
	SARADC_T_PD_SOC 	= 0x004,
	SARADC_T_DAS_SOC 	= 0x00c,
	SARADC_END_INT_EN 	= 0x104,
	SARADC_ST_CON 		= 0x108,
	SARADC_STATUS 		= 0x10c,
	SARADC_END_INT_ST 	= 0x110,
	SARADC_DATA_BASE 	= 0x120,
};

struct adc_rv1106_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int reset;
};

static u32_t adc_rv1106_read(struct adc_t * adc, int channel)
{
	struct adc_rv1106_pdata_t * pdat = (struct adc_rv1106_pdata_t *)adc->priv;
	u32_t val;

	reset_assert(pdat->reset);
	udelay(1);
	reset_deassert(pdat->reset);
	udelay(1);

	write32(pdat->virt + SARADC_T_PD_SOC, 0x20);
	write32(pdat->virt + SARADC_T_DAS_SOC, 0xc);
	val = ((0x1 << 0) << 16) | (0x1 << 0);
	write32(pdat->virt + SARADC_END_INT_EN, val);
	val = (1 << 4) | (1 << 5) | channel;
	write32(pdat->virt + SARADC_CONV_CON, (val << 16) | val);

	ktime_t timeout = ktime_add_ms(ktime_get(), 1);
	while(((read32(pdat->virt + SARADC_END_INT_ST) & 0x1) == 0x0) || ktime_before(ktime_get(), timeout));
	write32(pdat->virt + SARADC_END_INT_ST, 0x1);
	return read32(pdat->virt + SARADC_DATA_BASE + (channel << 2));
}

static struct device_t * adc_rv1106_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct adc_rv1106_pdata_t * pdat;
	struct adc_t * adc;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct adc_rv1106_pdata_t));
	if(!pdat)
		return NULL;

	adc = malloc(sizeof(struct adc_t));
	if(!adc)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->reset = dt_read_int(n, "reset", -1);

	adc->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	adc->vreference = dt_read_int(n, "reference-voltage", 1800000);
	adc->resolution = 10;
	adc->nchannel = 2;
	adc->read = adc_rv1106_read;
	adc->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
	{
		reset_assert(pdat->reset);
		udelay(1);
		reset_deassert(pdat->reset);
		udelay(1);
	}

	if(!(dev = register_adc(adc, drv)))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(adc->name);
		free(adc->priv);
		free(adc);
		return NULL;
	}
	return dev;
}

static void adc_rv1106_remove(struct device_t * dev)
{
	struct adc_t * adc = (struct adc_t *)dev->priv;
	struct adc_rv1106_pdata_t * pdat = (struct adc_rv1106_pdata_t *)adc->priv;

	if(adc)
	{
		unregister_adc(adc);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(adc->name);
		free(adc->priv);
		free(adc);
	}
}

static void adc_rv1106_suspend(struct device_t * dev)
{
}

static void adc_rv1106_resume(struct device_t * dev)
{
}

static struct driver_t adc_rv1106 = {
	.name		= "adc-rv1106",
	.probe		= adc_rv1106_probe,
	.remove		= adc_rv1106_remove,
	.suspend	= adc_rv1106_suspend,
	.resume		= adc_rv1106_resume,
};

static __init void adc_rv1106_driver_init(void)
{
	register_driver(&adc_rv1106);
}

static __exit void adc_rv1106_driver_exit(void)
{
	unregister_driver(&adc_rv1106);
}

driver_initcall(adc_rv1106_driver_init);
driver_exitcall(adc_rv1106_driver_exit);
