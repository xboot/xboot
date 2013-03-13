/*
 * s5pv210-irq.c
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
#include <s5pv210/reg-gpio.h>
#include <s5pv210/reg-vic.h>
#include <s5pv210-cp15.h>

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
static irq_handler s5pv210_irq_handler[128];

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
	u32_t vic0, vic1, vic2, vic3;
	u32_t offset;

	/* read vector interrupt controller's irq status */
	vic0 = readl(S5PV210_VIC0_IRQSTATUS);
	vic1 = readl(S5PV210_VIC1_IRQSTATUS);
	vic2 = readl(S5PV210_VIC2_IRQSTATUS);
	vic3 = readl(S5PV210_VIC3_IRQSTATUS);

	if(vic0 != 0)
	{
		/* get interrupt offset */
		offset = irq_offset(vic0);

		/* running interrupt server function */
		(s5pv210_irq_handler[offset])();

		/* clear software interrupt */
		writel(S5PV210_VIC0_SOFTINTCLEAR, 0x1<<offset);

		/* set vic address to zero */
		writel(S5PV210_VIC0_ADDRESS, 0x00000000);
	}
	else if(vic1 != 0)
	{
		/* get interrupt offset */
		offset = irq_offset(vic1);

		/* running interrupt server function */
		(s5pv210_irq_handler[offset + 32])();

		/* clear software interrupt */
		writel(S5PV210_VIC1_SOFTINTCLEAR, 0x1<<(offset-32));

		/* set all vic address to zero */
		writel(S5PV210_VIC1_ADDRESS, 0x00000000);
	}
	else if(vic2 != 0)
	{
		/* get interrupt offset */
		offset = irq_offset(vic2);

		/* running interrupt server function */
		(s5pv210_irq_handler[offset + 64])();

		/* clear software interrupt */
		writel(S5PV210_VIC2_SOFTINTCLEAR, 0x1<<(offset-32));

		/* set all vic address to zero */
		writel(S5PV210_VIC2_ADDRESS, 0x00000000);
	}
	else if(vic3 != 0)
	{
		/* get interrupt offset */
		offset = irq_offset(vic3);

		/* running interrupt server function */
		(s5pv210_irq_handler[offset + 96])();

		/* clear software interrupt */
		writel(S5PV210_VIC3_SOFTINTCLEAR, 0x1<<(offset-32));

		/* set all vic address to zero */
		writel(S5PV210_VIC3_ADDRESS, 0x00000000);
	}
	else
	{
		/* clear all software interrupts */
		writel(S5PV210_VIC0_SOFTINTCLEAR, 0xffffffff);
		writel(S5PV210_VIC1_SOFTINTCLEAR, 0xffffffff);
		writel(S5PV210_VIC2_SOFTINTCLEAR, 0xffffffff);
		writel(S5PV210_VIC3_SOFTINTCLEAR, 0xffffffff);

		/* set vic address to zero */
		writel(S5PV210_VIC0_ADDRESS, 0x00000000);
		writel(S5PV210_VIC1_ADDRESS, 0x00000000);
		writel(S5PV210_VIC2_ADDRESS, 0x00000000);
		writel(S5PV210_VIC3_ADDRESS, 0x00000000);
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
			writel( S5PV210_VIC0_INTENABLE, (readl(S5PV210_VIC0_INTENABLE) | (0x1<<irq_no)) );
		else
			writel( S5PV210_VIC0_INTENCLEAR, (readl(S5PV210_VIC0_INTENCLEAR) | (0x1<<irq_no)) );
	}
	else if(irq_no < 64)
	{
		irq_no = irq_no - 32;

		if(enable)
			writel( S5PV210_VIC1_INTENABLE, (readl(S5PV210_VIC1_INTENABLE) | (0x1<<irq_no)) );
		else
			writel( S5PV210_VIC1_INTENCLEAR, (readl(S5PV210_VIC1_INTENCLEAR) | (0x1<<irq_no)) );
	}
	else if(irq_no < 96)
	{
		irq_no = irq_no - 64;

		if(enable)
			writel( S5PV210_VIC2_INTENABLE, (readl(S5PV210_VIC2_INTENABLE) | (0x1<<irq_no)) );
		else
			writel( S5PV210_VIC2_INTENCLEAR, (readl(S5PV210_VIC2_INTENCLEAR) | (0x1<<irq_no)) );
	}
	else if(irq_no < 128)
	{
		irq_no = irq_no - 96;

		if(enable)
			writel( S5PV210_VIC3_INTENABLE, (readl(S5PV210_VIC3_INTENABLE) | (0x1<<irq_no)) );
		else
			writel( S5PV210_VIC3_INTENCLEAR, (readl(S5PV210_VIC3_INTENCLEAR) | (0x1<<irq_no)) );
	}
	else
	{
		/* not yet support others */
	}
}

