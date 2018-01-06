/*
 * driver/irq-f1c100s.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

struct irq_f1c100s_pdata_t
{
	virtual_addr_t virt;
	int base;
	int nirq;
};

static void irq_f1c100s_enable(struct irqchip_t * chip, int offset)
{
	struct irq_f1c100s_pdata_t * pdat = (struct irq_f1c100s_pdata_t *)chip->priv;
	int irq = chip->base + offset;
	u32_t val;

	val = read32(pdat->virt + IRQ_ENABLE0 + (irq / 32) * 4);
	val |= 1 << (irq % 32);
	write32(pdat->virt + IRQ_ENABLE0 + (irq / 32) * 4, val);

	val = read32(pdat->virt + IRQ_MASK0 + (irq / 32) * 4);
	val &= ~(1 << (irq % 32));
	write32(pdat->virt + IRQ_MASK0 + (irq / 32) * 4, val);
}

static void irq_f1c100s_disable(struct irqchip_t * chip, int offset)
{
	struct irq_f1c100s_pdata_t * pdat = (struct irq_f1c100s_pdata_t *)chip->priv;
	int irq = chip->base + offset;
	u32_t val;

	val = read32(pdat->virt + IRQ_ENABLE0 + (irq / 32) * 4);
	val &= ~(1 << (irq % 32));
	write32(pdat->virt + IRQ_ENABLE0 + (irq / 32) * 4, val);

	val = read32(pdat->virt + IRQ_MASK0 + (irq / 32) * 4);
	val |= 1 << (irq % 32);
	write32(pdat->virt + IRQ_MASK0 + (irq / 32) * 4, val);
}

static void irq_f1c100s_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
}

static void irq_f1c100s_dispatch(struct irqchip_t * chip)
{
	struct irq_f1c100s_pdata_t * pdat = (struct irq_f1c100s_pdata_t *)chip->priv;
	int irq = read32(pdat->virt + IRQ_VECTOR) >> 2;
	int offset = irq - chip->base;

	if((offset >= 0) && (offset < chip->nirq))
	{
		(chip->handler[offset].func)(chip->handler[offset].data);
	}
}

static struct device_t * irq_f1c100s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct irq_f1c100s_pdata_t * pdat;
	struct irqchip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "interrupt-base", -1);
	int nirq = dt_read_int(n, "interrupt-count", -1);

	if((base < 0) || (nirq <= 0))
		return NULL;

	pdat = malloc(sizeof(struct irq_f1c100s_pdata_t));
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
	chip->enable = irq_f1c100s_enable;
	chip->disable = irq_f1c100s_disable;
	chip->settype = irq_f1c100s_settype;
	chip->dispatch = irq_f1c100s_dispatch;
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

static void irq_f1c100s_remove(struct device_t * dev)
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

static void irq_f1c100s_suspend(struct device_t * dev)
{
}

static void irq_f1c100s_resume(struct device_t * dev)
{
}

static struct driver_t irq_f1c100s = {
	.name		= "irq-f1c100s",
	.probe		= irq_f1c100s_probe,
	.remove		= irq_f1c100s_remove,
	.suspend	= irq_f1c100s_suspend,
	.resume		= irq_f1c100s_resume,
};

static __init void irq_f1c100s_driver_init(void)
{
	register_driver(&irq_f1c100s);
}

static __exit void irq_f1c100s_driver_exit(void)
{
	unregister_driver(&irq_f1c100s);
}

driver_initcall(irq_f1c100s_driver_init);
driver_exitcall(irq_f1c100s_driver_exit);
