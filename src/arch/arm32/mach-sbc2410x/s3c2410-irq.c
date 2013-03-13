/*
 * arch/arm/mach-sbc2410x/s3c2410-irq.c
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
#include <s3c2410/reg-gpio.h>
#include <s3c2410/reg-int.h>
#include <s3c2410/reg-lcd.h>

/*
 * the struct of regs, which saved and restore in the interrupt.
 */
struct regs {
	u32_t	r0,		r1,		r2,		r3, 	r4,		r5;
	u32_t	r6,		r7,		r8, 	r9, 	r10,	fp;
	u32_t	ip, 	sp, 	lr, 	pc,		cpsr, 	orig_r0;
};

/*
 * the irq handler.
 */
static irq_handler s3c2410_irq_handler[32];
static irq_handler s3c2410_irq_eint4_23_handler[4 + 20];

/*
 * enable interrupts.
 */
static bool_t interrupts_enable(void)
{
	u32_t temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "bic %0, %0, #0x80\n"
			     "msr cpsr_c, %0"
			     : "=r" (temp)
			     :
			     : "memory");

	return TRUE;
}

/*
 * disable interrupts.
 */
static bool_t interrupts_disable(void)
{
	u32_t old, temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "orr %1, %0, #0xc0\n"
			     "msr cpsr_c, %1"
			     : "=r" (old), "=r" (temp)
			     :
			     : "memory");

	return TRUE;
}

/*
 * do irqs.
 */
void do_irqs(struct regs * regs)
{
	u32_t offset;

	/* read interrupt offset */
	offset = readl(S3C2410_INTOFFSET);

	/* running interrupt server function */
	(s3c2410_irq_handler[offset])();

	/* clear interrupt pending flag */
	writel( S3C2410_SRCPND, (0x1<<offset) );
	writel( S3C2410_INTPND, readl(S3C2410_INTPND) );
}

/*
 * enable or disable irq.
 */
static void enable_irqs(struct irq * irq, bool_t enable)
{
	u32_t irq_no = irq->irq_no;

	if(irq_no < 32)
	{
		if(enable)
			writel( S3C2410_INTMSK, (readl(S3C2410_INTMSK) & (~(0x1<<irq_no))) );
		else
			writel( S3C2410_INTMSK, (readl(S3C2410_INTMSK) | (0x1<<irq_no)) );
	}
	else
	{
		irq_no = irq_no - 32;

		if(enable)
			writel( S3C2410_EINTMASK, (readl(S3C2410_EINTMASK) & (~(0x1<<irq_no))) );
		else
			writel( S3C2410_EINTMASK, (readl(S3C2410_EINTMASK) | (0x1<<irq_no)) );
	}
}

/*
 * sub irq for eint 4-23
 */
static void irq_eint4_23(void)
{
	u32_t eint, i;

	/* read interrupt offset */
	eint = readl(S3C2410_EINTPEND);

	for(i = 4; i < 24; i++)
	{
		if(eint & (0x1<<i))
			break;
	}

	if(i == 24)
		return;

	/* running interrupt server function */
	(s3c2410_irq_eint4_23_handler[i])();

	/* clear extern interrupt pending flag */
	writel( S3C2410_EINTPEND, (0x1<<i) );
}

/*
 * the array of irq.
 */
