/*
 * driver/adc-s5pv210.c
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
#include <clk/clk.h>
#include <adc/adc.h>

enum {
	TSADCCON	= 0x00,
	TSCON		= 0x04,
	TSDLY		= 0x08,
	TSDATX		= 0x0c,
	TSDATY		= 0x10,
	TSPENSTAT	= 0x14,
	CLRINTADC	= 0x18,
	ADCMUX		= 0x1c,
	CLRINTPEN	= 0x20,
};

struct adc_s5pv210_pdata_t
{
	virtual_addr_t virt;
	char * clk;
};

static u32_t adc_s5pv210_read(struct adc_t * adc, int channel)
{
	struct adc_s5pv210_pdata_t * pdat = (struct adc_s5pv210_pdata_t *)adc->priv;

	write32(pdat->virt + TSADCCON, read32(pdat->virt + TSADCCON) & ~((1 << 2) | (1 << 1)));
	write32(pdat->virt + ADCMUX, channel & 0xf);
	write32(pdat->virt + TSADCCON, read32(pdat->virt + TSADCCON) | (1 << 0));
	while(read32(pdat->virt + TSADCCON) & (1 << 0));
	while(!(read32(pdat->virt + TSADCCON) & (1 << 15)));
	return (read32(pdat->virt + TSDATX) & 0xfff);
}

static struct device_t * adc_s5pv210_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct adc_s5pv210_pdata_t * pdat;
	struct adc_t * adc;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct adc_s5pv210_pdata_t));
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
	adc->vreference = dt_read_int(n, "reference-voltage", 3300000);
	adc->resolution = 12;
	adc->nchannel = 2;
	adc->read = adc_s5pv210_read;
	adc->priv = pdat;

	clk_enable(pdat->clk);
	write32(pdat->virt + TSADCCON, (1 << 16) | (1 << 14) | (65 << 6));

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

static void adc_s5pv210_remove(struct device_t * dev)
{
	struct adc_t * adc = (struct adc_t *)dev->priv;
	struct adc_s5pv210_pdata_t * pdat = (struct adc_s5pv210_pdata_t *)adc->priv;

	if(adc && unregister_adc(adc))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(adc->name);
		free(adc->priv);
		free(adc);
	}
}

static void adc_s5pv210_suspend(struct device_t * dev)
{
}

static void adc_s5pv210_resume(struct device_t * dev)
{
}

static struct driver_t adc_s5pv210 = {
	.name		= "adc-s5pv210",
	.probe		= adc_s5pv210_probe,
	.remove		= adc_s5pv210_remove,
	.suspend	= adc_s5pv210_suspend,
	.resume		= adc_s5pv210_resume,
};

static __init void adc_s5pv210_driver_init(void)
{
	register_driver(&adc_s5pv210);
}

static __exit void adc_s5pv210_driver_exit(void)
{
	unregister_driver(&adc_s5pv210);
}

driver_initcall(adc_s5pv210_driver_init);
driver_exitcall(adc_s5pv210_driver_exit);
