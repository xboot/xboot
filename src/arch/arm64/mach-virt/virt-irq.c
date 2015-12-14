/*
 * virt-irq.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
#include <virt/reg-gic.h>

static struct irq_handler_t virt_sgi_ppi_handler[32];
static struct irq_handler_t virt_spi_handler[128];

void do_irq(void * regs)
{
	int irq;

	irq = read32(phys_to_virt(VIRT_GIC_CPU_BASE + CPU_INTACK)) & 0x3ff;
	if(irq < 32)
		(virt_sgi_ppi_handler[irq].func)(virt_sgi_ppi_handler[irq].data);
	else if(irq < 64)
		(virt_spi_handler[irq].func)(virt_spi_handler[irq].data);
	write32(phys_to_virt(VIRT_GIC_CPU_BASE + CPU_EOI), irq);
}

static void virt_irq_enable(struct irq_t * irq)
{
	write32(phys_to_virt(VIRT_GIC_DIST_BASE + DIST_ENABLE_SET + (irq->no / 32) * 4), 1 << (irq->no % 32));
}

static void virt_irq_disable(struct irq_t * irq)
{
	write32(phys_to_virt(VIRT_GIC_DIST_BASE + DIST_ENABLE_CLEAR + (irq->no / 32) * 4), 1 << (irq->no % 32));
}

static void virt_irq_set_type(struct irq_t * irq, enum irq_type_t type)
{
}

static struct irq_t virt_irqs[] = {
	{
		.name		= "VMI",
		.no			= 25,
		.handler	= &virt_sgi_ppi_handler[25],
		.enable		= virt_irq_enable,
		.disable	= virt_irq_disable,
		.set_type	= virt_irq_set_type,
	}, {
		.name		= "HYPTIMER",
		.no			= 26,
		.handler	= &virt_sgi_ppi_handler[26],
		.enable		= virt_irq_enable,
		.disable	= virt_irq_disable,
		.set_type	= virt_irq_set_type,
	}, {
		.name		= "VIRTIMER",
		.no			= 27,
		.handler	= &virt_sgi_ppi_handler[27],
		.enable		= virt_irq_enable,
		.disable	= virt_irq_disable,
		.set_type	= virt_irq_set_type,
	}, {
		.name		= "LEGACYFIQ",
		.no			= 28,
		.handler	= &virt_sgi_ppi_handler[28],
		.enable		= virt_irq_enable,
		.disable	= virt_irq_disable,
		.set_type	= virt_irq_set_type,
	}, {
		.name		= "STIMER",
		.no			= 29,
		.handler	= &virt_sgi_ppi_handler[29],
		.enable		= virt_irq_enable,
		.disable	= virt_irq_disable,
		.set_type	= virt_irq_set_type,
	}, {
		.name		= "NSTIMER",
		.no			= 30,
		.handler	= &virt_sgi_ppi_handler[30],
		.enable		= virt_irq_enable,
		.disable	= virt_irq_disable,
		.set_type	= virt_irq_set_type,
	}, {
		.name		= "LEGACYIRQ",
		.no			= 31,
		.handler	= &virt_sgi_ppi_handler[31],
		.enable		= virt_irq_enable,
		.disable	= virt_irq_disable,
		.set_type	= virt_irq_set_type,
	}, {
		.name		= "UART",
		.no			= 32 + 0,
		.handler	= &virt_spi_handler[32 + 0],
		.enable		= virt_irq_enable,
		.disable	= virt_irq_disable,
		.set_type	= virt_irq_set_type,
	}, {
		.name		= "RTC",
		.no			= 32 + 1,
		.handler	= &virt_spi_handler[32 + 1],
		.enable		= virt_irq_enable,
		.disable	= virt_irq_disable,
		.set_type	= virt_irq_set_type,
	},
};

static void gic_dist_init(physical_addr_t dist)
{
	u32_t gic_irqs;
	u32_t cpumask;
	int i;

	write32(phys_to_virt(dist + DIST_CTRL), 0x0);

	/*
	 * Find out how many interrupts are supported.
	 * The GIC only supports up to 1020 interrupt sources.
	 */
	gic_irqs = read32(phys_to_virt(dist + DIST_CTR)) & 0x1f;
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
		write32(phys_to_virt(dist + DIST_TARGET + i * 4 / 4), cpumask);

	/*
	 * Set all global interrupts to be level triggered, active low.
	 */
	for(i = 32; i < gic_irqs; i += 16)
		write32(phys_to_virt(dist + DIST_CONFIG + i * 4 / 16), 0);

	/*
	 * Set priority on all global interrupts.
	 */
	for(i = 32; i < gic_irqs; i += 4)
		write32(phys_to_virt(dist + DIST_PRI + i * 4 / 4), 0xa0a0a0a0);

	/*
	 * Disable all interrupts.  Leave the PPI and SGIs alone
	 * as these enables are banked registers.
	 */
	for(i = 32; i < gic_irqs; i += 32)
		write32(phys_to_virt(dist + DIST_ENABLE_CLEAR + i * 4 / 32), 0xffffffff);

	write32(phys_to_virt(dist + DIST_CTRL), 0x1);
}

static void gic_cpu_init(physical_addr_t dist, physical_addr_t cpu)
{
	int i;

	/*
	 * Deal with the banked PPI and SGI interrupts - disable all
	 * PPI interrupts, ensure all SGI interrupts are enabled.
	 */
	write32(phys_to_virt(dist + DIST_ENABLE_CLEAR), 0xffff0000);
	write32(phys_to_virt(dist + DIST_ENABLE_SET), 0x0000ffff);

	/*
	 * Set priority on PPI and SGI interrupts
	 */
	for(i = 0; i < 32; i += 4)
		write32(phys_to_virt(dist + DIST_PRI + i * 4 / 4), 0xa0a0a0a0);

	write32(phys_to_virt(cpu + CPU_PRIMASK), 0xf0);
	write32(phys_to_virt(cpu + CPU_CTRL), 0x1);
}

static __init void virt_irq_init(void)
{
	int i;

	gic_dist_init(VIRT_GIC_DIST_BASE);
	gic_cpu_init(VIRT_GIC_DIST_BASE, VIRT_GIC_CPU_BASE);

	for(i = 0; i < ARRAY_SIZE(virt_irqs); i++)
	{
		if(irq_register(&virt_irqs[i]))
			LOG("Register irq '%s'", virt_irqs[i].name);
		else
			LOG("Failed to register irq '%s'", virt_irqs[i].name);
	}

	arm64_interrupt_enable();
}

static __exit void virt_irq_exit(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(virt_irqs); i++)
	{
		if(irq_unregister(&virt_irqs[i]))
			LOG("Unregister irq '%s'", virt_irqs[i].name);
		else
			LOG("Failed to unregister irq '%s'", virt_irqs[i].name);
	}

	arm64_interrupt_disable();
}

core_initcall(virt_irq_init);
core_exitcall(virt_irq_exit);
