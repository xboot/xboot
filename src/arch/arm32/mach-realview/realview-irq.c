/*
 * realview-irq.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <types.h>
#include <io.h>
#include <xboot/log.h>
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <xboot/irq.h>
#include <realview-cp15.h>
#include <realview/reg-gic.h>


/*
 * the struct of regs, which saved and restore in the interrupt.
 */
struct irq_regs {
	u32_t	r0,		r1,		r2,		r3, 	r4,		r5;
	u32_t	r6,		r7,		r8, 	r9, 	r10,	fp;
	u32_t	ip, 	sp, 	lr, 	pc,		cpsr, 	orig_r0;
};

/*
 * the irq handler.
 */
static irq_handler realview_irq_handler[32];

/*
 * null function for irq handler
 */
static void null_irq_handler(void) { }

/*
 * do irqs.
 */
void do_irqs(struct irq_regs * regs)
{
	u32_t irq;

	/* get irq's offset */
	irq = readl(REALVIEW_GIC1_CPU_INTACK) & 0x3ff;

	/* running interrupt server function */
	(realview_irq_handler[irq-32])();

	/* end of interrupt */
	writel(REALVIEW_GIC1_CPU_EOI, irq);
}

/*
 * enable or disable irq.
 */
static void enable_irqs(struct irq * irq, bool_t enable)
{
	u32_t mask = 1 << (irq->irq_no % 32);

	if(enable)
		writel(REALVIEW_GIC1_DIST_ENABLE_SET + (irq->irq_no / 32) * 4, mask);
	else
		writel(REALVIEW_GIC1_DIST_ENABLE_CLEAR + (irq->irq_no / 32) * 4, mask);
}

/*
 * the array of irq.
 */
static struct irq realview_irqs[] = {
	{
		.name		= "WDOG",
		.irq_no		= 32 + 0,
		.handler	= &realview_irq_handler[0],
		.enable		= enable_irqs,
	}, {
		.name		= "SOFT",
		.irq_no		= 32 + 1,
		.handler	= &realview_irq_handler[1],
		.enable		= enable_irqs,
	}, {
		.name		= "DBGURX",
		.irq_no		= 32 + 2,
		.handler	= &realview_irq_handler[2],
		.enable		= enable_irqs,
	}, {
		.name		= "DBGUTX",
		.irq_no		= 32 + 3,
		.handler	= &realview_irq_handler[3],
		.enable		= enable_irqs,
	}, {
		.name		= "TMIER0_1",
		.irq_no		= 32 + 4,
		.handler	= &realview_irq_handler[4],
		.enable		= enable_irqs,
	}, {
		.name		= "TMIER2_3",
		.irq_no		= 32 + 5,
		.handler	= &realview_irq_handler[5],
		.enable		= enable_irqs,
	}, {
		.name		= "GPIO0",
		.irq_no		= 32 + 6,
		.handler	= &realview_irq_handler[6],
		.enable		= enable_irqs,
	}, {
		.name		= "GPIO1",
		.irq_no		= 32 + 7,
		.handler	= &realview_irq_handler[7],
		.enable		= enable_irqs,
	}, {
		.name		= "GPIO2",
		.irq_no		= 32 + 8,
		.handler	= &realview_irq_handler[8],
		.enable		= enable_irqs,
	}, {
		.name		= "RTC",
		.irq_no		= 32 + 10,
		.handler	= &realview_irq_handler[10],
		.enable		= enable_irqs,
	}, {
		.name		= "SSP",
		.irq_no		= 32 + 11,
		.handler	= &realview_irq_handler[11],
		.enable		= enable_irqs,
	}, {
		.name		= "UART0",
		.irq_no		= 32 + 12,
		.handler	= &realview_irq_handler[12],
		.enable		= enable_irqs,
	}, {
		.name		= "UART1",
		.irq_no		= 32 + 13,
		.handler	= &realview_irq_handler[13],
		.enable		= enable_irqs,
	}, {
		.name		= "UART2",
		.irq_no		= 32 + 14,
		.handler	= &realview_irq_handler[14],
		.enable		= enable_irqs,
	}, {
		.name		= "UART3",
		.irq_no		= 32 + 15,
		.handler	= &realview_irq_handler[15],
		.enable		= enable_irqs,
	}, {
		.name		= "SCI",
		.irq_no		= 32 + 16,
		.handler	= &realview_irq_handler[16],
		.enable		= enable_irqs,
	}, {
		.name		= "MMCI0A",
		.irq_no		= 32 + 17,
		.handler	= &realview_irq_handler[17],
		.enable		= enable_irqs,
	}, {
		.name		= "MMCI0B",
		.irq_no		= 32 + 18,
		.handler	= &realview_irq_handler[18],
		.enable		= enable_irqs,
	}, {
		.name		= "AACI",
		.irq_no		= 32 + 19,
		.handler	= &realview_irq_handler[19],
		.enable		= enable_irqs,
	}, {
		.name		= "KMI0",
		.irq_no		= 32 + 20,
		.handler	= &realview_irq_handler[20],
		.enable		= enable_irqs,
	}, {
		.name		= "KMI1",
		.irq_no		= 32 + 21,
		.handler	= &realview_irq_handler[21],
		.enable		= enable_irqs,
	}, {
		.name		= "CHARLCD",
		.irq_no		= 32 + 22,
		.handler	= &realview_irq_handler[22],
		.enable		= enable_irqs,
	}, {
		.name		= "CLCD",
		.irq_no		= 32 + 23,
		.handler	= &realview_irq_handler[23],
		.enable		= enable_irqs,
	}, {
		.name		= "DMA",
		.irq_no		= 32 + 24,
		.handler	= &realview_irq_handler[24],
		.enable		= enable_irqs,
	}, {
		.name		= "PWRFAIL",
		.irq_no		= 32 + 25,
		.handler	= &realview_irq_handler[25],
		.enable		= enable_irqs,
	}, {
		.name		= "PISMO",
		.irq_no		= 32 + 26,
		.handler	= &realview_irq_handler[26],
		.enable		= enable_irqs,
	}, {
		.name		= "DOC",
		.irq_no		= 32 + 27,
		.handler	= &realview_irq_handler[27],
		.enable		= enable_irqs,
	}, {
		.name		= "ETH",
		.irq_no		= 32 + 28,
		.handler	= &realview_irq_handler[28],
		.enable		= enable_irqs,
	}, {
		.name		= "USB",
		.irq_no		= 32 + 29,
		.handler	= &realview_irq_handler[29],
		.enable		= enable_irqs,
	}, {
		.name		= "TSPEN",
		.irq_no		= 32 + 30,
		.handler	= &realview_irq_handler[30],
		.enable		= enable_irqs,
	}, {
		.name		= "TSKPAD",
		.irq_no		= 32 + 31,
		.handler	= &realview_irq_handler[31],
		.enable		= enable_irqs,
	}
};

