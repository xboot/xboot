/*
 * driver/irq-s5p4418-gpioalv.c
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
#include <interrupt/interrupt.h>

enum {
	GPIOALV_PWRGATEREG					= 0x800,
	GPIOALV_ASYNCDETECTMODERSTREG0		= 0x804,
	GPIOALV_ASYNCDETECTMODESETREG0		= 0x808,
	GPIOALV_LOWASYNCDETECTMODEREADREG	= 0x80C,
	GPIOALV_ASYNCDETECTMODERSTREG1		= 0x810,
	GPIOALV_ASYNCDETECTMODESETREG1		= 0x814,
	GPIOALV_HIGHASYNCDETECTMODEREADREG	= 0x818,
	GPIOALV_DETECTMODERSTREG0			= 0x81C,
	GPIOALV_DETECTMODESETREG0			= 0x820,
	GPIOALV_FALLDETECTMODEREADREG		= 0x824,
	GPIOALV_DETECTMODERSTREG1			= 0x828,
	GPIOALV_DETECTMODESETREG1			= 0x82C,
	GPIOALV_RISEDETECTMODEREADREG		= 0x830,
	GPIOALV_DETECTMODERSTREG2			= 0x834,
	GPIOALV_DETECTMODESETREG2			= 0x838,
	GPIOALV_LOWDETECTMODEREADREG		= 0x83C,
	GPIOALV_DETECTMODERSTREG3			= 0x840,
	GPIOALV_DETECTMODESETREG3			= 0x844,
	GPIOALV_HIGHDETECTMODEREADREG		= 0x848,
	GPIOALV_DETECTENBRSTREG				= 0x84C,
	GPIOALV_DETECTENBSETREG				= 0x850,
	GPIOALV_DETECTENBREADREG			= 0x854,
	GPIOALV_INTENBRSTREG				= 0x858,
	GPIOALV_INTENBSETREG				= 0x85C,
	GPIOALV_INTENBREADREG				= 0x860,
	GPIOALV_DETECTPENDREG				= 0x864,
	GPIOALV_SCRATCHRSTREG				= 0x868,
	GPIOALV_SCRATCHSETREG				= 0x86C,
	GPIOALV_SCRATCHREADREG				= 0x870,
	GPIOALV_PADOUTENBRSTREG				= 0x874,
	GPIOALV_PADOUTENBSETREG				= 0x878,
	GPIOALV_PADOUTENBREADREG			= 0x87C,
	GPIOALV_PADPULLUPRSTREG				= 0x880,
	GPIOALV_PADPULLUPSETREG				= 0x884,
	GPIOALV_PADPULLUPREADREG			= 0x888,
	GPIOALV_PADOUTRSTREG				= 0x88C,
	GPIOALV_PADOUTSETREG				= 0x890,
	GPIOALV_PADOUTREADREG				= 0x894,
	GPIOALV_VDDCTRLRSTREG				= 0x898,
	GPIOALV_VDDCTRLSETREG				= 0x89C,
	GPIOALV_VDDCTRLREADREG				= 0x8A0,
	GPIOALV_CLEARWAKEUPSTATUS			= 0x8A4,
	GPIOALV_SLEEPWAKEUPSTATUS			= 0x8A8,
	GPIOALV_SCRATCHRST1					= 0x8AC,
	GPIOALV_SCRATCHRST2					= 0x8B8,
	GPIOALV_SCRATCHRST3					= 0x8C4,
	GPIOALV_SCRATCHRST4					= 0x8D0,
	GPIOALV_SCRATCHRST5					= 0x8DC,
	GPIOALV_SCRATCHRST6					= 0x8E8,
	GPIOALV_SCRATCHRST7					= 0x8F4,
	GPIOALV_SCRATCHRST8					= 0x900,
	GPIOALV_SCRATCHSET1					= 0x8B0,
	GPIOALV_SCRATCHSET2					= 0x8BC,
	GPIOALV_SCRATCHSET3					= 0x8C8,
	GPIOALV_SCRATCHSET4					= 0x8D4,
	GPIOALV_SCRATCHSET5					= 0x8E0,
	GPIOALV_SCRATCHSET6					= 0x8EC,
	GPIOALV_SCRATCHSET7					= 0x8F8,
	GPIOALV_SCRATCHSET8					= 0x904,
	GPIOALV_SCRATCHREAD1				= 0x8B4,
	GPIOALV_SCRATCHREAD2				= 0x8C0,
	GPIOALV_SCRATCHREAD3				= 0x8CC,
	GPIOALV_SCRATCHREAD4				= 0x8D8,
	GPIOALV_SCRATCHREAD5				= 0x8E4,
	GPIOALV_SCRATCHREAD6				= 0x8F0,
	GPIOALV_SCRATCHREAD7				= 0x8FC,
	GPIOALV_SCRATCHREAD8				= 0x908,
	GPIOALV_VDDOFFDELAYRST				= 0x90C,
	GPIOALV_VDDOFFDELAYSET				= 0x910,
	GPIOALV_VDDOFFDELAYVAL				= 0x914,
	GPIOALV_VDDOFFDELAYIME				= 0x918,
	GPIOALV_GPIOINPUTVALUE				= 0x91C,
	GPIOALV_PMUNISOLATE					= 0xD00,
	GPIOALV_PMUNPWRUPPRE				= 0xD04,
	GPIOALV_PMUNPWRUP					= 0xD08,
	GPIOALV_PMUNPWRUPACK				= 0xD0C,
};

struct irq_s5p4418_gpioalv_pdata_t
{
	virtual_addr_t virt;
	int base;
	int nirq;
	int parent;
};

static inline void gpioalv_write_enable(virtual_addr_t virt)
{
	write32(virt + GPIOALV_PWRGATEREG, 0x1);
}

static inline void gpioalv_write_disable(virtual_addr_t virt)
{
	write32(virt + GPIOALV_PWRGATEREG, 0x0);
}

static void irq_s5p4418_gpioalv_enable(struct irqchip_t * chip, int offset)
{
	struct irq_s5p4418_gpioalv_pdata_t * pdat = (struct irq_s5p4418_gpioalv_pdata_t *)chip->priv;
	gpioalv_write_enable(pdat->virt);
	write32(pdat->virt + GPIOALV_INTENBSETREG, 0x1 << offset);
	gpioalv_write_disable(pdat->virt);
}

static void irq_s5p4418_gpioalv_disable(struct irqchip_t * chip, int offset)
{
	struct irq_s5p4418_gpioalv_pdata_t * pdat = (struct irq_s5p4418_gpioalv_pdata_t *)chip->priv;
	gpioalv_write_enable(pdat->virt);
	write32(pdat->virt + GPIOALV_INTENBRSTREG, 0x1 << offset);
	gpioalv_write_disable(pdat->virt);
}

static void irq_s5p4418_gpioalv_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
	struct irq_s5p4418_gpioalv_pdata_t * pdat = (struct irq_s5p4418_gpioalv_pdata_t *)chip->priv;

	if(type != IRQ_TYPE_NONE)
	{
		gpioalv_write_enable(pdat->virt);
		switch(type)
		{
		case IRQ_TYPE_LEVEL_LOW:
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG0, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG1, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODESETREG2, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG3, 0x1 << offset);
			break;

		case IRQ_TYPE_LEVEL_HIGH:
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG0, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG1, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG2, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODESETREG3, 0x1 << offset);
			break;

		case IRQ_TYPE_EDGE_FALLING:
			write32(pdat->virt + GPIOALV_DETECTMODESETREG0, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG1, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG2, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG3, 0x1 << offset);
			break;

		case IRQ_TYPE_EDGE_RISING:
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG0, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODESETREG1, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG2, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG3, 0x1 << offset);
			break;

		default:
			break;
		}
		write32(pdat->virt + GPIOALV_DETECTENBSETREG, 0x1 << offset);
		gpioalv_write_disable(pdat->virt);
	}
	else
	{
		gpioalv_write_enable(pdat->virt);
		write32(pdat->virt + GPIOALV_DETECTENBRSTREG, 0x1 << offset);
		gpioalv_write_disable(pdat->virt);
	}
}

static void irq_s5p4418_gpioalv_dispatch(struct irqchip_t * chip)
{
	struct irq_s5p4418_gpioalv_pdata_t * pdat = (struct irq_s5p4418_gpioalv_pdata_t *)chip->priv;
	u32_t det = read32(pdat->virt + GPIOALV_DETECTPENDREG);

	if(det != 0)
	{
		u32_t offset = __ffs(det);
		if((offset >= 0) && (offset < chip->nirq))
		{
			(chip->handler[offset].func)(chip->handler[offset].data);
			gpioalv_write_enable(pdat->virt);
			write32(pdat->virt + GPIOALV_DETECTPENDREG, (0x1 << offset));
			gpioalv_write_disable(pdat->virt);
		}
	}
}

static struct device_t * irq_s5p4418_gpioalv_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct irq_s5p4418_gpioalv_pdata_t * pdat;
	struct irqchip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "interrupt-base", -1);
	int nirq = dt_read_int(n, "interrupt-count", -1);
	int parent = dt_read_int(n, "interrupt-parent", -1);

	if((base < 0) || (nirq <= 0) || !irq_is_valid(parent))
		return NULL;

	pdat = malloc(sizeof(struct irq_s5p4418_gpioalv_pdata_t));
	if(!pdat)
		return NULL;

	chip = malloc(sizeof(struct irqchip_t));
	if(!chip)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->base = base;
	pdat->nirq = nirq;
	pdat->parent = parent;

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->nirq = pdat->nirq;
	chip->handler = malloc(sizeof(struct irq_handler_t) * pdat->nirq);
	chip->enable = irq_s5p4418_gpioalv_enable;
	chip->disable = irq_s5p4418_gpioalv_disable;
	chip->settype = irq_s5p4418_gpioalv_settype;
	chip->dispatch = irq_s5p4418_gpioalv_dispatch;
	chip->priv = pdat;

	if(!(dev = register_sub_irqchip(pdat->parent, chip, drv)))
	{
		free_device_name(chip->name);
		free(chip->handler);
		free(chip->priv);
		free(chip);
		return NULL;
	}
	return dev;
}

static void irq_s5p4418_gpioalv_remove(struct device_t * dev)
{
	struct irqchip_t * chip = (struct irqchip_t *)dev->priv;
	struct irq_s5p4418_gpioalv_pdata_t * pdat = (struct irq_s5p4418_gpioalv_pdata_t *)chip->priv;

	if(chip)
	{
		unregister_sub_irqchip(pdat->parent, chip);
		free_device_name(chip->name);
		free(chip->handler);
		free(chip->priv);
		free(chip);
	}
}

static void irq_s5p4418_gpioalv_suspend(struct device_t * dev)
{
}

static void irq_s5p4418_gpioalv_resume(struct device_t * dev)
{
}

static struct driver_t irq_s5p4418_gpioalv = {
	.name		= "irq-s5p4418-gpioalv",
	.probe		= irq_s5p4418_gpioalv_probe,
	.remove		= irq_s5p4418_gpioalv_remove,
	.suspend	= irq_s5p4418_gpioalv_suspend,
	.resume		= irq_s5p4418_gpioalv_resume,
};

static __init void irq_s5p4418_gpioalv_driver_init(void)
{
	register_driver(&irq_s5p4418_gpioalv);
}

static __exit void irq_s5p4418_gpioalv_driver_exit(void)
{
	unregister_driver(&irq_s5p4418_gpioalv);
}

driver_initcall(irq_s5p4418_gpioalv_driver_init);
driver_exitcall(irq_s5p4418_gpioalv_driver_exit);