/*
 * the array of irq.
 */
static struct irq s5pv210_irqs[] = {
	{
		.name		= "EINT0",
		.irq_no		= 0,
		.handler	= &s5pv210_irq_handler[0],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT1",
		.irq_no		= 1,
		.handler	= &s5pv210_irq_handler[1],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT2",
		.irq_no		= 2,
		.handler	= &s5pv210_irq_handler[2],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT3",
		.irq_no		= 3,
		.handler	= &s5pv210_irq_handler[3],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT4",
		.irq_no		= 4,
		.handler	= &s5pv210_irq_handler[4],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT5",
		.irq_no		= 5,
		.handler	= &s5pv210_irq_handler[5],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT6",
		.irq_no		= 6,
		.handler	= &s5pv210_irq_handler[6],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT7",
		.irq_no		= 7,
		.handler	= &s5pv210_irq_handler[7],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT8",
		.irq_no		= 8,
		.handler	= &s5pv210_irq_handler[8],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT9",
		.irq_no		= 9,
		.handler	= &s5pv210_irq_handler[9],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT10",
		.irq_no		= 10,
		.handler	= &s5pv210_irq_handler[10],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT11",
		.irq_no		= 11,
		.handler	= &s5pv210_irq_handler[11],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT12",
		.irq_no		= 12,
		.handler	= &s5pv210_irq_handler[12],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT13",
		.irq_no		= 13,
		.handler	= &s5pv210_irq_handler[13],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT14",
		.irq_no		= 14,
		.handler	= &s5pv210_irq_handler[14],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT15",
		.irq_no		= 15,
		.handler	= &s5pv210_irq_handler[15],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT16_31",
		.irq_no		= 16,
		.handler	= &s5pv210_irq_handler[16],
		.enable		= enable_irqs,
	}, {
		.name		= "MDMA",
		.irq_no		= 18,
		.handler	= &s5pv210_irq_handler[18],
		.enable		= enable_irqs,
	}, {
		.name		= "PDMA0",
		.irq_no		= 19,
		.handler	= &s5pv210_irq_handler[19],
		.enable		= enable_irqs,
	}, {
		.name		= "PDMA1",
		.irq_no		= 20,
		.handler	= &s5pv210_irq_handler[20],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER0",
		.irq_no		= 21,
		.handler	= &s5pv210_irq_handler[21],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER1",
		.irq_no		= 22,
		.handler	= &s5pv210_irq_handler[22],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER2",
		.irq_no		= 23,
		.handler	= &s5pv210_irq_handler[23],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER3",
		.irq_no		= 24,
		.handler	= &s5pv210_irq_handler[24],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER4",
		.irq_no		= 25,
		.handler	= &s5pv210_irq_handler[25],
		.enable		= enable_irqs,
	}, {
		.name		= "SYSTEM_TMIER",
		.irq_no		= 26,
		.handler	= &s5pv210_irq_handler[26],
		.enable		= enable_irqs,
	}, {
		.name		= "WDT",
		.irq_no		= 27,
		.handler	= &s5pv210_irq_handler[27],
		.enable		= enable_irqs,
	}, {
		.name		= "RTC_ALARM",
		.irq_no		= 28,
		.handler	= &s5pv210_irq_handler[28],
		.enable		= enable_irqs,
	}, {
		.name		= "RTC_TIC",
		.irq_no		= 29,
		.handler	= &s5pv210_irq_handler[29],
		.enable		= enable_irqs,
	}, {
		.name		= "GPIOINT",
		.irq_no		= 30,
		.handler	= &s5pv210_irq_handler[30],
		.enable		= enable_irqs,
	}, {
		.name		= "FIMC3",
		.irq_no		= 31,
		.handler	= &s5pv210_irq_handler[31],
		.enable		= enable_irqs,
	}, {
		.name		= "CORTEX0",
		.irq_no		= 32,
		.handler	= &s5pv210_irq_handler[32],
		.enable		= enable_irqs,
	}, {
		.name		= "CORTEX1",
		.irq_no		= 33,
		.handler	= &s5pv210_irq_handler[33],
		.enable		= enable_irqs,
	}, {
		.name		= "CORTEX2",
		.irq_no		= 34,
		.handler	= &s5pv210_irq_handler[34],
		.enable		= enable_irqs,
	}, {
		.name		= "CORTEX3",
		.irq_no		= 35,
		.handler	= &s5pv210_irq_handler[35],
		.enable		= enable_irqs,
	}, {
		.name		= "CORTEX4",
		.irq_no		= 36,
		.handler	= &s5pv210_irq_handler[36],
		.enable		= enable_irqs,
	}, {
		.name		= "IEM_APC",
		.irq_no		= 37,
		.handler	= &s5pv210_irq_handler[37],
		.enable		= enable_irqs,
	}, {
		.name		= "IEM_IEC",
		.irq_no		= 38,
		.handler	= &s5pv210_irq_handler[38],
		.enable		= enable_irqs,
	}, {
		.name		= "NFC",
		.irq_no		= 40,
		.handler	= &s5pv210_irq_handler[40],
		.enable		= enable_irqs,
	}, {
		.name		= "CFC",
		.irq_no		= 41,
		.handler	= &s5pv210_irq_handler[41],
		.enable		= enable_irqs,
	}, {
		.name		= "UART0",
		.irq_no		= 42,
		.handler	= &s5pv210_irq_handler[42],
		.enable		= enable_irqs,
	}, {
		.name		= "UART1",
		.irq_no		= 43,
		.handler	= &s5pv210_irq_handler[43],
		.enable		= enable_irqs,
	}, {
		.name		= "UART2",
		.irq_no		= 44,
		.handler	= &s5pv210_irq_handler[44],
		.enable		= enable_irqs,
	}, {
		.name		= "UART3",
		.irq_no		= 45,
		.handler	= &s5pv210_irq_handler[45],
		.enable		= enable_irqs,
	}, {
		.name		= "I2C0",
		.irq_no		= 46,
		.handler	= &s5pv210_irq_handler[46],
		.enable		= enable_irqs,
	}, {
		.name		= "SPI0",
		.irq_no		= 47,
		.handler	= &s5pv210_irq_handler[47],
		.enable		= enable_irqs,
	}, {
		.name		= "SPI1",
		.irq_no		= 48,
		.handler	= &s5pv210_irq_handler[48],
		.enable		= enable_irqs,
	}, {
		.name		= "AUDIO_SS",
		.irq_no		= 50,
		.handler	= &s5pv210_irq_handler[50],
		.enable		= enable_irqs,
	}, {
		.name		= "I2C2",
		.irq_no		= 51,
		.handler	= &s5pv210_irq_handler[51],
		.enable		= enable_irqs,
	}, {
		.name		= "I2C_HDMI_PHY",
		.irq_no		= 52,
		.handler	= &s5pv210_irq_handler[52],
		.enable		= enable_irqs,
	}, {
		.name		= "UHOST",
		.irq_no		= 55,
		.handler	= &s5pv210_irq_handler[55],
		.enable		= enable_irqs,
	}, {
		.name		= "OTG",
		.irq_no		= 56,
		.handler	= &s5pv210_irq_handler[56],
		.enable		= enable_irqs,
	}, {
		.name		= "MODEMIF",
		.irq_no		= 57,
		.handler	= &s5pv210_irq_handler[57],
		.enable		= enable_irqs,
	}, {
		.name		= "HSMMC0",
		.irq_no		= 58,
		.handler	= &s5pv210_irq_handler[58],
		.enable		= enable_irqs,
	}, {
		.name		= "HSMMC1",
		.irq_no		= 59,
		.handler	= &s5pv210_irq_handler[59],
		.enable		= enable_irqs,
	}, {
		.name		= "HSMMC2",
		.irq_no		= 60,
		.handler	= &s5pv210_irq_handler[60],
		.enable		= enable_irqs,
	}, {
		.name		= "MIPI_CSI",
		.irq_no		= 61,
		.handler	= &s5pv210_irq_handler[61],
		.enable		= enable_irqs,
	}, {
		.name		= "MIPI_DSI",
		.irq_no		= 62,
		.handler	= &s5pv210_irq_handler[62],
		.enable		= enable_irqs,
	}, {
		.name		= "ONENAND_AUDI",
		.irq_no		= 63,
		.handler	= &s5pv210_irq_handler[63],
		.enable		= enable_irqs,
	}, {
		.name		= "LCD0",
		.irq_no		= 64,
		.handler	= &s5pv210_irq_handler[64],
		.enable		= enable_irqs,
	}, {
		.name		= "LCD1",
		.irq_no		= 65,
		.handler	= &s5pv210_irq_handler[65],
		.enable		= enable_irqs,
	}, {
		.name		= "LCD2",
		.irq_no		= 66,
		.handler	= &s5pv210_irq_handler[66],
		.enable		= enable_irqs,
	}, {
		.name		= "ROTATOR",
		.irq_no		= 68,
		.handler	= &s5pv210_irq_handler[68],
		.enable		= enable_irqs,
	}, {
		.name		= "FIMC0",
		.irq_no		= 69,
		.handler	= &s5pv210_irq_handler[69],
		.enable		= enable_irqs,
	}, {
		.name		= "FIMC1",
		.irq_no		= 70,
		.handler	= &s5pv210_irq_handler[70],
		.enable		= enable_irqs,
	}, {
		.name		= "FIMC2",
		.irq_no		= 71,
		.handler	= &s5pv210_irq_handler[71],
		.enable		= enable_irqs,
	}, {
		.name		= "JPEG",
		.irq_no		= 72,
		.handler	= &s5pv210_irq_handler[72],
		.enable		= enable_irqs,
	}, {
		.name		= "2D",
		.irq_no		= 73,
		.handler	= &s5pv210_irq_handler[73],
		.enable		= enable_irqs,
	}, {
		.name		= "3D",
		.irq_no		= 74,
		.handler	= &s5pv210_irq_handler[74],
		.enable		= enable_irqs,
	}, {
		.name		= "MIXER",
		.irq_no		= 75,
		.handler	= &s5pv210_irq_handler[75],
		.enable		= enable_irqs,
	}, {
		.name		= "HDMI",
		.irq_no		= 76,
		.handler	= &s5pv210_irq_handler[76],
		.enable		= enable_irqs,
	}, {
		.name		= "I2C_HDMI_DDC",
		.irq_no		= 77,
		.handler	= &s5pv210_irq_handler[77],
		.enable		= enable_irqs,
	}, {
		.name		= "MFC",
		.irq_no		= 78,
		.handler	= &s5pv210_irq_handler[78],
		.enable		= enable_irqs,
	}, {
		.name		= "TVENC",
		.irq_no		= 79,
		.handler	= &s5pv210_irq_handler[79],
		.enable		= enable_irqs,
	}, {
		.name		= "I2S0",
		.irq_no		= 80,
		.handler	= &s5pv210_irq_handler[80],
		.enable		= enable_irqs,
	}, {
		.name		= "I2S1",
		.irq_no		= 81,
		.handler	= &s5pv210_irq_handler[81],
		.enable		= enable_irqs,
	}, {
		.name		= "AC97",
		.irq_no		= 83,
		.handler	= &s5pv210_irq_handler[83],
		.enable		= enable_irqs,
	}, {
		.name		= "PCM0",
		.irq_no		= 84,
		.handler	= &s5pv210_irq_handler[84],
		.enable		= enable_irqs,
	}, {
		.name		= "PCM1",
		.irq_no		= 85,
		.handler	= &s5pv210_irq_handler[85],
		.enable		= enable_irqs,
	}, {
		.name		= "SPDIF",
		.irq_no		= 86,
		.handler	= &s5pv210_irq_handler[86],
		.enable		= enable_irqs,
	}, {
		.name		= "ADC",
		.irq_no		= 87,
		.handler	= &s5pv210_irq_handler[87],
		.enable		= enable_irqs,
	}, {
		.name		= "PENDN",
		.irq_no		= 88,
		.handler	= &s5pv210_irq_handler[88],
		.enable		= enable_irqs,
	}, {
		.name		= "KEYPAD",
		.irq_no		= 89,
		.handler	= &s5pv210_irq_handler[89],
		.enable		= enable_irqs,
	}, {
		.name		= "INTHASH_SSS",
		.irq_no		= 91,
		.handler	= &s5pv210_irq_handler[91],
		.enable		= enable_irqs,
	}, {
		.name		= "INTFEEDCTRL_SSS",
		.irq_no		= 92,
		.handler	= &s5pv210_irq_handler[92],
		.enable		= enable_irqs,
	}, {
		.name		= "PCM2",
		.irq_no		= 93,
		.handler	= &s5pv210_irq_handler[93],
		.enable		= enable_irqs,
	}, {
		.name		= "SDM_IRQ",
		.irq_no		= 94,
		.handler	= &s5pv210_irq_handler[94],
		.enable		= enable_irqs,
	}, {
		.name		= "SDM_FIQ",
		.irq_no		= 95,
		.handler	= &s5pv210_irq_handler[95],
		.enable		= enable_irqs,
	}, {
		.name		= "MMC3",
		.irq_no		= 98,
		.handler	= &s5pv210_irq_handler[98],
		.enable		= enable_irqs,
	}, {
		.name		= "CEC",
		.irq_no		= 99,
		.handler	= &s5pv210_irq_handler[99],
		.enable		= enable_irqs,
	}, {
		.name		= "TSI",
		.irq_no		= 100,
		.handler	= &s5pv210_irq_handler[100],
		.enable		= enable_irqs,
	}, {
		.name		= "ADC1",
		.irq_no		= 105,
		.handler	= &s5pv210_irq_handler[105],
		.enable		= enable_irqs,
	}, {
		.name		= "PENDN1",
		.irq_no		= 106,
		.handler	= &s5pv210_irq_handler[106],
		.enable		= enable_irqs,
	}
};

