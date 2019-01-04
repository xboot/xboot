/*
 * driver/irq-f1c500s.c
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
#include <interrupt/interrupt.h>
#include <arm32.h>

enum
{
	IRQ_VECTOR			= 0x00,
	IRQ_BASE_ADDR		= 0x04,
	IRQ_NMI_CTRL		= 0x0c,
	IRQ_PEND0			= 0x10,
	IRQ_PEND1			= 0x14,
	IRQ_ENABLE0			= 0x20,
	IRQ_ENABLE1			= 0x24,
	IRQ_MASK0			= 0x30,
	IRQ_MASK1			= 0x34,
	IRQ_RESP0			= 0x40,
	IRQ_RESP1			= 0x44,
	IRQ_FORCE0			= 0x50,
	IRQ_FORCE1			= 0x54,
	IRQ_PRIORITY0		= 0x60,
	IRQ_PRIORITY1		= 0x64,
	IRQ_PRIORITY2		= 0x68,
	IRQ_PRIORITY3		= 0x6c,
};

struct irq_f1c500s_pdata_t
{
	virtual_addr_t virt;
	int base;
	int nirq;
};

static void irq_f1c500s_enable(struct irqchip_t * chip, int offset)
{
	struct irq_f1c500s_pdata_t * pdat = (struct irq_f1c500s_pdata_t *)chip->priv;
	int irq = chip->base + offset;
	u32_t val;

	val = read32(pdat->virt + IRQ_ENABLE0 + (irq / 32) * 4);
	val |= 1 << (irq % 32);
	write32(pdat->virt + IRQ_ENABLE0 + (irq / 32) * 4, val);

	val = read32(pdat->virt + IRQ_MASK0 + (irq / 32) * 4);
	val &= ~(1 << (irq % 32));
	write32(pdat->virt + IRQ_MASK0 + (irq / 32) * 4, val);
}

static void irq_f1c500s_disable(struct irqchip_t * chip, int offset)
{
	struct irq_f1c500s_pdata_t * pdat = (struct irq_f1c500s_pdata_t *)chip->priv;
	int irq = chip->base + offset;
	u32_t val;

	val = read32(pdat->virt + IRQ_ENABLE0 + (irq / 32) * 4);
	val &= ~(1 << (irq % 32));
	write32(pdat->virt + IRQ_ENABLE0 + (irq / 32) * 4, val);

	val = read32(pdat->virt + IRQ_MASK0 + (irq / 32) * 4);
	val |= 1 << (irq % 32);
	write32(pdat->virt + IRQ_MASK0 + (irq / 32) * 4, val);
}

static void irq_f1c500s_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
}

static void irq_f1c500s_dispatch(struct irqchip_t * chip)
{
	struct irq_f1c500s_pdata_t * pdat = (struct irq_f1c500s_pdata_t *)chip->priv;
	int irq = read32(pdat->virt + IRQ_VECTOR) >> 2;
	int offset = irq - chip->base;

	if((offset >= 0) && (offset < chip->nirq))
	{
		(chip->handler[offset].func)(chip->handler[offset].data);
	}
}

static struct device_t * irq_f1c500s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct irq_f1c500s_pdata_t * pdat;
	struct irqchip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "interrupt-base", -1);
	int nirq = dt_read_int(n, "interrupt-count", -1);

	if((base < 0) || (nirq <= 0))
		return NULL;

	pdat = malloc(sizeof(struct irq_f1c500s_pdata_t));
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
	chip->enable = irq_f1c500s_enable;
	chip->disable = irq_f1c500s_disable;
	chip->settype = irq_f1c500s_settype;
	chip->dispatch = irq_f1c500s_dispatch;
	chip->priv = pdat;

	arm32_interrupt_enable();

	if(!register_irqchip(&dev, chip))
	{
		free_device_name(chip->name);
		free(chip->handler);
		free(chip->priv);
		free(chip);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void irq_f1c500s_remove(struct device_t * dev)
{
	struct irqchip_t * chip = (struct irqchip_t *)dev->priv;

	if(chip && unregister_irqchip(chip))
	{
		free_device_name(chip->name);
		free(chip->handler);
		free(chip->priv);
		free(chip);
	}
}

static void irq_f1c500s_suspend(struct device_t * dev)
{
}

static void irq_f1c500s_resume(struct device_t * dev)
{
}

static struct driver_t irq_f1c500s = {
	.name		= "irq-f1c500s",
	.probe		= irq_f1c500s_probe,
	.remove		= irq_f1c500s_remove,
	.suspend	= irq_f1c500s_suspend,
	.resume		= irq_f1c500s_resume,
};

static __init void irq_f1c500s_driver_init(void)
{
	register_driver(&irq_f1c500s);
}

static __exit void irq_f1c500s_driver_exit(void)
{
	unregister_driver(&irq_f1c500s);
}

driver_initcall(irq_f1c500s_driver_init);
driver_exitcall(irq_f1c500s_driver_exit);
