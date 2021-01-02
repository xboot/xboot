/*
 * driver/irq-bcm2837.c
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
#include <arm64.h>

enum
{
	IRQ_FIQCTL			= 0x0c,
	IRQ_PEND1			= 0x04,
	IRQ_PEND2			= 0x08,
	IRQ_PEND_BASIC		= 0x00,
	IRQ_ENABLE1			= 0x10,
	IRQ_ENABLE2			= 0x14,
	IRQ_ENABLE_BASIC	= 0x18,
	IRQ_DISABLE1		= 0x1c,
	IRQ_DISABLE2		= 0x20,
	IRQ_DISABLE_BASIC	= 0x24,
};

struct irq_bcm2837_pdata_t
{
	virtual_addr_t virt;
	int base;
	int nirq;
};

static void irq_bcm2837_enable(struct irqchip_t * chip, int offset)
{
	struct irq_bcm2837_pdata_t * pdat = (struct irq_bcm2837_pdata_t *)chip->priv;
	const int enable[3] = {IRQ_ENABLE1, IRQ_ENABLE2, IRQ_ENABLE_BASIC};
	int irq = chip->base + offset;
	write32(pdat->virt + enable[irq >> 5], 1 << (irq & 0x1f));
}

static void irq_bcm2837_disable(struct irqchip_t * chip, int offset)
{
	struct irq_bcm2837_pdata_t * pdat = (struct irq_bcm2837_pdata_t *)chip->priv;
	const int disable[3] = {IRQ_DISABLE1, IRQ_DISABLE2, IRQ_DISABLE_BASIC};
	int irq = chip->base + offset;
	write32(pdat->virt + disable[irq >> 5], 1 << (irq & 0x1f));
}

static void irq_bcm2837_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
}

static void irq_bcm2837_dispatch(struct irqchip_t * chip)
{
	struct irq_bcm2837_pdata_t * pdat = (struct irq_bcm2837_pdata_t *)chip->priv;
	u32_t basic, pending;
	int offset = -1;

	if((basic = read32(pdat->virt + IRQ_PEND_BASIC) & 0x3ff) != 0)
	{
		if(basic & (1 << 8))
		{
			if((pending = read32(pdat->virt + IRQ_PEND1)) != 0)
				offset = __ffs(pending) + 0;
		}
		else if(basic & (1 << 9))
		{
			if((pending = read32(pdat->virt + IRQ_PEND2)) != 0)
				offset = __ffs(pending) + 32;
		}
		else if(basic & 0xff)
		{
			offset = __ffs(basic & 0xff) + 64;
		}
	}

	if((offset >= 0) && (offset < chip->nirq))
		(chip->handler[offset].func)(chip->handler[offset].data);
}

static struct device_t * irq_bcm2837_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct irq_bcm2837_pdata_t * pdat;
	struct irqchip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "interrupt-base", -1);
	int nirq = dt_read_int(n, "interrupt-count", -1);

	if((base < 0) || (nirq <= 0))
		return NULL;

	pdat = malloc(sizeof(struct irq_bcm2837_pdata_t));
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

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->nirq = pdat->nirq;
	chip->handler = malloc(sizeof(struct irq_handler_t) * pdat->nirq);
	chip->enable = irq_bcm2837_enable;
	chip->disable = irq_bcm2837_disable;
	chip->settype = irq_bcm2837_settype;
	chip->dispatch = irq_bcm2837_dispatch;
	chip->priv = pdat;

	arm64_interrupt_enable();

	if(!(dev = register_irqchip(chip, drv)))
	{
		free_device_name(chip->name);
		free(chip->handler);
		free(chip->priv);
		free(chip);
		return NULL;
	}
	return dev;
}

static void irq_bcm2837_remove(struct device_t * dev)
{
	struct irqchip_t * chip = (struct irqchip_t *)dev->priv;

	if(chip)
	{
		unregister_irqchip(chip);
		free_device_name(chip->name);
		free(chip->handler);
		free(chip->priv);
		free(chip);
	}
}

static void irq_bcm2837_suspend(struct device_t * dev)
{
}

static void irq_bcm2837_resume(struct device_t * dev)
{
}

static struct driver_t irq_bcm2837 = {
	.name		= "irq-bcm2837",
	.probe		= irq_bcm2837_probe,
	.remove		= irq_bcm2837_remove,
	.suspend	= irq_bcm2837_suspend,
	.resume		= irq_bcm2837_resume,
};

static __init void irq_bcm2837_driver_init(void)
{
	register_driver(&irq_bcm2837);
}

static __exit void irq_bcm2837_driver_exit(void)
{
	unregister_driver(&irq_bcm2837);
}

driver_initcall(irq_bcm2837_driver_init);
driver_exitcall(irq_bcm2837_driver_exit);
