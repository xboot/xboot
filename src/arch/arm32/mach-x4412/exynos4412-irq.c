/*
 * exynos4412-irq.c
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
#include <exynos4412/reg-gpio.h>
#include <exynos4412/reg-gic.h>
#include <exynos4412/reg-combiner.h>
#include <exynos4412/reg-timer.h>

struct pt_regs_t {
	u32_t	r0,		r1,		r2,		r3, 	r4,		r5;
	u32_t	r6,		r7,		r8, 	r9, 	r10,	fp;
	u32_t	ip, 	sp, 	lr, 	pc,		cpsr, 	orig_r0;
};

static struct irq_handler_t exynos4412_irq_handler[160];

void do_irqs(struct pt_regs_t * regs)
{
	u32_t irq;

	/* Get irq's offset */
	irq = read32(EXYNOS4412_GIC_CPU_BASE + GIC_CPU_INTACK) & 0x3ff;

	/* Handle interrupt server function */
	(exynos4412_irq_handler[irq - 32].func)(exynos4412_irq_handler[irq - 32].data);

	/* Exit interrupt */
	write32(EXYNOS4412_GIC_CPU_BASE + GIC_CPU_EOI, irq);
}

static void exynos4412_irq_enable(struct irq_t * irq)
{
	u32_t mask = 1 << (irq->no % 32);
	write32(EXYNOS4412_GIC_DIST_BASE + GIC_DIST_ENABLE_SET + (irq->no / 32) * 4, mask);
}

static void exynos4412_irq_disable(struct irq_t * irq)
{
	u32_t mask = 1 << (irq->no % 32);
	write32(EXYNOS4412_GIC_DIST_BASE + GIC_DIST_ENABLE_CLEAR + (irq->no / 32) * 4, mask);
}

static void exynos4412_irq_set_type(struct irq_t * irq, enum irq_type_t type)
{
}

