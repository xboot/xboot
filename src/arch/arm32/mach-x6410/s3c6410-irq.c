/*
 * arch/arm/mach-x6410/s3c6410-irq.c
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
#include <s3c6410/reg-gpio.h>
#include <s3c6410/reg-vic.h>
#include <s3c6410-cp15.h>

/*
 * exception handlers for irq from start.s
 */
extern void irq(void);

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
static irq_handler s3c6410_irq_handler[64];

/*
 * null function for irq handler
 */
static void null_irq_handler(void) { }

/*
 * get interrupt offset
 */
static u32_t irq_offset(u32_t x)
{
	u32_t index = x;

	index = (index - 1) & (~index);
	index = (index & 0x55555555) + ((index >> 1) & 0x55555555);
	index = (index & 0x33333333) + ((index >> 2) & 0x33333333);
	index = (index & 0x0f0f0f0f) + ((index >> 4) & 0x0f0f0f0f);
	index = (index & 0xff) + ((index & 0xff00) >> 8) + ((index & 0xff0000) >> 16) + ((index & 0xff000000) >> 24);

	return (index);
}

/*
 * do irqs.
 */
void do_irqs(struct regs * regs)
{
	u32_t vic0, vic1;
	u32_t offset;

	/* read vector interrupt controller's irq status */
	vic0 = readl(S3C6410_VIC0_IRQSTATUS);
	vic1 = readl(S3C6410_VIC1_IRQSTATUS);

	if(vic0 != 0)
	{
		/* get interrupt offset */
		offset = irq_offset(vic0);

		/* running interrupt server function */
		(s3c6410_irq_handler[offset])();

		/* clear software interrupt */
		writel(S3C6410_VIC0_SOFTINTCLEAR, 0x1<<offset);

		/* set vic address to zero */
		writel(S3C6410_VIC0_ADDRESS, 0x00000000);
	}
	else if(vic1 != 0)
	{
		/* get interrupt offset */
		offset = irq_offset(vic1);

		/* running interrupt server function */
		(s3c6410_irq_handler[offset + 32])();

		/* clear software interrupt */
		writel(S3C6410_VIC1_SOFTINTCLEAR, 0x1<<(offset-32));

		/* set all vic address to zero */
		writel(S3C6410_VIC1_ADDRESS, 0x00000000);
	}
	else
	{
		/* clear all software interrupts */
		writel(S3C6410_VIC0_SOFTINTCLEAR, 0xffffffff);
		writel(S3C6410_VIC1_SOFTINTCLEAR, 0xffffffff);

		/* set vic address to zero */
		writel(S3C6410_VIC0_ADDRESS, 0x00000000);
		writel(S3C6410_VIC1_ADDRESS, 0x00000000);
	}
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
			writel( S3C6410_VIC0_INTENABLE, (readl(S3C6410_VIC0_INTENABLE) | (0x1<<irq_no)) );
		else
			writel( S3C6410_VIC0_INTENCLEAR, (readl(S3C6410_VIC0_INTENCLEAR) | (0x1<<irq_no)) );
	}
	else if(irq_no < 64)
	{
		irq_no = irq_no - 32;

		if(enable)
			writel( S3C6410_VIC1_INTENABLE, (readl(S3C6410_VIC1_INTENABLE) | (0x1<<irq_no)) );
		else
			writel( S3C6410_VIC1_INTENCLEAR, (readl(S3C6410_VIC1_INTENCLEAR) | (0x1<<irq_no)) );
	}
	else
	{
		/* not yet support eint0-4 */
	}
}

/*
 * the array of irq.
 */
