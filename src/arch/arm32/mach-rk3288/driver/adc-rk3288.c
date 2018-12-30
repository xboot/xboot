/*
 * driver/adc-rk3288.c
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
#include <clk/clk.h>
#include <adc/adc.h>

enum {
	SARADC_DATA		= 0x00,
	SARADC_STAT		= 0x04,
	SARADC_CTRL		= 0x08,
	SARADC_DELAY	= 0x0c,
};

struct adc_rk3288_pdata_t
{
	virtual_addr_t virt;
	char * clk;
};

static u32_t adc_rk3288_read(struct adc_t * adc, int channel)
{
	struct adc_rk3288_pdata_t * pdat = (struct adc_rk3288_pdata_t *)adc->priv;
	u32_t value;

	write32(pdat->virt + SARADC_CTRL, 0);
	udelay(1);
	write32(pdat->virt + SARADC_CTRL, (0x1 << 5) | (0x1 << 3) | ((channel & 0x7) << 0));
	udelay(1);
	while((read32(pdat->virt + SARADC_CTRL) & (0x1 << 6)) == 0x00);
	value = read32(pdat->virt + SARADC_DATA) & 0x3ff;
	write32(pdat->virt + SARADC_CTRL, 0);

	return value;
}

static struct device_t * adc_rk3288_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct adc_rk3288_pdata_t * pdat;
	struct adc_t * adc;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct adc_rk3288_pdata_t));
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

	adc->name = alloc_device_name(dt_read_name(n), -1);
	adc->vreference = dt_read_int(n, "reference-voltage", 1800000);
	adc->resolution = 10;
	adc->nchannel = 3;
	adc->read = adc_rk3288_read;
	adc->priv = pdat;

	clk_enable(pdat->clk);
	write32(pdat->virt + SARADC_CTRL, 0);

	if(!register_adc(&dev, adc))
	{
		write32(pdat->virt + SARADC_CTRL, 0);
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(adc->name);
		free(adc->priv);
		free(adc);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void adc_rk3288_remove(struct device_t * dev)
{
	struct adc_t * adc = (struct adc_t *)dev->priv;
	struct adc_rk3288_pdata_t * pdat = (struct adc_rk3288_pdata_t *)adc->priv;

	if(adc && unregister_adc(adc))
	{
		write32(pdat->virt + SARADC_CTRL, 0);
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(adc->name);
		free(adc->priv);
		free(adc);
	}
}

static void adc_rk3288_suspend(struct device_t * dev)
{
}

static void adc_rk3288_resume(struct device_t * dev)
{
}

static struct driver_t adc_rk3288 = {
	.name		= "adc-rk3288",
	.probe		= adc_rk3288_probe,
	.remove		= adc_rk3288_remove,
	.suspend	= adc_rk3288_suspend,
	.resume		= adc_rk3288_resume,
};

static __init void adc_rk3288_driver_init(void)
{
	register_driver(&adc_rk3288);
}

static __exit void adc_rk3288_driver_exit(void)
{
	unregister_driver(&adc_rk3288);
}

driver_initcall(adc_rk3288_driver_init);
driver_exitcall(adc_rk3288_driver_exit);
