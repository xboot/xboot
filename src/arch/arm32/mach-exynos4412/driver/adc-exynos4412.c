/*
 * driver/adc-exynos4412.c
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
#include <clk/clk.h>
#include <adc/adc.h>

enum {
	ADCCON		= 0x00,
	ADCDLY		= 0x08,
	ADCDAT		= 0x0c,
	CLRINTADC	= 0x18,
	ADCMUX		= 0x1c,
};

struct adc_exynos4412_pdata_t
{
	virtual_addr_t virt;
	char * clk;
};

static u32_t adc_exynos4412_read(struct adc_t * adc, int channel)
{
	struct adc_exynos4412_pdata_t * pdat = (struct adc_exynos4412_pdata_t *)adc->priv;

	write32(pdat->virt + ADCCON, read32(pdat->virt + ADCCON) & ~((1 << 2) | (1 << 1)));
	write32(pdat->virt + ADCMUX, channel & 0xf);
	write32(pdat->virt + ADCCON, read32(pdat->virt + ADCCON) | (1 << 0));
	while(read32(pdat->virt + ADCCON) & (1 << 0));
	while(!(read32(pdat->virt + ADCCON) & (1 << 15)));
	return (read32(pdat->virt + ADCDAT) & 0xfff);
}

static struct device_t * adc_exynos4412_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct adc_exynos4412_pdata_t * pdat;
	struct adc_t * adc;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct adc_exynos4412_pdata_t));
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
	adc->resolution = 12;
	adc->nchannel = 4;
	adc->read = adc_exynos4412_read;
	adc->priv = pdat;

	clk_enable(pdat->clk);
	write32(pdat->virt + ADCCON, (1 << 16) | (1 << 14) | (100 << 6));

	if(!register_adc(&dev, adc))
	{
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

static void adc_exynos4412_remove(struct device_t * dev)
{
	struct adc_t * adc = (struct adc_t *)dev->priv;
	struct adc_exynos4412_pdata_t * pdat = (struct adc_exynos4412_pdata_t *)adc->priv;

	if(adc && unregister_adc(adc))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(adc->name);
		free(adc->priv);
		free(adc);
	}
}

static void adc_exynos4412_suspend(struct device_t * dev)
{
}

static void adc_exynos4412_resume(struct device_t * dev)
{
}

static struct driver_t adc_exynos4412 = {
	.name		= "adc-exynos4412",
	.probe		= adc_exynos4412_probe,
	.remove		= adc_exynos4412_remove,
	.suspend	= adc_exynos4412_suspend,
	.resume		= adc_exynos4412_resume,
};

static __init void adc_exynos4412_driver_init(void)
{
	register_driver(&adc_exynos4412);
}

static __exit void adc_exynos4412_driver_exit(void)
{
	unregister_driver(&adc_exynos4412);
}

driver_initcall(adc_exynos4412_driver_init);
driver_exitcall(adc_exynos4412_driver_exit);