static __init void realview_irq_init(void)
{
	u32_t i;
	u32_t max_irq;
	u32_t cpumask;

	/* get cpumask */
	cpumask = 1 << 0;
	cpumask |= cpumask << 8;
	cpumask |= cpumask << 16;

	/* ignore all peripheral interrupt signals */
	writel(REALVIEW_GIC1_DIST_CTRL, 0);

	/*
	 * find out how many interrupts are supported.
	 * and the GIC only supports up to 1020 interrupt sources.
	 */
	max_irq = readl(REALVIEW_GIC1_DIST_CTR) & 0x1f;
	max_irq = (max_irq + 1) * 32;

	if(max_irq > 1020)
		max_irq = 1020;

	/*
	 * set all global interrupts to be level triggered, active low.
	 */
	for(i = 32; i < max_irq; i += 16)
		writel(REALVIEW_GIC1_DIST_CONFIG + i * 4 / 16, 0);

	/*
	 * set all global interrupts to this CPU only.
	 */
	for(i = 32; i < max_irq; i += 4)
		writel(REALVIEW_GIC1_DIST_TARGET + i * 4 / 4, cpumask);

	/*
	 * set priority on all interrupts.
	 */
	for(i = 0; i < max_irq; i += 4)
		writel(REALVIEW_GIC1_DIST_PRI + i * 4 / 4, 0xa0a0a0a0);

	/*
	 * disable all interrupts.
	 */
	for(i = 0; i < max_irq; i += 32)
		writel(REALVIEW_GIC1_DIST_ENABLE_CLEAR + i * 4 / 32, 0xffffffff);

	/* monitor the peripheral interrupt signals */
	writel(REALVIEW_GIC1_DIST_CTRL, 1);

	/* the priority mask level for cpu interface */
	writel(REALVIEW_GIC1_CPU_PRIMASK, 0xf0);

	/* enable signalling of interrupts */
	writel(REALVIEW_GIC1_CPU_CTRL, 1);

	/* initial irq's handler to null_irq_handler */
	for(i = 0; i < ARRAY_SIZE(realview_irq_handler); i++)
	{
		realview_irq_handler[i] = (irq_handler)null_irq_handler;
	}

	for(i = 0; i < ARRAY_SIZE(realview_irqs); i++)
	{
		if(!irq_register(&realview_irqs[i]))
		{
			LOG_E("failed to register irq '%s'", realview_irqs[i].name);
		}
	}

	/* enable irq and fiq */
	irq_enable();
	fiq_enable();
}

static __exit void realview_irq_exit(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(realview_irqs); i++)
	{
		if(!irq_unregister(&realview_irqs[i]))
		{
			LOG_E("failed to unregister irq '%s'", realview_irqs[i].name);
		}
	}

	/* disable irq and fiq */
	irq_disable();
	fiq_disable();
}

core_initcall(realview_irq_init);
core_exitcall(realview_irq_exit);