static struct irq_t exynos4412_irqs[] = {
	{
		.name		= "INTG0",
		.no			= 32,
		.handler	= &exynos4412_irq_handler[32 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG1",
		.no			= 33,
		.handler	= &exynos4412_irq_handler[33 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG2",
		.no			= 34,
		.handler	= &exynos4412_irq_handler[34 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG3",
		.no			= 35,
		.handler	= &exynos4412_irq_handler[35 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG4",
		.no			= 36,
		.handler	= &exynos4412_irq_handler[36 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG5",
		.no			= 37,
		.handler	= &exynos4412_irq_handler[37 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG6",
		.no			= 38,
		.handler	= &exynos4412_irq_handler[38 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG7",
		.no			= 39,
		.handler	= &exynos4412_irq_handler[39 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG8",
		.no			= 40,
		.handler	= &exynos4412_irq_handler[40 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG9",
		.no			= 41,
		.handler	= &exynos4412_irq_handler[41 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG10",
		.no			= 42,
		.handler	= &exynos4412_irq_handler[42 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG11",
		.no			= 43,
		.handler	= &exynos4412_irq_handler[43 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG12",
		.no			= 44,
		.handler	= &exynos4412_irq_handler[44 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG13",
		.no			= 45,
		.handler	= &exynos4412_irq_handler[45 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG14",
		.no			= 46,
		.handler	= &exynos4412_irq_handler[46 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG15",
		.no			= 47,
		.handler	= &exynos4412_irq_handler[47 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT0",
		.no			= 48,
		.handler	= &exynos4412_irq_handler[48 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT1",
		.no			= 49,
		.handler	= &exynos4412_irq_handler[49 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT2",
		.no			= 50,
		.handler	= &exynos4412_irq_handler[50 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT3",
		.no			= 51,
		.handler	= &exynos4412_irq_handler[51 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT4",
		.no			= 52,
		.handler	= &exynos4412_irq_handler[52 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT5",
		.no			= 53,
		.handler	= &exynos4412_irq_handler[53 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT6",
		.no			= 54,
		.handler	= &exynos4412_irq_handler[54 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT7",
		.no			= 55,
		.handler	= &exynos4412_irq_handler[55 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT8",
		.no			= 56,
		.handler	= &exynos4412_irq_handler[56 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT9",
		.no			= 57,
		.handler	= &exynos4412_irq_handler[57 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT10",
		.no			= 58,
		.handler	= &exynos4412_irq_handler[58 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT11",
		.no			= 59,
		.handler	= &exynos4412_irq_handler[59 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT12",
		.no			= 60,
		.handler	= &exynos4412_irq_handler[60 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT13",
		.no			= 61,
		.handler	= &exynos4412_irq_handler[61 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT14",
		.no			= 62,
		.handler	= &exynos4412_irq_handler[62 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT15",
		.no			= 63,
		.handler	= &exynos4412_irq_handler[63 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "EINT16_31",
		.no			= 64,
		.handler	= &exynos4412_irq_handler[64 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "C2C_SSCM0",
		.no			= 65,
		.handler	= &exynos4412_irq_handler[65 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "MDMA",
		.no			= 66,
		.handler	= &exynos4412_irq_handler[66 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "PDMA0",
		.no			= 67,
		.handler	= &exynos4412_irq_handler[67 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "PDMA1",
		.no			= 68,
		.handler	= &exynos4412_irq_handler[68 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "TIMER0",
		.no			= 69,
		.handler	= &exynos4412_irq_handler[69 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "TIMER1",
		.no			= 70,
		.handler	= &exynos4412_irq_handler[70 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "TIMER2",
		.no			= 71,
		.handler	= &exynos4412_irq_handler[71 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "TIMER3",
		.no			= 72,
		.handler	= &exynos4412_irq_handler[72 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "TIMER4",
		.no			= 73,
		.handler	= &exynos4412_irq_handler[73 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG19",
		.no			= 74,
		.handler	= &exynos4412_irq_handler[74 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "WDT",
		.no			= 75,
		.handler	= &exynos4412_irq_handler[75 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "RTC_ALARM",
		.no			= 76,
		.handler	= &exynos4412_irq_handler[76 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "RTC_TIC",
		.no			= 77,
		.handler	= &exynos4412_irq_handler[77 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "GPIO_RT",
		.no			= 78,
		.handler	= &exynos4412_irq_handler[78 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "GPIO_LB",
		.no			= 79,
		.handler	= &exynos4412_irq_handler[79 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG18",
		.no			= 80,
		.handler	= &exynos4412_irq_handler[80 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "IEM_APC",
		.no			= 81,
		.handler	= &exynos4412_irq_handler[81 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "IEM_IEC",
		.no			= 82,
		.handler	= &exynos4412_irq_handler[82 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "NFC",
		.no			= 83,
		.handler	= &exynos4412_irq_handler[83 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "UART0",
		.no			= 84,
		.handler	= &exynos4412_irq_handler[84 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "UART1",
		.no			= 85,
		.handler	= &exynos4412_irq_handler[85 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "UART2",
		.no			= 86,
		.handler	= &exynos4412_irq_handler[86 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "UART3",
		.no			= 87,
		.handler	= &exynos4412_irq_handler[87 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "G0_IRQ",
		.no			= 89,
		.handler	= &exynos4412_irq_handler[89 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "I2C0",
		.no			= 90,
		.handler	= &exynos4412_irq_handler[90 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "I2C1",
		.no			= 91,
		.handler	= &exynos4412_irq_handler[91 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "I2C2",
		.no			= 92,
		.handler	= &exynos4412_irq_handler[92 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "I2C3",
		.no			= 93,
		.handler	= &exynos4412_irq_handler[93 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "I2C4",
		.no			= 94,
		.handler	= &exynos4412_irq_handler[94 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "I2C5",
		.no			= 95,
		.handler	= &exynos4412_irq_handler[95 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "I2C6",
		.no			= 96,
		.handler	= &exynos4412_irq_handler[96 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "I2C7",
		.no			= 97,
		.handler	= &exynos4412_irq_handler[97 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "SPI0",
		.no			= 98,
		.handler	= &exynos4412_irq_handler[98 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "SPI1",
		.no			= 99,
		.handler	= &exynos4412_irq_handler[99 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "SPI2",
		.no			= 100,
		.handler	= &exynos4412_irq_handler[100 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "G1_IRQ",
		.no			= 101,
		.handler	= &exynos4412_irq_handler[101 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "UHOST",
		.no			= 102,
		.handler	= &exynos4412_irq_handler[102 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "HSOTG",
		.no			= 103,
		.handler	= &exynos4412_irq_handler[103 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "GPIO_C2C",
		.no			= 104,
		.handler	= &exynos4412_irq_handler[104 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "HSMMC0",
		.no			= 105,
		.handler	= &exynos4412_irq_handler[105 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "HSMMC1",
		.no			= 106,
		.handler	= &exynos4412_irq_handler[106 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "HSMMC2",
		.no			= 107,
		.handler	= &exynos4412_irq_handler[107 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	},{
		.name		= "HSMMC3",
		.no			= 108,
		.handler	= &exynos4412_irq_handler[108 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "SDMMC",
		.no			= 109,
		.handler	= &exynos4412_irq_handler[109 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "MIPI_CSI_4LANE",
		.no			= 110,
		.handler	= &exynos4412_irq_handler[110 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "MIPI_DSI_4LANE",
		.no			= 111,
		.handler	= &exynos4412_irq_handler[111 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "MIPI_CSI_2LANE",
		.no			= 112,
		.handler	= &exynos4412_irq_handler[112 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "ROTATOR",
		.no			= 115,
		.handler	= &exynos4412_irq_handler[115 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "FIMC0",
		.no			= 116,
		.handler	= &exynos4412_irq_handler[116 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	},{
		.name		= "FIMC1",
		.no			= 117,
		.handler	= &exynos4412_irq_handler[117 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "FIMC2",
		.no			= 118,
		.handler	= &exynos4412_irq_handler[118 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "FIMC3",
		.no			= 119,
		.handler	= &exynos4412_irq_handler[119 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "JPEG",
		.no			= 120,
		.handler	= &exynos4412_irq_handler[120 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "G2D",
		.no			= 121,
		.handler	= &exynos4412_irq_handler[121 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "ISP0",
		.no			= 122,
		.handler	= &exynos4412_irq_handler[122 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "MIXER",
		.no			= 123,
		.handler	= &exynos4412_irq_handler[123 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	},{
		.name		= "HDMI",
		.no			= 124,
		.handler	= &exynos4412_irq_handler[124 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "HDMI_I2C",
		.no			= 125,
		.handler	= &exynos4412_irq_handler[125 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "MFC",
		.no			= 126,
		.handler	= &exynos4412_irq_handler[126 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "ISP1",
		.no			= 127,
		.handler	= &exynos4412_irq_handler[127 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "AUDIO_SS",
		.no			= 128,
		.handler	= &exynos4412_irq_handler[128 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "I2S0",
		.no			= 129,
		.handler	= &exynos4412_irq_handler[129 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "I2S1",
		.no			= 130,
		.handler	= &exynos4412_irq_handler[130 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	},{
		.name		= "I2S2",
		.no			= 131,
		.handler	= &exynos4412_irq_handler[131 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "AC97",
		.no			= 132,
		.handler	= &exynos4412_irq_handler[132 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "PCM0",
		.no			= 133,
		.handler	= &exynos4412_irq_handler[133 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "PCM1",
		.no			= 134,
		.handler	= &exynos4412_irq_handler[134 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "PCM2",
		.no			= 135,
		.handler	= &exynos4412_irq_handler[135 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "SPDIF",
		.no			= 136,
		.handler	= &exynos4412_irq_handler[136 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "FIMC_LITE0",
		.no			= 137,
		.handler	= &exynos4412_irq_handler[137 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	},{
		.name		= "FIMC_LITE1",
		.no			= 138,
		.handler	= &exynos4412_irq_handler[138 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG16",
		.no			= 139,
		.handler	= &exynos4412_irq_handler[139 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "INTG17",
		.no			= 140,
		.handler	= &exynos4412_irq_handler[140 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "KEYPAD",
		.no			= 141,
		.handler	= &exynos4412_irq_handler[141 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "PMU",
		.no			= 142,
		.handler	= &exynos4412_irq_handler[142 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "GPS",
		.no			= 143,
		.handler	= &exynos4412_irq_handler[143 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "SSS",
		.no			= 144,
		.handler	= &exynos4412_irq_handler[144 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	},{
		.name		= "SLIMBUS",
		.no			= 145,
		.handler	= &exynos4412_irq_handler[145 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "CEC",
		.no			= 146,
		.handler	= &exynos4412_irq_handler[146 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "TSI",
		.no			= 147,
		.handler	= &exynos4412_irq_handler[147 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "C2C_SSCM1",
		.no			= 148,
		.handler	= &exynos4412_irq_handler[148 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "G3D_IRQPMU",
		.no			= 149,
		.handler	= &exynos4412_irq_handler[149 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "G3D_IRQPPMMU0",
		.no			= 150,
		.handler	= &exynos4412_irq_handler[150 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "G3D_IRQPPMMU1",
		.no			= 151,
		.handler	= &exynos4412_irq_handler[151 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	},{
		.name		= "G3D_IRQPPMMU2",
		.no			= 152,
		.handler	= &exynos4412_irq_handler[152 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "G3D_IRQPPMMU3",
		.no			= 153,
		.handler	= &exynos4412_irq_handler[153 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "G3D_IRQGPMMU",
		.no			= 154,
		.handler	= &exynos4412_irq_handler[154 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "G3D_IRQPP0",
		.no			= 155,
		.handler	= &exynos4412_irq_handler[155 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "G3D_IRQPP1",
		.no			= 156,
		.handler	= &exynos4412_irq_handler[156 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "G3D_IRQPP2",
		.no			= 157,
		.handler	= &exynos4412_irq_handler[157 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "G3D_IRQPP3",
		.no			= 158,
		.handler	= &exynos4412_irq_handler[158 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	}, {
		.name		= "G3D_IRQGP",
		.no			= 159,
		.handler	= &exynos4412_irq_handler[159 - 32],
		.enable		= exynos4412_irq_enable,
		.disable	= exynos4412_irq_disable,
		.set_type	= exynos4412_irq_set_type,
	},
};

static void gic_dist_init(physical_addr_t dist)
{
	u32_t gic_irqs;
	u32_t cpumask;
	u32_t i;

	write32(dist + GIC_DIST_CTRL, 0x0);

	/*
	 * Find out how many interrupts are supported.
	 * The GIC only supports up to 1020 interrupt sources.
	 */
	gic_irqs = read32(dist + GIC_DIST_CTR) & 0x1f;
	gic_irqs = (gic_irqs + 1) * 32;
	if(gic_irqs > 1020)
		gic_irqs = 1020;

	/*
	 * Set all global interrupts to be level triggered, active low.
	 */
	for(i = 32; i < gic_irqs; i += 16)
		write32(dist + GIC_DIST_CONFIG + i * 4 / 16, 0);

	/*
	 * Set all global interrupts to this CPU only.
	 */
	cpumask = 1 << get_cpuid();
	cpumask |= cpumask << 8;
	cpumask |= cpumask << 16;

	for(i = 32; i < gic_irqs; i += 4)
		write32(dist + GIC_DIST_TARGET + i * 4 / 4, cpumask);

	/*
	 * Set priority on all global interrupts.
	 */
	for(i = 32; i < gic_irqs; i += 4)
		write32(dist + GIC_DIST_PRI + i * 4 / 4, 0xa0a0a0a0);

	/*
	 * Disable all interrupts.  Leave the PPI and SGIs alone
	 * as these enables are banked registers.
	 */
	for(i = 32; i < gic_irqs; i += 32)
		write32(dist + GIC_DIST_ENABLE_CLEAR + i * 4 / 32, 0xffffffff);

	write32(dist + GIC_DIST_CTRL, 0x1);
}

static void gic_cpu_init(physical_addr_t dist, physical_addr_t cpu)
{
	int i;

	/*
	 * Deal with the banked PPI and SGI interrupts - disable all
	 * PPI interrupts, ensure all SGI interrupts are enabled.
	 */
	write32(dist + GIC_DIST_ENABLE_CLEAR, 0xffff0000);
	write32(dist + GIC_DIST_ENABLE_SET, 0x0000ffff);

	/*
	 * Set priority on PPI and SGI interrupts
	 */
	for(i = 0; i < 32; i += 4)
		write32(dist + GIC_DIST_PRI + i * 4 / 4, 0xa0a0a0a0);

	write32(cpu + GIC_CPU_PRIMASK, 0xf0);
	write32(cpu + GIC_CPU_CTRL, 0x1);
}

static void combiner_init(physical_addr_t comb)
{
	int i;

	for(i = 0; i < 5; i++)
		write32(comb + COMBINER_ENABLE_CLEAR + i * 0x10, 0xffffffff);
}

static __init void exynos4412_irq_init(void)
{
	int i;

	gic_dist_init(EXYNOS4412_GIC_DIST_BASE);
	gic_cpu_init(EXYNOS4412_GIC_DIST_BASE, EXYNOS4412_GIC_CPU_BASE);
	combiner_init(EXYNOS4412_COMBINER_BASE);

	for(i = 0; i < ARRAY_SIZE(exynos4412_irqs); i++)
	{
		if(irq_register(&exynos4412_irqs[i]))
			LOG("Register irq '%s'", exynos4412_irqs[i].name);
		else
			LOG("Failed to register irq '%s'", exynos4412_irqs[i].name);
	}

	/* Enable vector interrupt controller */
	vic_enable();

	/* Enable irq and fiq */
	irq_enable();
	fiq_enable();
}

static __exit void exynos4412_irq_exit(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(exynos4412_irqs); i++)
	{
		if(irq_unregister(&exynos4412_irqs[i]))
			LOG("Unregister irq '%s'", exynos4412_irqs[i].name);
		else
			LOG("Failed to unregister irq '%s'", exynos4412_irqs[i].name);
	}

	/* Disable vector interrupt controller */
	vic_disable();

	/* Disable irq and fiq */
	irq_disable();
	fiq_disable();
}

core_initcall(exynos4412_irq_init);
core_exitcall(exynos4412_irq_exit);