static __init void s5pv210_irq_init(void)
{
	u32_t i;

	/* select irq mode */
	writel(S5PV210_VIC0_INTSELECT, 0x00000000);
	writel(S5PV210_VIC1_INTSELECT, 0x00000000);
	writel(S5PV210_VIC2_INTSELECT, 0x00000000);
	writel(S5PV210_VIC3_INTSELECT, 0x00000000);

	/* disable all interrupts */
	writel(S5PV210_VIC0_INTENABLE, 0x00000000);
	writel(S5PV210_VIC1_INTENABLE, 0x00000000);
	writel(S5PV210_VIC2_INTENABLE, 0x00000000);
	writel(S5PV210_VIC3_INTENABLE, 0x00000000);

	/* clear all interrupts */
	writel(S5PV210_VIC0_INTENCLEAR, 0xffffffff);
	writel(S5PV210_VIC1_INTENCLEAR, 0xffffffff);
	writel(S5PV210_VIC2_INTENCLEAR, 0xffffffff);
	writel(S5PV210_VIC3_INTENCLEAR, 0xffffffff);

	/* clear all irq status */
	writel(S5PV210_VIC0_IRQSTATUS, 0x00000000);
	writel(S5PV210_VIC1_IRQSTATUS, 0x00000000);
	writel(S5PV210_VIC2_IRQSTATUS, 0x00000000);
	writel(S5PV210_VIC3_IRQSTATUS, 0x00000000);

	/* clear all fiq status */
	writel(S5PV210_VIC0_FIQSTATUS, 0x00000000);
	writel(S5PV210_VIC1_FIQSTATUS, 0x00000000);
	writel(S5PV210_VIC2_FIQSTATUS, 0x00000000);
	writel(S5PV210_VIC3_FIQSTATUS, 0x00000000);

	/* clear all software interrupts */
	writel(S5PV210_VIC0_SOFTINTCLEAR, 0xffffffff);
	writel(S5PV210_VIC1_SOFTINTCLEAR, 0xffffffff);
	writel(S5PV210_VIC2_SOFTINTCLEAR, 0xffffffff);
	writel(S5PV210_VIC3_SOFTINTCLEAR, 0xffffffff);

	/* set vic address to zero */
	writel(S5PV210_VIC0_ADDRESS, 0x00000000);
	writel(S5PV210_VIC1_ADDRESS, 0x00000000);
	writel(S5PV210_VIC2_ADDRESS, 0x00000000);
	writel(S5PV210_VIC3_ADDRESS, 0x00000000);

	for(i = 0; i< 32; i++)
	{
		writel((S5PV210_VIC0_VECTADDR0 + 4 * i), (u32_t)irq);
		writel((S5PV210_VIC1_VECTADDR0 + 4 * i), (u32_t)irq);
		writel((S5PV210_VIC2_VECTADDR0 + 4 * i), (u32_t)irq);
		writel((S5PV210_VIC3_VECTADDR0 + 4 * i), (u32_t)irq);
	}

	for(i = 0; i< 32; i++)
	{
		writel((S5PV210_VIC0_VECPRIORITY0 + 4 * i), 0xf);
		writel((S5PV210_VIC1_VECPRIORITY0 + 4 * i), 0xf);
		writel((S5PV210_VIC2_VECPRIORITY0 + 4 * i), 0xf);
		writel((S5PV210_VIC3_VECPRIORITY0 + 4 * i), 0xf);
	}

	for(i = 0; i < ARRAY_SIZE(s5pv210_irq_handler); i++)
	{
		s5pv210_irq_handler[i] = (irq_handler)null_irq_handler;
	}

	for(i = 0; i < ARRAY_SIZE(s5pv210_irqs); i++)
	{
		if(!irq_register(&s5pv210_irqs[i]))
		{
			LOG_E("failed to register irq '%s'", s5pv210_irqs[i].name);
		}
	}

	/* enable vector interrupt controller */
	vic_enable();

	/* enable irq and fiq */
	irq_enable();
	fiq_enable();
}

static __exit void s5pv210_irq_exit(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(s5pv210_irqs); i++)
	{
		if(!irq_unregister(&s5pv210_irqs[i]))
		{
			LOG_E("failed to unregister irq '%s'", s5pv210_irqs[i].name);
		}
	}

	/* disable vector interrupt controller */
	vic_disable();

	/* disable irq and fiq */
	irq_disable();
	fiq_disable();
}

core_initcall(s5pv210_irq_init);
core_exitcall(s5pv210_irq_exit);