static struct irq s3c2410_irqs[] = {
	{
		.name		= "EINT0",
		.irq_no		= 0,
		.handler	= &s3c2410_irq_handler[0],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT1",
		.irq_no		= 1,
		.handler	= &s3c2410_irq_handler[1],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT2",
		.irq_no		= 2,
		.handler	= &s3c2410_irq_handler[2],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT3",
		.irq_no		= 3,
		.handler	= &s3c2410_irq_handler[3],
		.enable		= enable_irqs,
	}, {
		.name		= "BATT_FLT",
		.irq_no		= 7,
		.handler	= &s3c2410_irq_handler[7],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_TICK",
		.irq_no		= 8,
		.handler	= &s3c2410_irq_handler[8],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_WDT",
		.irq_no		= 9,
		.handler	= &s3c2410_irq_handler[9],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_TIMER0",
		.irq_no		= 10,
		.handler	= &s3c2410_irq_handler[10],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_TIMER1",
		.irq_no		= 11,
		.handler	= &s3c2410_irq_handler[11],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_TIMER2",
		.irq_no		= 12,
		.handler	= &s3c2410_irq_handler[12],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_TIMER3",
		.irq_no		= 13,
		.handler	= &s3c2410_irq_handler[13],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_TIMER4",
		.irq_no		= 14,
		.handler	= &s3c2410_irq_handler[14],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_UART2",
		.irq_no		= 15,
		.handler	= &s3c2410_irq_handler[15],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_LCD",
		.irq_no		= 16,
		.handler	= &s3c2410_irq_handler[16],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_DMA0",
		.irq_no		= 17,
		.handler	= &s3c2410_irq_handler[17],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_DMA1",
		.irq_no		= 18,
		.handler	= &s3c2410_irq_handler[18],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_DMA2",
		.irq_no		= 19,
		.handler	= &s3c2410_irq_handler[19],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_DMA3",
		.irq_no		= 20,
		.handler	= &s3c2410_irq_handler[20],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_SDI",
		.irq_no		= 21,
		.handler	= &s3c2410_irq_handler[21],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_SPI0",
		.irq_no		= 22,
		.handler	= &s3c2410_irq_handler[22],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_UART1",
		.irq_no		= 23,
		.handler	= &s3c2410_irq_handler[23],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_USBD",
		.irq_no		= 25,
		.handler	= &s3c2410_irq_handler[25],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_USBH",
		.irq_no		= 26,
		.handler	= &s3c2410_irq_handler[26],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_IIC",
		.irq_no		= 27,
		.handler	= &s3c2410_irq_handler[27],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_UART0",
		.irq_no		= 28,
		.handler	= &s3c2410_irq_handler[28],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_SPI1",
		.irq_no		= 29,
		.handler	= &s3c2410_irq_handler[29],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_RTC",
		.irq_no		= 30,
		.handler	= &s3c2410_irq_handler[30],
		.enable		= enable_irqs,
	}, {
		.name		= "INT_ADC",
		.irq_no		= 31,
		.handler	= &s3c2410_irq_handler[31],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT4",
		.irq_no		= 36,
		.handler	= &s3c2410_irq_eint4_23_handler[4],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT5",
		.irq_no		= 37,
		.handler	= &s3c2410_irq_eint4_23_handler[5],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT6",
		.irq_no		= 38,
		.handler	= &s3c2410_irq_eint4_23_handler[6],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT7",
		.irq_no		= 39,
		.handler	= &s3c2410_irq_eint4_23_handler[7],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT8",
		.irq_no		= 40,
		.handler	= &s3c2410_irq_eint4_23_handler[8],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT9",
		.irq_no		= 41,
		.handler	= &s3c2410_irq_eint4_23_handler[9],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT10",
		.irq_no		= 42,
		.handler	= &s3c2410_irq_eint4_23_handler[10],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT11",
		.irq_no		= 43,
		.handler	= &s3c2410_irq_eint4_23_handler[11],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT12",
		.irq_no		= 44,
		.handler	= &s3c2410_irq_eint4_23_handler[12],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT13",
		.irq_no		= 45,
		.handler	= &s3c2410_irq_eint4_23_handler[13],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT14",
		.irq_no		= 46,
		.handler	= &s3c2410_irq_eint4_23_handler[14],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT15",
		.irq_no		= 47,
		.handler	= &s3c2410_irq_eint4_23_handler[15],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT16",
		.irq_no		= 48,
		.handler	= &s3c2410_irq_eint4_23_handler[16],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT17",
		.irq_no		= 49,
		.handler	= &s3c2410_irq_eint4_23_handler[17],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT18",
		.irq_no		= 50,
		.handler	= &s3c2410_irq_eint4_23_handler[18],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT19",
		.irq_no		= 51,
		.handler	= &s3c2410_irq_eint4_23_handler[19],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT20",
		.irq_no		= 52,
		.handler	= &s3c2410_irq_eint4_23_handler[20],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT21",
		.irq_no		= 53,
		.handler	= &s3c2410_irq_eint4_23_handler[21],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT22",
		.irq_no		= 54,
		.handler	= &s3c2410_irq_eint4_23_handler[22],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT23",
		.irq_no		= 55,
		.handler	= &s3c2410_irq_eint4_23_handler[23],
		.enable		= enable_irqs,
	}
};