static struct irq s3c6410_irqs[] = {
	{
		.name		= "EINT0",
		.irq_no		= 0,
		.handler	= &s3c6410_irq_handler[0],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT1",
		.irq_no		= 1,
		.handler	= &s3c6410_irq_handler[1],
		.enable		= enable_irqs,
	}, {
		.name		= "RTC_TIC",
		.irq_no		= 2,
		.handler	= &s3c6410_irq_handler[2],
		.enable		= enable_irqs,
	}, {
		.name		= "CAMIF_C",
		.irq_no		= 3,
		.handler	= &s3c6410_irq_handler[3],
		.enable		= enable_irqs,
	}, {
		.name		= "CAMIF_P",
		.irq_no		= 4,
		.handler	= &s3c6410_irq_handler[4],
		.enable		= enable_irqs,
	}, {
		.name		= "I2C1",
		.irq_no		= 5,
		.handler	= &s3c6410_irq_handler[5],
		.enable		= enable_irqs,
	}, {
		.name		= "I2S",
		.irq_no		= 6,
		.handler	= &s3c6410_irq_handler[6],
		.enable		= enable_irqs,
	}, {
		.name		= "3D",
		.irq_no		= 8,
		.handler	= &s3c6410_irq_handler[8],
		.enable		= enable_irqs,
	}, {
		.name		= "POST0",
		.irq_no		= 9,
		.handler	= &s3c6410_irq_handler[9],
		.enable		= enable_irqs,
	}, {
		.name		= "ROTATOR",
		.irq_no		= 10,
		.handler	= &s3c6410_irq_handler[10],
		.enable		= enable_irqs,
	}, {
		.name		= "2D",
		.irq_no		= 11,
		.handler	= &s3c6410_irq_handler[11],
		.enable		= enable_irqs,
	}, {
		.name		= "TVENC",
		.irq_no		= 12,
		.handler	= &s3c6410_irq_handler[12],
		.enable		= enable_irqs,
	}, {
		.name		= "SCALER",
		.irq_no		= 13,
		.handler	= &s3c6410_irq_handler[13],
		.enable		= enable_irqs,
	}, {
		.name		= "BATF",
		.irq_no		= 14,
		.handler	= &s3c6410_irq_handler[14],
		.enable		= enable_irqs,
	}, {
		.name		= "JPEG",
		.irq_no		= 15,
		.handler	= &s3c6410_irq_handler[15],
		.enable		= enable_irqs,
	}, {
		.name		= "MFC",
		.irq_no		= 16,
		.handler	= &s3c6410_irq_handler[16],
		.enable		= enable_irqs,
	}, {
		.name		= "SDMA0",
		.irq_no		= 17,
		.handler	= &s3c6410_irq_handler[17],
		.enable		= enable_irqs,
	}, {
		.name		= "SDMA1",
		.irq_no		= 18,
		.handler	= &s3c6410_irq_handler[18],
		.enable		= enable_irqs,
	}, {
		.name		= "ARM_DMAERR",
		.irq_no		= 19,
		.handler	= &s3c6410_irq_handler[19],
		.enable		= enable_irqs,
	}, {
		.name		= "ARM_DMA",
		.irq_no		= 20,
		.handler	= &s3c6410_irq_handler[20],
		.enable		= enable_irqs,
	}, {
		.name		= "ARM_DMAS",
		.irq_no		= 21,
		.handler	= &s3c6410_irq_handler[21],
		.enable		= enable_irqs,
	}, {
		.name		= "KEYPAD",
		.irq_no		= 22,
		.handler	= &s3c6410_irq_handler[22],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER0",
		.irq_no		= 23,
		.handler	= &s3c6410_irq_handler[23],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER1",
		.irq_no		= 24,
		.handler	= &s3c6410_irq_handler[24],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER2",
		.irq_no		= 25,
		.handler	= &s3c6410_irq_handler[25],
		.enable		= enable_irqs,
	}, {
		.name		= "WDT",
		.irq_no		= 26,
		.handler	= &s3c6410_irq_handler[26],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER3",
		.irq_no		= 27,
		.handler	= &s3c6410_irq_handler[27],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER4",
		.irq_no		= 28,
		.handler	= &s3c6410_irq_handler[28],
		.enable		= enable_irqs,
	}, {
		.name		= "LCD0",
		.irq_no		= 29,
		.handler	= &s3c6410_irq_handler[29],
		.enable		= enable_irqs,
	}, {
		.name		= "LCD1",
		.irq_no		= 30,
		.handler	= &s3c6410_irq_handler[30],
		.enable		= enable_irqs,
	}, {
		.name		= "LCD2",
		.irq_no		= 31,
		.handler	= &s3c6410_irq_handler[31],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT2",
		.irq_no		= 32,
		.handler	= &s3c6410_irq_handler[32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT3",
		.irq_no		= 33,
		.handler	= &s3c6410_irq_handler[33],
		.enable		= enable_irqs,
	}, {
		.name		= "PCM0",
		.irq_no		= 34,
		.handler	= &s3c6410_irq_handler[34],
		.enable		= enable_irqs,
	}, {
		.name		= "PCM1",
		.irq_no		= 35,
		.handler	= &s3c6410_irq_handler[35],
		.enable		= enable_irqs,
	}, {
		.name		= "AC97",
		.irq_no		= 36,
		.handler	= &s3c6410_irq_handler[36],
		.enable		= enable_irqs,
	}, {
		.name		= "UART0",
		.irq_no		= 37,
		.handler	= &s3c6410_irq_handler[37],
		.enable		= enable_irqs,
	}, {
		.name		= "UART1",
		.irq_no		= 38,
		.handler	= &s3c6410_irq_handler[38],
		.enable		= enable_irqs,
	}, {
		.name		= "UART2",
		.irq_no		= 39,
		.handler	= &s3c6410_irq_handler[39],
		.enable		= enable_irqs,
	}, {
		.name		= "UART3",
		.irq_no		= 40,
		.handler	= &s3c6410_irq_handler[40],
		.enable		= enable_irqs,
	}, {
		.name		= "DMA0",
		.irq_no		= 41,
		.handler	= &s3c6410_irq_handler[41],
		.enable		= enable_irqs,
	}, {
		.name		= "DMA1",
		.irq_no		= 42,
		.handler	= &s3c6410_irq_handler[42],
		.enable		= enable_irqs,
	}, {
		.name		= "ONENAND0",
		.irq_no		= 43,
		.handler	= &s3c6410_irq_handler[43],
		.enable		= enable_irqs,
	}, {
		.name		= "ONENAND1",
		.irq_no		= 44,
		.handler	= &s3c6410_irq_handler[44],
		.enable		= enable_irqs,
	}, {
		.name		= "NFC",
		.irq_no		= 45,
		.handler	= &s3c6410_irq_handler[45],
		.enable		= enable_irqs,
	}, {
		.name		= "CFC",
		.irq_no		= 46,
		.handler	= &s3c6410_irq_handler[46],
		.enable		= enable_irqs,
	}, {
		.name		= "UHOST",
		.irq_no		= 47,
		.handler	= &s3c6410_irq_handler[47],
		.enable		= enable_irqs,
	}, {
		.name		= "SPI0",
		.irq_no		= 48,
		.handler	= &s3c6410_irq_handler[48],
		.enable		= enable_irqs,
	}, {
		.name		= "SPI1",
		.irq_no		= 49,
		.handler	= &s3c6410_irq_handler[49],
		.enable		= enable_irqs,
	}, {
		.name		= "IIC0",
		.irq_no		= 50,
		.handler	= &s3c6410_irq_handler[50],
		.enable		= enable_irqs,
	}, {
		.name		= "HSITX",
		.irq_no		= 51,
		.handler	= &s3c6410_irq_handler[51],
		.enable		= enable_irqs,
	}, {
		.name		= "HSIRX",
		.irq_no		= 52,
		.handler	= &s3c6410_irq_handler[52],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT4",
		.irq_no		= 53,
		.handler	= &s3c6410_irq_handler[53],
		.enable		= enable_irqs,
	}, {
		.name		= "MSM",
		.irq_no		= 54,
		.handler	= &s3c6410_irq_handler[54],
		.enable		= enable_irqs,
	}, {
		.name		= "HOSTIF",
		.irq_no		= 55,
		.handler	= &s3c6410_irq_handler[55],
		.enable		= enable_irqs,
	}, {
		.name		= "HSMMC0",
		.irq_no		= 56,
		.handler	= &s3c6410_irq_handler[56],
		.enable		= enable_irqs,
	}, {
		.name		= "HSMMC1",
		.irq_no		= 57,
		.handler	= &s3c6410_irq_handler[57],
		.enable		= enable_irqs,
	}, {
		.name		= "OTG",
		.irq_no		= 58,
		.handler	= &s3c6410_irq_handler[58],
		.enable		= enable_irqs,
	}, {
		.name		= "IRDA",
		.irq_no		= 59,
		.handler	= &s3c6410_irq_handler[59],
		.enable		= enable_irqs,
	}, {
		.name		= "RTC_ALARM",
		.irq_no		= 60,
		.handler	= &s3c6410_irq_handler[60],
		.enable		= enable_irqs,
	}, {
		.name		= "SEC",
		.irq_no		= 61,
		.handler	= &s3c6410_irq_handler[61],
		.enable		= enable_irqs,
	}, {
		.name		= "PENDNUP",
		.irq_no		= 62,
		.handler	= &s3c6410_irq_handler[62],
		.enable		= enable_irqs,
	}, {
		.name		= "ADC",
		.irq_no		= 63,
		.handler	= &s3c6410_irq_handler[63],
		.enable		= enable_irqs,
	}
};

static __init void s3c6410_irq_init(void)
{
	u32_t i;

	/* disable all interrupts */
	writel(S3C6410_VIC0_INTENCLEAR, 0xffffffff);
	writel(S3C6410_VIC1_INTENCLEAR, 0xffffffff);

	/* clear all software interrupts */
	writel(S3C6410_VIC0_SOFTINTCLEAR, 0xffffffff);
	writel(S3C6410_VIC1_SOFTINTCLEAR, 0xffffffff);

	/* select irq mode */
	writel(S3C6410_VIC0_INTSELECT, 0x00000000);
	writel(S3C6410_VIC1_INTSELECT, 0x00000000);

	/* set vic address to zero */
	writel(S3C6410_VIC0_ADDRESS, 0x00000000);
	writel(S3C6410_VIC1_ADDRESS, 0x00000000);

	for(i = 0; i< 32; i++)
	{
		writel((S3C6410_VIC0_VECTADDR0 + 4 * i), (u32_t)irq);
		writel((S3C6410_VIC1_VECTADDR0 + 4 * i), (u32_t)irq);
	}

	for(i = 0; i< 64; i++)
	{
		s3c6410_irq_handler[i] = (irq_handler)null_irq_handler;
	}

	for(i = 0; i < ARRAY_SIZE(s3c6410_irqs); i++)
	{
		if(!irq_register(&s3c6410_irqs[i]))
		{
			LOG_E("failed to register irq '%s'", s3c6410_irqs[i].name);
		}
	}

	/* enable vector interrupt controller */
	vic_enable();

	/* enable irq and fiq */
	irq_enable();
	fiq_enable();
}

static __exit void s3c6410_irq_exit(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(s3c6410_irqs); i++)
	{
		if(!irq_unregister(&s3c6410_irqs[i]))
		{
			LOG_E("failed to unregister irq '%s'", s3c6410_irqs[i].name);
		}
	}

	/* disable vector interrupt controller */
	vic_disable();

	/* disable irq and fiq */
	irq_disable();
	fiq_disable();
}

core_initcall(s3c6410_irq_init);
core_exitcall(s3c6410_irq_exit);
