/*
 * realview-irq.c
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
#include <cp15.h>
#include <realview/reg-gic.h>

static struct irq_handler_t realview_irq_handler[32];

void do_irq(void * regs)
{
	u32_t irq;

	/* Get irq's offset */
	irq = read32(phys_to_virt(REALVIEW_GIC_CPU_BASE + CPU_INTACK)) & 0x3ff;

	/* Handle interrupt server function */
	(realview_irq_handler[irq - 32].func)(realview_irq_handler[irq - 32].data);

	/* Exit interrupt */
	write32(phys_to_virt(REALVIEW_GIC_CPU_BASE + CPU_EOI), irq);
}

static void realview_irq_enable(struct irq_t * irq)
{
	u32_t mask = 1 << (irq->no % 32);
	write32(phys_to_virt(REALVIEW_GIC_DIST_BASE + DIST_ENABLE_SET + (irq->no / 32) * 4), mask);
}

static void realview_irq_disable(struct irq_t * irq)
{
	u32_t mask = 1 << (irq->no % 32);
	write32(phys_to_virt(REALVIEW_GIC_DIST_BASE + DIST_ENABLE_CLEAR + (irq->no / 32) * 4), mask);
}

static void realview_irq_set_type(struct irq_t * irq, enum irq_type_t type)
{
}

static struct irq_t realview_irqs[] = {
	{
		.name		= "WDOG",
		.no			= 32 + 0,
		.handler	= &realview_irq_handler[0],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "SOFT",
		.no			= 32 + 1,
		.handler	= &realview_irq_handler[1],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "DBGURX",
		.no			= 32 + 2,
		.handler	= &realview_irq_handler[2],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "DBGUTX",
		.no			= 32 + 3,
		.handler	= &realview_irq_handler[3],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "TMIER0_1",
		.no			= 32 + 4,
		.handler	= &realview_irq_handler[4],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "TMIER2_3",
		.no			= 32 + 5,
		.handler	= &realview_irq_handler[5],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "GPIO0",
		.no			= 32 + 6,
		.handler	= &realview_irq_handler[6],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "GPIO1",
		.no			= 32 + 7,
		.handler	= &realview_irq_handler[7],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "GPIO2",
		.no			= 32 + 8,
		.handler	= &realview_irq_handler[8],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "RTC",
		.no			= 32 + 10,
		.handler	= &realview_irq_handler[10],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "SSP",
		.no			= 32 + 11,
		.handler	= &realview_irq_handler[11],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "UART0",
		.no			= 32 + 12,
		.handler	= &realview_irq_handler[12],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "UART1",
		.no			= 32 + 13,
		.handler	= &realview_irq_handler[13],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "UART2",
		.no			= 32 + 14,
		.handler	= &realview_irq_handler[14],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "UART3",
		.no			= 32 + 15,
		.handler	= &realview_irq_handler[15],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "SCI",
		.no			= 32 + 16,
		.handler	= &realview_irq_handler[16],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "MMCI0A",
		.no			= 32 + 17,
		.handler	= &realview_irq_handler[17],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "MMCI0B",
		.no			= 32 + 18,
		.handler	= &realview_irq_handler[18],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "AACI",
		.no			= 32 + 19,
		.handler	= &realview_irq_handler[19],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "KMI0",
		.no			= 32 + 20,
		.handler	= &realview_irq_handler[20],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "KMI1",
		.no			= 32 + 21,
		.handler	= &realview_irq_handler[21],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "CHARLCD",
		.no			= 32 + 22,
		.handler	= &realview_irq_handler[22],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "CLCD",
		.no			= 32 + 23,
		.handler	= &realview_irq_handler[23],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "DMA",
		.no			= 32 + 24,
		.handler	= &realview_irq_handler[24],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "PWRFAIL",
		.no			= 32 + 25,
		.handler	= &realview_irq_handler[25],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "PISMO",
		.no			= 32 + 26,
		.handler	= &realview_irq_handler[26],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "DOC",
		.no			= 32 + 27,
		.handler	= &realview_irq_handler[27],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "ETH",
		.no			= 32 + 28,
		.handler	= &realview_irq_handler[28],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "USB",
		.no			= 32 + 29,
		.handler	= &realview_irq_handler[29],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "TSPEN",
		.no			= 32 + 30,
		.handler	= &realview_irq_handler[30],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}, {
		.name		= "TSKPAD",
		.no			= 32 + 31,
		.handler	= &realview_irq_handler[31],
		.enable		= realview_irq_enable,
		.disable	= realview_irq_disable,
		.set_type	= realview_irq_set_type,
	}
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
	cpumask = 1 << smp_processor_id();
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

static __init void realview_irq_init(void)
{
	int i;

	gic_dist_init(REALVIEW_GIC_DIST_BASE);
	gic_cpu_init(REALVIEW_GIC_DIST_BASE, REALVIEW_GIC_CPU_BASE);

	for(i = 0; i < ARRAY_SIZE(realview_irqs); i++)
	{
		if(irq_register(&realview_irqs[i]))
			LOG("Register irq '%s'", realview_irqs[i].name);
		else
			LOG("Failed to register irq '%s'", realview_irqs[i].name);
	}

	vic_enable();
	irq_enable();
	fiq_enable();
}

static __exit void realview_irq_exit(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(realview_irqs); i++)
	{
		if(irq_unregister(&realview_irqs[i]))
			LOG("Unregister irq '%s'", realview_irqs[i].name);
		else
			LOG("Failed to unregister irq '%s'", realview_irqs[i].name);
	}

	vic_disable();
	irq_disable();
	fiq_disable();
}

core_initcall(realview_irq_init);
core_exitcall(realview_irq_exit);
