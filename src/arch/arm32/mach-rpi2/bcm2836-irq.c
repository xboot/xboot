/*
 * bcm2836-irq.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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
#include <arm32.h>
#include <bcm2836-irq.h>
#include <bcm2836/reg-irq.h>

struct irqchip_data_t
{
	const char * name;
	int base;
	int nirq;
	physical_addr_t physpend;
	physical_addr_t physenable;
	physical_addr_t physdisable;
};

struct irqchip_pdata_t
{
	const char * name;
	int base;
	int nirq;
	virtual_addr_t virtpend;
	virtual_addr_t virtenable;
	virtual_addr_t virtdisable;
};

static struct irqchip_data_t datas[] = {
	{
		.name		= "VCIRQ",
		.base		= 0,
		.nirq		= 32,
		.physpend	= BCM2836_IRQ_BASE + IRQ_PEND1,
		.physenable	= BCM2836_IRQ_BASE + IRQ_ENABLE1,
		.physdisable= BCM2836_IRQ_BASE + IRQ_DISABLE1,
	}, {
		.name		= "GPUIRQ",
		.base		= 32,
		.nirq		= 32,
		.physpend	= BCM2836_IRQ_BASE + IRQ_PEND2,
		.physenable	= BCM2836_IRQ_BASE + IRQ_ENABLE2,
		.physdisable= BCM2836_IRQ_BASE + IRQ_DISABLE2,
	}, {
		.name		= "ARMIRQ",
		.base		= 64,
		.nirq		= 8,
		.physpend	= BCM2836_IRQ_BASE + IRQ_PEND_BASIC,
		.physenable	= BCM2836_IRQ_BASE + IRQ_ENABLE_BASIC,
		.physdisable= BCM2836_IRQ_BASE + IRQ_DISABLE_BASIC,
	}
};

static void irqchip_enable(struct irqchip_t * chip, int offset)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;
	int irq = chip->base + offset;
	write32(pdat->virtenable, 1 << (irq % 32));
}

static void irqchip_disable(struct irqchip_t * chip, int offset)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;
	int irq = chip->base + offset;
	write32(pdat->virtdisable, 1 << (irq % 32));
}

static void irqchip_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
}

static int irqchip_process(struct irqchip_t * chip)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;
	int offset = __ffs(read32(pdat->virtpend));
	if((offset < 0) || (offset >= chip->nirq))
		return 0;
	(chip->handler[offset].func)(chip->handler[offset].data);
	return 1;
}

static __init void bcm2836_irqchip_init(void)
{
	struct irqchip_pdata_t * pdat;
	struct irqchip_t * chip;
	int i;

	for(i = 0; i < ARRAY_SIZE(datas); i++)
	{
		pdat = malloc(sizeof(struct irqchip_pdata_t));
		if(!pdat)
			continue;

		chip = malloc(sizeof(struct irqchip_t));
		if(!chip)
		{
			free(pdat);
			continue;
		}

		pdat->name = datas[i].name;
		pdat->base = datas[i].base;
		pdat->nirq = datas[i].nirq;
		pdat->virtpend = phys_to_virt(datas[i].physpend);
		pdat->virtenable = phys_to_virt(datas[i].physenable);
		pdat->virtdisable = phys_to_virt(datas[i].physdisable);

		chip->name = pdat->name;
		chip->base = pdat->base;
		chip->nirq = pdat->nirq;
		chip->handler = malloc(sizeof(struct irq_handler_t) * pdat->nirq);
		chip->enable = irqchip_enable;
		chip->disable = irqchip_disable;
		chip->settype = irqchip_settype;
		chip->process = irqchip_process;
		chip->priv = pdat;

		register_irqchip(chip);
	}
	arm32_interrupt_enable();
}
//core_initcall(bcm2836_irqchip_init);
