/*
 * driver/adc-f1c500s.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
	LRADC_CTRL	= 0x00,
	LRADC_INTC	= 0x04,
	LRADC_INTS	= 0x08,
	LRADC_DATA0	= 0x0c,
};

/* LRADC_CTRL bits */
#define FIRST_CONVERT_DLY(x)	((x) << 24) /* 8 bits */
#define CHAN_SELECT(x)			((x) << 22) /* 2 bits */
#define CONTINUE_TIME_SEL(x)	((x) << 16) /* 4 bits */
#define KEY_MODE_SEL(x)			((x) << 12) /* 2 bits */
#define LEVELA_B_CNT(x)			((x) << 8)  /* 4 bits */
#define HOLD_EN(x)				((x) << 6)
#define LEVELB_VOL(x)			((x) << 4)  /* 2 bits */
#define SAMPLE_RATE(x)			((x) << 2)  /* 2 bits */
#define ENABLE(x)				((x) << 0)

/* LRADC_INTC and LRADC_INTS bits */
#define CHAN1_KEYUP_IRQ			(1 << 12)
#define CHAN1_ALRDY_HOLD_IRQ	(1 << 11)
#define CHAN1_HOLD_IRQ			(1 << 10)
#define	CHAN1_KEYDOWN_IRQ		(1 << 9)
#define CHAN1_DATA_IRQ			(1 << 8)
#define CHAN0_KEYUP_IRQ			(1 << 4)
#define CHAN0_ALRDY_HOLD_IRQ	(1 << 3)
#define CHAN0_HOLD_IRQ			(1 << 2)
#define	CHAN0_KEYDOWN_IRQ		(1 << 1)
#define CHAN0_DATA_IRQ			(1 << 0)

struct adc_f1c500s_pdata_t
{
	virtual_addr_t virt;
};

static u32_t adc_f1c500s_read(struct adc_t * adc, int channel)
{
	struct adc_f1c500s_pdata_t * pdat = (struct adc_f1c500s_pdata_t *)adc->priv;
	u32_t ints, val;

	write32(pdat->virt + LRADC_CTRL, FIRST_CONVERT_DLY(2) | LEVELA_B_CNT(1) | SAMPLE_RATE(0) | ENABLE(1));
	write32(pdat->virt + LRADC_INTS, 0);
	do {
		ints = read32(pdat->virt + LRADC_INTS);
	} while(!(ints & CHAN0_DATA_IRQ));
	val = (read32(pdat->virt + LRADC_DATA0) & 0x3f);
	write32(pdat->virt + LRADC_INTS, ints);
	write32(pdat->virt + LRADC_CTRL, 0);

	return val;
}

static struct device_t * adc_f1c500s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct adc_f1c500s_pdata_t * pdat;
	struct adc_t * adc;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));

	pdat = malloc(sizeof(struct adc_f1c500s_pdata_t));
	if(!pdat)
		return NULL;

	adc = malloc(sizeof(struct adc_t));
	if(!adc)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;

	adc->name = alloc_device_name(dt_read_name(n), -1);
	adc->vreference = dt_read_int(n, "reference-voltage", 3000000) * 2 / 3;
	adc->resolution = 6;
	adc->nchannel = 1;
	adc->read = adc_f1c500s_read;
	adc->priv = pdat;

	write32(pdat->virt + LRADC_CTRL, 0);
	write32(pdat->virt + LRADC_INTC, 0);

	if(!(dev = register_adc(adc, drv)))
	{
		free_device_name(adc->name);
		free(adc->priv);
		free(adc);
		return NULL;
	}
	return dev;
}

static void adc_f1c500s_remove(struct device_t * dev)
{
	struct adc_t * adc = (struct adc_t *)dev->priv;

	if(adc)
	{
		unregister_adc(adc);
		free_device_name(adc->name);
		free(adc->priv);
		free(adc);
	}
}

static void adc_f1c500s_suspend(struct device_t * dev)
{
}

static void adc_f1c500s_resume(struct device_t * dev)
{
}

static struct driver_t adc_f1c500s = {
	.name		= "adc-f1c500s",
	.probe		= adc_f1c500s_probe,
	.remove		= adc_f1c500s_remove,
	.suspend	= adc_f1c500s_suspend,
	.resume		= adc_f1c500s_resume,
};

static __init void adc_f1c500s_driver_init(void)
{
	register_driver(&adc_f1c500s);
}

static __exit void adc_f1c500s_driver_exit(void)
{
	unregister_driver(&adc_f1c500s);
}

driver_initcall(adc_f1c500s_driver_init);
driver_exitcall(adc_f1c500s_driver_exit);