/*
 * null function for irq handler
 */
static void null_irq_handler(void)	{ }

static __init void s3c2410_irq_init(void)
{
	u32_t i;

	/* all irq mode. */
	writel(S3C2410_INTMOD, 0x00000000);

	/* clear all interrupt pending */
	writel( S3C2410_SRCPND, readl(S3C2410_SRCPND) );
	writel( S3C2410_INTPND, readl(S3C2410_INTPND) );
	writel( S3C2410_SUBSRCPND, readl(S3C2410_SUBSRCPND) );
	writel( S3C2410_EINTPEND, readl(S3C2410_EINTPEND) );
	writel( S3C2410_LCDSRCPND, readl(S3C2410_LCDSRCPND) );
	writel( S3C2410_LCDINTPND, readl(S3C2410_LCDINTPND) );

	/* all interrupt is masked, but EINT4_7 and EINT8_23 and SUB_INT */
	writel(S3C2410_INTMSK, 0xffffffcf);
	writel(S3C2410_EINTMASK, 0x00fffff0);
	writel(S3C2410_INTSUBMSK, 0x00000000);
	writel(S3C2410_LCDINTMSK, readl(S3C2410_LCDINTMSK)&(~0x3));

	/* irq priority for default */
	writel(S3C2410_PRIORITY, 0x7f);

	for(i = 0; i< 32; i++)
	{
		s3c2410_irq_handler[i] = (irq_handler)null_irq_handler;
	}

	for(i = 4; i< 24; i++)
	{
		s3c2410_irq_eint4_23_handler[i] = (irq_handler)null_irq_handler;
	}

	s3c2410_irq_handler[4] = (irq_handler)irq_eint4_23;
	s3c2410_irq_handler[5] = (irq_handler)irq_eint4_23;

	for(i = 0; i < ARRAY_SIZE(s3c2410_irqs); i++)
	{
		if(!irq_register(&s3c2410_irqs[i]))
		{
			LOG_E("failed to register irq '%s'", s3c2410_irqs[i].name);
		}
	}

	/* enable interrupt */
	interrupts_enable();
}

static __exit void s3c2410_irq_exit(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(s3c2410_irqs); i++)
	{
		if(!irq_unregister(&s3c2410_irqs[i]))
		{
			LOG_E("failed to unregister irq '%s'", s3c2410_irqs[i].name);
		}
	}

	/* clear all interrupt pending */
	writel( S3C2410_SRCPND, readl(S3C2410_SRCPND) );
	writel( S3C2410_INTPND, readl(S3C2410_INTPND) );
	writel( S3C2410_SUBSRCPND, readl(S3C2410_SUBSRCPND) );
	writel( S3C2410_EINTPEND, readl(S3C2410_EINTPEND) );
	writel( S3C2410_LCDSRCPND, readl(S3C2410_LCDSRCPND) );
	writel( S3C2410_LCDINTPND, readl(S3C2410_LCDINTPND) );

	/* all interrupt is masked */
	writel(S3C2410_INTMSK, 0xffffffff);
	writel(S3C2410_EINTMASK, 0x00fffff0);
	writel(S3C2410_INTSUBMSK, 0xffffffff);
	writel(S3C2410_LCDINTMSK, readl(S3C2410_LCDINTMSK) |0x3);

	/* disable interrupt */
	interrupts_disable();
}

core_initcall(s3c2410_irq_init);
core_exitcall(s3c2410_irq_exit);
