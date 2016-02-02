/*
 * virt-irq.c
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
#include <arm64.h>
#include <virt-irq.h>
#include <virt/reg-gic.h>

struct irqchip_data_t
{
	const char * name;
	int base;
	int nirq;
	physical_addr_t physdist;
	physical_addr_t physcpu;
};

struct irqchip_pdata_t
{
	const char * name;
	int base;
	int nirq;
	virtual_addr_t virtdist;
	virtual_addr_t virtcpu;
};

static struct irqchip_data_t datas[] = {
	{
		.name		= "PPI",
		.base		= 16,
		.nirq		= 16,
		.physdist	= VIRT_GIC_DIST_BASE,
		.physcpu	= VIRT_GIC_CPU_BASE,
	}, {
		.name		= "SPI",
		.base		= 32,
		.nirq		= 32,
		.physdist	= VIRT_GIC_DIST_BASE,
		.physcpu	= VIRT_GIC_CPU_BASE,
	}
};

static void irqchip_enable(struct irqchip_t * chip, int offset)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;
	int irq = chip->base + offset;
	write32(pdat->virtdist + DIST_ENABLE_SET + (irq / 32) * 4, 1 << (irq % 32));
}

static void irqchip_disable(struct irqchip_t * chip, int offset)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;
	int irq = chip->base + offset;
	write32(pdat->virtdist + DIST_ENABLE_CLEAR + (irq / 32) * 4, 1 << (irq % 32));
}

static void irqchip_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
}

static int irqchip_process(struct irqchip_t * chip)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;
	int irq = read32(pdat->virtcpu + CPU_INTACK) & 0x3ff;
	int offset = irq - chip->base;

	if((offset < 0) || (offset >= chip->nirq))
		return 0;

	(chip->handler[offset].func)(chip->handler[offset].data);
	write32(pdat->virtcpu + CPU_EOI, irq);
	return 1;
}

static void gic_dist_init(virtual_addr_t dist)
{
	u32_t gic_irqs;
	u32_t cpumask;
	int i;

	write32(dist + DIST_CTRL, 0x0);

	/*
	 * Find out how many interrupts are supported.
	 * The GIC only supports up to 1020 interrupt sources.
	 */
	gic_irqs = read32(dist + DIST_CTR) & 0x1f;
	gic_irqs = (gic_irqs + 1) * 32;
	if(gic_irqs > 1020)
		gic_irqs = 1020;

	/*
	 * Set all global interrupts to this CPU only.
	 */
	cpumask = 1 << arm64_smp_processor_id();
	cpumask |= cpumask << 8;
	cpumask |= cpumask << 16;
	for(i = 32; i < gic_irqs; i += 4)
		write32(dist + DIST_TARGET + i * 4 / 4, cpumask);

	/*
	 * Set all global interrupts to be level triggered, active low.
	 */
	for(i = 32; i < gic_irqs; i += 16)
		write32(dist + DIST_CONFIG + i * 4 / 16, 0);

	/*
	 * Set priority on all global interrupts.
	 */
	for(i = 32; i < gic_irqs; i += 4)
		write32(dist + DIST_PRI + i * 4 / 4, 0xa0a0a0a0);

	/*
	 * Disable all interrupts, leave the SGI and PPI alone
	 * as these enables are banked registers.
	 */
	for(i = 32; i < gic_irqs; i += 32)
		write32(dist + DIST_ENABLE_CLEAR + i * 4 / 32, 0xffffffff);

	write32(dist + DIST_CTRL, 0x1);
}

static void gic_cpu_init(virtual_addr_t dist, virtual_addr_t cpu)
{
	int i;

	/*
	 * Deal with the banked SGI and PPI interrupts - enable all
	 * SGI interrupts, ensure all PPI interrupts are disabled.
	 */
	write32(dist + DIST_ENABLE_CLEAR, 0xffff0000);
	write32(dist + DIST_ENABLE_SET, 0x0000ffff);

	/*
	 * Set priority on SGI and PPI interrupts
	 */
	for(i = 0; i < 32; i += 4)
		write32(dist + DIST_PRI + i * 4 / 4, 0xa0a0a0a0);

	write32(cpu + CPU_PRIMASK, 0xf0);
	write32(cpu + CPU_CTRL, 0x1);
}

static __init void virt_irqchip_init(void)
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
		pdat->virtdist = phys_to_virt(datas[i].physdist);
		pdat->virtcpu = phys_to_virt(datas[i].physcpu);

		chip->name = pdat->name;
		chip->base = pdat->base;
		chip->nirq = pdat->nirq;
		chip->handler = malloc(sizeof(struct irq_handler_t) * pdat->nirq);
		chip->enable = irqchip_enable;
		chip->disable = irqchip_disable;
		chip->settype = irqchip_settype;
		chip->process = irqchip_process;
		chip->priv = pdat;

		gic_dist_init(pdat->virtdist);
		gic_cpu_init(pdat->virtdist, pdat->virtcpu);
		register_irqchip(chip);
	}
	arm64_interrupt_enable();
}
core_initcall(virt_irqchip_init);
