/*
 * driver/adc-t113.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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
	GPADC_SR_CON		= 0x00,
	GPADC_CTRL			= 0x04,
	GPADC_CS_EN			= 0x08,
	GPADC_FIFO_INTC		= 0x0c,
	GPADC_FIFO_INTS		= 0x10,
	GPADC_FIFO_DATA		= 0x14,
	GPADC_CDATA			= 0x18,
	GPADC_DATAL_INTC	= 0x20,
	GPADC_DATAH_INTC	= 0x24,
	GPADC_DATA_INTC		= 0x28,
	GPADC_DATAL_INTS	= 0x30,
	GPADC_DATAH_INTS	= 0x34,
	GPADC_DATA_INTS		= 0x38,
	GPADC_CH0_CMP_DATA	= 0x40,
	GPADC_CH1_CMP_DATA	= 0x44,
	GPADC_CH0_DATA		= 0x80,
	GPADC_CH1_DATA		= 0x84,
};

struct adc_t113_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int reset;
};

static u32_t adc_t113_read(struct adc_t * adc, int channel)
{
	struct adc_t113_pdata_t * pdat = (struct adc_t113_pdata_t *)adc->priv;
	u32_t val;

	val = read32(pdat->virt + GPADC_CS_EN);
	val |= (1 << channel);
	write32(pdat->virt + GPADC_CS_EN, val);

	val = read32(pdat->virt + GPADC_CTRL);
	val &= ~(0x3 << 18);
	val |= (0x2 << 18);
	write32(pdat->virt + GPADC_CTRL, val);

	write32(pdat->virt + GPADC_DATA_INTC, 0);
	write32(pdat->virt + GPADC_DATA_INTS, 1);

	val = read32(pdat->virt + GPADC_CTRL);
	val |= (1 << 16);
	write32(pdat->virt + GPADC_CTRL, val);

	while((read32(pdat->virt + GPADC_DATA_INTS) & (1 << channel)) == 0);
	return read32(pdat->virt + GPADC_CH0_DATA + (channel << 2));
}

static struct device_t * adc_t113_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct adc_t113_pdata_t * pdat;
	struct adc_t * adc;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct adc_t113_pdata_t));
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

	adc->name = alloc_device_name(dt_read_name(n), -1);
	adc->vreference = dt_read_int(n, "reference-voltage", 1800000);
	adc->resolution = 12;
	adc->nchannel = 2;
	adc->read = adc_t113_read;
	adc->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
	{
		reset_assert(pdat->reset);
		udelay(1);
		reset_deassert(pdat->reset);
		udelay(1);
	}
	write32(pdat->virt + GPADC_SR_CON, 0x01df002f);
	write32(pdat->virt + GPADC_CTRL, 0);

	if(!(dev = register_adc(adc, drv)))
	{
		write32(pdat->virt + GPADC_CTRL, 0);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(adc->name);
		free(adc->priv);
		free(adc);
		return NULL;
	}
	return dev;
}

static void adc_t113_remove(struct device_t * dev)
{
	struct adc_t * adc = (struct adc_t *)dev->priv;
	struct adc_t113_pdata_t * pdat = (struct adc_t113_pdata_t *)adc->priv;

	if(adc)
	{
		unregister_adc(adc);
		write32(pdat->virt + GPADC_CTRL, 0);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(adc->name);
		free(adc->priv);
		free(adc);
	}
}

static void adc_t113_suspend(struct device_t * dev)
{
}

static void adc_t113_resume(struct device_t * dev)
{
}

static struct driver_t adc_t113 = {
	.name		= "adc-t113",
	.probe		= adc_t113_probe,
	.remove		= adc_t113_remove,
	.suspend	= adc_t113_suspend,
	.resume		= adc_t113_resume,
};

static __init void adc_t113_driver_init(void)
{
	register_driver(&adc_t113);
}

static __exit void adc_t113_driver_exit(void)
{
	unregister_driver(&adc_t113);
}

driver_initcall(adc_t113_driver_init);
driver_exitcall(adc_t113_driver_exit);
