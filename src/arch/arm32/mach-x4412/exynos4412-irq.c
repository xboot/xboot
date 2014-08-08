/*
 * exynos4412-irq.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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
#include <exynos4412/reg-gpio.h>
#include <exynos4412/reg-gic.h>
#include <exynos4412/reg-combiner.h>
#include <exynos4412/reg-timer.h>

struct pt_regs_t {
	u32_t	r0,		r1,		r2,		r3, 	r4,		r5;
	u32_t	r6,		r7,		r8, 	r9, 	r10,	fp;
	u32_t	ip, 	sp, 	lr, 	pc,		cpsr, 	orig_r0;
};

void do_irqs(struct pt_regs_t * regs)
{
	u32_t irq;

	/* Get irq's offset */
	irq = readl(EXYNOS4412_GIC_CPU_BASE + GIC_CPU_INTACK) & 0x3ff;

	/* Handle interrupt server function */
	printk("do irqs = 0x%08x\r\n", irq);

	/* Exit interrupt */
	writel(EXYNOS4412_GIC_CPU_BASE + GIC_CPU_EOI, irq);
}

static void enable_irqs(struct irq_t * irq, bool_t enable)
{
	u32_t no = irq->irq_no;
	u32_t mask = 1 << (no % 32);

	if(enable)
		writel(EXYNOS4412_GIC_DIST_BASE + GIC_DIST_ENABLE_SET + no * 4 / 32, mask);
	else
		writel(EXYNOS4412_GIC_DIST_BASE + GIC_DIST_ENABLE_CLEAR + no * 4 / 32, mask);
}

static void gic_dist_init(physical_addr_t dist)
{
	u32_t gic_irqs;
	u32_t cpumask;
	u32_t i;

	/*
	 * Find out how many interrupts are supported.
	 * The GIC only supports up to 1020 interrupt sources.
	 */
	gic_irqs = readl(dist + GIC_DIST_CTR) & 0x1f;
	gic_irqs = (gic_irqs + 1) * 32;
	if(gic_irqs > 1020)
		gic_irqs = 1020;

	writel(dist + GIC_DIST_CTRL, 0x0);

	/*
	 * Set all global interrupts to be level triggered, active low.
	 */
	for(i = 32; i < gic_irqs; i += 16)
		writel(dist + GIC_DIST_CONFIG + i * 4 / 16, 0);

	/*
	 * Set all global interrupts to this CPU only.
	 */
	cpumask = 1 << 0;
	cpumask |= cpumask << 8;
	cpumask |= cpumask << 16;

	for(i = 32; i < gic_irqs; i += 4)
		writel(dist + GIC_DIST_TARGET + i * 4 / 4, cpumask);

	/*
	 * Set priority on all global interrupts.
	 */
	for(i = 32; i < gic_irqs; i += 4)
		writel(dist + GIC_DIST_PRI + i * 4 / 4, 0xa0a0a0a0);

	/*
	 * Disable all interrupts.  Leave the PPI and SGIs alone
	 * as these enables are banked registers.
	 */
	for(i = 32; i < gic_irqs; i += 32)
		writel(dist + GIC_DIST_ENABLE_CLEAR + i * 4 / 32, 0xffffffff);
//	writel(dist + GIC_DIST_ENABLE_SET + 64 * 4 / 32, 0xffffffff);

	writel(dist + GIC_DIST_CTRL, 0x1);
}

static void gic_cpu_init(physical_addr_t dist, physical_addr_t cpu)
{
	int i;

	/*
	 * Deal with the banked PPI and SGI interrupts - disable all
	 * PPI interrupts, ensure all SGI interrupts are enabled.
	 */
	writel(dist + GIC_DIST_ENABLE_CLEAR, 0xffff0000);
	writel(dist + GIC_DIST_ENABLE_SET, 0x0000ffff);

	/*
	 * Set priority on PPI and SGI interrupts
	 */
	for(i = 0; i < 32; i += 4)
		writel(dist + GIC_DIST_PRI + i * 4 / 4, 0xa0a0a0a0);

	writel(cpu + GIC_CPU_PRIMASK, 0xf0);
	writel(cpu + GIC_CPU_CTRL, 0x1);
}

static void combiner_init(physical_addr_t comb)
{
	int i;

	for(i = 0; i < 5; i++)
		writel(comb + COMBINER_ENABLE_CLEAR + i * 0x10, 0xffffffff);
}

void gic_test(void)
{
	printk("gic test...\r\n");

	gic_dist_init(EXYNOS4412_GIC_DIST_BASE);
	gic_cpu_init(EXYNOS4412_GIC_DIST_BASE, EXYNOS4412_GIC_CPU_BASE);
	combiner_init(EXYNOS4412_COMBINER_BASE);

	gpio_set_cfg(EXYNOS4412_GPX0(0), 0xF);
	gpio_set_cfg(EXYNOS4412_GPX0(1), 0xF);
	gpio_set_cfg(EXYNOS4412_GPX0(2), 0xF);
	gpio_set_cfg(EXYNOS4412_GPX0(3), 0xF);
	gpio_set_cfg(EXYNOS4412_GPX0(4), 0xF);
	gpio_set_cfg(EXYNOS4412_GPX0(5), 0xF);
	gpio_set_cfg(EXYNOS4412_GPX0(6), 0xF);
	gpio_set_cfg(EXYNOS4412_GPX0(7), 0xF);

	gpio_set_cfg(EXYNOS4412_GPX1(0), 0xF);
	gpio_set_cfg(EXYNOS4412_GPX1(1), 0xF);
	gpio_set_cfg(EXYNOS4412_GPX1(2), 0xF);
	gpio_set_cfg(EXYNOS4412_GPX1(3), 0xF);
	gpio_set_cfg(EXYNOS4412_GPX1(4), 0xF);
	gpio_set_cfg(EXYNOS4412_GPX1(5), 0xF);
	gpio_set_cfg(EXYNOS4412_GPX1(6), 0xF);
	gpio_set_cfg(EXYNOS4412_GPX1(7), 0xF);

	//gpio_set_pull(EXYNOS4412_GPX0(1), GPIO_PULL_UP);
	writel(0x11400000 + 0x0900, 0x0); //enable intterrupt
	writel(0x11400000 + 0x0904, 0x0); //enable intterrupt

	/* Enable vector interrupt controller */
	vic_enable();

	/* Enable irq and fiq */
	irq_enable();
	fiq_enable();
}

static __init void exynos4412_irq_init(void)
{
}

static __exit void exynos4412_irq_exit(void)
{
}

postcore_initcall(exynos4412_irq_init);
postcore_exitcall(exynos4412_irq_exit);
