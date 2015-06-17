/*
 * s5pv210-irq.c
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
#include <s5pv210/reg-gpio.h>
#include <s5pv210/reg-vic.h>

/*
 * Exception handlers for irq from Start.s
 */
extern void irq(void);

struct pt_regs_t {
	u32_t	r0,		r1,		r2,		r3, 	r4,		r5;
	u32_t	r6,		r7,		r8, 	r9, 	r10,	fp;
	u32_t	ip, 	sp, 	lr, 	pc,		cpsr, 	orig_r0;
};

static struct irq_handler_t s5pv210_irq_handler[128];

void do_irqs(struct pt_regs_t * regs)
{
	u32_t vic0, vic1, vic2, vic3;
	u32_t offset;

	/* Read vector interrupt controller's irq status */
	vic0 = read32(S5PV210_VIC0_IRQSTATUS);
	vic1 = read32(S5PV210_VIC1_IRQSTATUS);
	vic2 = read32(S5PV210_VIC2_IRQSTATUS);
	vic3 = read32(S5PV210_VIC3_IRQSTATUS);

	if(vic0 != 0)
	{
		/* Get interrupt offset */
		offset = __ffs(vic0);

		/* Handle interrupt server function */
		(s5pv210_irq_handler[offset].func)(s5pv210_irq_handler[offset].data);

		/* Clear software interrupt */
		write32(S5PV210_VIC0_SOFTINTCLEAR, 0x1<<offset);

		/* Set vic address to zero */
		write32(S5PV210_VIC0_ADDRESS, 0x00000000);
	}
	else if(vic1 != 0)
	{
		/* Get interrupt offset */
		offset = __ffs(vic1);

		/* Handle interrupt server function */
		(s5pv210_irq_handler[offset + 32].func)(s5pv210_irq_handler[offset + 32].data);

		/* Clear software interrupt */
		write32(S5PV210_VIC1_SOFTINTCLEAR, 0x1<<(offset-32));

		/* Set all vic address to zero */
		write32(S5PV210_VIC1_ADDRESS, 0x00000000);
	}
	else if(vic2 != 0)
	{
		/* Get interrupt offset */
		offset = __ffs(vic2);

		/* Handle interrupt server function */
		(s5pv210_irq_handler[offset + 64].func)(s5pv210_irq_handler[offset + 64].data);

		/* Clear software interrupt */
		write32(S5PV210_VIC2_SOFTINTCLEAR, 0x1<<(offset-32));

		/* Set all vic address to zero */
		write32(S5PV210_VIC2_ADDRESS, 0x00000000);
	}
	else if(vic3 != 0)
	{
		/* Get interrupt offset */
		offset = __ffs(vic3);

		/* Handle interrupt server function */
		(s5pv210_irq_handler[offset + 96].func)(s5pv210_irq_handler[offset + 96].data);

		/* Clear software interrupt */
		write32(S5PV210_VIC3_SOFTINTCLEAR, 0x1<<(offset-32));

		/* Set all vic address to zero */
		write32(S5PV210_VIC3_ADDRESS, 0x00000000);
	}
	else
	{
		/* Clear all software interrupts */
		write32(S5PV210_VIC0_SOFTINTCLEAR, 0xffffffff);
		write32(S5PV210_VIC1_SOFTINTCLEAR, 0xffffffff);
		write32(S5PV210_VIC2_SOFTINTCLEAR, 0xffffffff);
		write32(S5PV210_VIC3_SOFTINTCLEAR, 0xffffffff);

		/* Set vic address to zero */
		write32(S5PV210_VIC0_ADDRESS, 0x00000000);
		write32(S5PV210_VIC1_ADDRESS, 0x00000000);
		write32(S5PV210_VIC2_ADDRESS, 0x00000000);
		write32(S5PV210_VIC3_ADDRESS, 0x00000000);
	}
}

static void s5pv210_irq_enable(struct irq_t * irq)
{
	u32_t no = irq->no;

	if(no < 32)
	{
		write32( S5PV210_VIC0_INTENABLE, (read32(S5PV210_VIC0_INTENABLE) | (0x1 << no)) );
	}
	else if(no < 64)
	{
		no = no - 32;
		write32( S5PV210_VIC1_INTENABLE, (read32(S5PV210_VIC1_INTENABLE) | (0x1 << no)) );
	}
	else if(no < 96)
	{
		no = no - 64;
		write32( S5PV210_VIC2_INTENABLE, (read32(S5PV210_VIC2_INTENABLE) | (0x1 << no)) );
	}
	else if(no < 128)
	{
		no = no - 96;
		write32( S5PV210_VIC3_INTENABLE, (read32(S5PV210_VIC3_INTENABLE) | (0x1 << no)) );
	}
}

static void s5pv210_irq_disable(struct irq_t * irq)
{
	u32_t no = irq->no;

	if(no < 32)
	{
		write32( S5PV210_VIC0_INTENCLEAR, (read32(S5PV210_VIC0_INTENCLEAR) | (0x1 << no)) );
	}
	else if(no < 64)
	{
		no = no - 32;
		write32( S5PV210_VIC1_INTENCLEAR, (read32(S5PV210_VIC1_INTENCLEAR) | (0x1 << no)) );
	}
	else if(no < 96)
	{
		no = no - 64;
		write32( S5PV210_VIC2_INTENCLEAR, (read32(S5PV210_VIC2_INTENCLEAR) | (0x1 << no)) );
	}
	else if(no < 128)
	{
		no = no - 96;
		write32( S5PV210_VIC3_INTENCLEAR, (read32(S5PV210_VIC3_INTENCLEAR) | (0x1 << no)) );
	}
}

static void s5pv210_irq_set_type(struct irq_t * irq, enum irq_type_t type)
{
}

static struct irq_t s5pv210_irqs[] = {
	{
		.name		= "EINT0",
		.no			= 0,
		.handler	= &s5pv210_irq_handler[0],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "EINT1",
		.no			= 1,
		.handler	= &s5pv210_irq_handler[1],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "EINT2",
		.no			= 2,
		.handler	= &s5pv210_irq_handler[2],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "EINT3",
		.no			= 3,
		.handler	= &s5pv210_irq_handler[3],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "EINT4",
		.no			= 4,
		.handler	= &s5pv210_irq_handler[4],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "EINT5",
		.no			= 5,
		.handler	= &s5pv210_irq_handler[5],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "EINT6",
		.no			= 6,
		.handler	= &s5pv210_irq_handler[6],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "EINT7",
		.no			= 7,
		.handler	= &s5pv210_irq_handler[7],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "EINT8",
		.no			= 8,
		.handler	= &s5pv210_irq_handler[8],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "EINT9",
		.no			= 9,
		.handler	= &s5pv210_irq_handler[9],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "EINT10",
		.no			= 10,
		.handler	= &s5pv210_irq_handler[10],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "EINT11",
		.no			= 11,
		.handler	= &s5pv210_irq_handler[11],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "EINT12",
		.no			= 12,
		.handler	= &s5pv210_irq_handler[12],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "EINT13",
		.no			= 13,
		.handler	= &s5pv210_irq_handler[13],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "EINT14",
		.no			= 14,
		.handler	= &s5pv210_irq_handler[14],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "EINT15",
		.no			= 15,
		.handler	= &s5pv210_irq_handler[15],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "EINT16_31",
		.no			= 16,
		.handler	= &s5pv210_irq_handler[16],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "MDMA",
		.no			= 18,
		.handler	= &s5pv210_irq_handler[18],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "PDMA0",
		.no			= 19,
		.handler	= &s5pv210_irq_handler[19],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "PDMA1",
		.no			= 20,
		.handler	= &s5pv210_irq_handler[20],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "TIMER0",
		.no			= 21,
		.handler	= &s5pv210_irq_handler[21],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "TIMER1",
		.no			= 22,
		.handler	= &s5pv210_irq_handler[22],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "TIMER2",
		.no			= 23,
		.handler	= &s5pv210_irq_handler[23],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "TIMER3",
		.no			= 24,
		.handler	= &s5pv210_irq_handler[24],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "TIMER4",
		.no			= 25,
		.handler	= &s5pv210_irq_handler[25],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "SYSTEM_TMIER",
		.no			= 26,
		.handler	= &s5pv210_irq_handler[26],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "WDT",
		.no			= 27,
		.handler	= &s5pv210_irq_handler[27],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "RTC_ALARM",
		.no			= 28,
		.handler	= &s5pv210_irq_handler[28],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "RTC_TIC",
		.no			= 29,
		.handler	= &s5pv210_irq_handler[29],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "GPIOINT",
		.no			= 30,
		.handler	= &s5pv210_irq_handler[30],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "FIMC3",
		.no			= 31,
		.handler	= &s5pv210_irq_handler[31],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "CORTEX0",
		.no			= 32,
		.handler	= &s5pv210_irq_handler[32],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "CORTEX1",
		.no			= 33,
		.handler	= &s5pv210_irq_handler[33],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "CORTEX2",
		.no			= 34,
		.handler	= &s5pv210_irq_handler[34],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "CORTEX3",
		.no			= 35,
		.handler	= &s5pv210_irq_handler[35],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "CORTEX4",
		.no			= 36,
		.handler	= &s5pv210_irq_handler[36],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "IEM_APC",
		.no			= 37,
		.handler	= &s5pv210_irq_handler[37],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "IEM_IEC",
		.no			= 38,
		.handler	= &s5pv210_irq_handler[38],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "NFC",
		.no			= 40,
		.handler	= &s5pv210_irq_handler[40],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "CFC",
		.no			= 41,
		.handler	= &s5pv210_irq_handler[41],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "UART0",
		.no			= 42,
		.handler	= &s5pv210_irq_handler[42],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "UART1",
		.no			= 43,
		.handler	= &s5pv210_irq_handler[43],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "UART2",
		.no			= 44,
		.handler	= &s5pv210_irq_handler[44],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "UART3",
		.no			= 45,
		.handler	= &s5pv210_irq_handler[45],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "I2C0",
		.no			= 46,
		.handler	= &s5pv210_irq_handler[46],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "SPI0",
		.no			= 47,
		.handler	= &s5pv210_irq_handler[47],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "SPI1",
		.no			= 48,
		.handler	= &s5pv210_irq_handler[48],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "AUDIO_SS",
		.no			= 50,
		.handler	= &s5pv210_irq_handler[50],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "I2C2",
		.no			= 51,
		.handler	= &s5pv210_irq_handler[51],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "I2C_HDMI_PHY",
		.no			= 52,
		.handler	= &s5pv210_irq_handler[52],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "UHOST",
		.no			= 55,
		.handler	= &s5pv210_irq_handler[55],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "OTG",
		.no			= 56,
		.handler	= &s5pv210_irq_handler[56],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "MODEMIF",
		.no			= 57,
		.handler	= &s5pv210_irq_handler[57],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "HSMMC0",
		.no			= 58,
		.handler	= &s5pv210_irq_handler[58],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "HSMMC1",
		.no			= 59,
		.handler	= &s5pv210_irq_handler[59],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "HSMMC2",
		.no			= 60,
		.handler	= &s5pv210_irq_handler[60],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "MIPI_CSI",
		.no			= 61,
		.handler	= &s5pv210_irq_handler[61],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "MIPI_DSI",
		.no			= 62,
		.handler	= &s5pv210_irq_handler[62],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "ONENAND_AUDI",
		.no			= 63,
		.handler	= &s5pv210_irq_handler[63],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "LCD0",
		.no			= 64,
		.handler	= &s5pv210_irq_handler[64],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "LCD1",
		.no			= 65,
		.handler	= &s5pv210_irq_handler[65],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "LCD2",
		.no			= 66,
		.handler	= &s5pv210_irq_handler[66],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "ROTATOR",
		.no			= 68,
		.handler	= &s5pv210_irq_handler[68],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "FIMC0",
		.no			= 69,
		.handler	= &s5pv210_irq_handler[69],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "FIMC1",
		.no			= 70,
		.handler	= &s5pv210_irq_handler[70],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "FIMC2",
		.no			= 71,
		.handler	= &s5pv210_irq_handler[71],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "JPEG",
		.no			= 72,
		.handler	= &s5pv210_irq_handler[72],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "2D",
		.no			= 73,
		.handler	= &s5pv210_irq_handler[73],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "3D",
		.no			= 74,
		.handler	= &s5pv210_irq_handler[74],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "MIXER",
		.no			= 75,
		.handler	= &s5pv210_irq_handler[75],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "HDMI",
		.no			= 76,
		.handler	= &s5pv210_irq_handler[76],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "I2C_HDMI_DDC",
		.no			= 77,
		.handler	= &s5pv210_irq_handler[77],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "MFC",
		.no			= 78,
		.handler	= &s5pv210_irq_handler[78],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "TVENC",
		.no			= 79,
		.handler	= &s5pv210_irq_handler[79],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "I2S0",
		.no			= 80,
		.handler	= &s5pv210_irq_handler[80],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "I2S1",
		.no			= 81,
		.handler	= &s5pv210_irq_handler[81],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "AC97",
		.no			= 83,
		.handler	= &s5pv210_irq_handler[83],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "PCM0",
		.no			= 84,
		.handler	= &s5pv210_irq_handler[84],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "PCM1",
		.no			= 85,
		.handler	= &s5pv210_irq_handler[85],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "SPDIF",
		.no			= 86,
		.handler	= &s5pv210_irq_handler[86],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "ADC",
		.no			= 87,
		.handler	= &s5pv210_irq_handler[87],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "PENDN",
		.no			= 88,
		.handler	= &s5pv210_irq_handler[88],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "KEYPAD",
		.no			= 89,
		.handler	= &s5pv210_irq_handler[89],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "INTHASH_SSS",
		.no			= 91,
		.handler	= &s5pv210_irq_handler[91],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "INTFEEDCTRL_SSS",
		.no			= 92,
		.handler	= &s5pv210_irq_handler[92],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "PCM2",
		.no			= 93,
		.handler	= &s5pv210_irq_handler[93],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "SDM_IRQ",
		.no			= 94,
		.handler	= &s5pv210_irq_handler[94],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "SDM_FIQ",
		.no			= 95,
		.handler	= &s5pv210_irq_handler[95],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "MMC3",
		.no			= 98,
		.handler	= &s5pv210_irq_handler[98],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "CEC",
		.no			= 99,
		.handler	= &s5pv210_irq_handler[99],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "TSI",
		.no			= 100,
		.handler	= &s5pv210_irq_handler[100],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "ADC1",
		.no			= 105,
		.handler	= &s5pv210_irq_handler[105],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}, {
		.name		= "PENDN1",
		.no			= 106,
		.handler	= &s5pv210_irq_handler[106],
		.enable		= s5pv210_irq_enable,
		.disable	= s5pv210_irq_disable,
		.set_type	= s5pv210_irq_set_type,
	}
};

static __init void s5pv210_irq_init(void)
{
	int i;

	/* Select irq mode */
	write32(S5PV210_VIC0_INTSELECT, 0x00000000);
	write32(S5PV210_VIC1_INTSELECT, 0x00000000);
	write32(S5PV210_VIC2_INTSELECT, 0x00000000);
	write32(S5PV210_VIC3_INTSELECT, 0x00000000);

	/* Disable all interrupts */
	write32(S5PV210_VIC0_INTENABLE, 0x00000000);
	write32(S5PV210_VIC1_INTENABLE, 0x00000000);
	write32(S5PV210_VIC2_INTENABLE, 0x00000000);
	write32(S5PV210_VIC3_INTENABLE, 0x00000000);

	/* Clear all interrupts */
	write32(S5PV210_VIC0_INTENCLEAR, 0xffffffff);
	write32(S5PV210_VIC1_INTENCLEAR, 0xffffffff);
	write32(S5PV210_VIC2_INTENCLEAR, 0xffffffff);
	write32(S5PV210_VIC3_INTENCLEAR, 0xffffffff);

	/* Clear all irq status */
	write32(S5PV210_VIC0_IRQSTATUS, 0x00000000);
	write32(S5PV210_VIC1_IRQSTATUS, 0x00000000);
	write32(S5PV210_VIC2_IRQSTATUS, 0x00000000);
	write32(S5PV210_VIC3_IRQSTATUS, 0x00000000);

	/* Clear all fiq status */
	write32(S5PV210_VIC0_FIQSTATUS, 0x00000000);
	write32(S5PV210_VIC1_FIQSTATUS, 0x00000000);
	write32(S5PV210_VIC2_FIQSTATUS, 0x00000000);
	write32(S5PV210_VIC3_FIQSTATUS, 0x00000000);

	/* Clear all software interrupts */
	write32(S5PV210_VIC0_SOFTINTCLEAR, 0xffffffff);
	write32(S5PV210_VIC1_SOFTINTCLEAR, 0xffffffff);
	write32(S5PV210_VIC2_SOFTINTCLEAR, 0xffffffff);
	write32(S5PV210_VIC3_SOFTINTCLEAR, 0xffffffff);

	/* Set vic address to zero */
	write32(S5PV210_VIC0_ADDRESS, 0x00000000);
	write32(S5PV210_VIC1_ADDRESS, 0x00000000);
	write32(S5PV210_VIC2_ADDRESS, 0x00000000);
	write32(S5PV210_VIC3_ADDRESS, 0x00000000);

	for(i = 0; i< 32; i++)
	{
		write32((S5PV210_VIC0_VECTADDR0 + 4 * i), (u32_t)irq);
		write32((S5PV210_VIC1_VECTADDR0 + 4 * i), (u32_t)irq);
		write32((S5PV210_VIC2_VECTADDR0 + 4 * i), (u32_t)irq);
		write32((S5PV210_VIC3_VECTADDR0 + 4 * i), (u32_t)irq);
	}

	for(i = 0; i< 32; i++)
	{
		write32((S5PV210_VIC0_VECPRIORITY0 + 4 * i), 0xf);
		write32((S5PV210_VIC1_VECPRIORITY0 + 4 * i), 0xf);
		write32((S5PV210_VIC2_VECPRIORITY0 + 4 * i), 0xf);
		write32((S5PV210_VIC3_VECPRIORITY0 + 4 * i), 0xf);
	}

	for(i = 0; i < ARRAY_SIZE(s5pv210_irqs); i++)
	{
		if(irq_register(&s5pv210_irqs[i]))
			LOG("Register irq '%s'", s5pv210_irqs[i].name);
		else
			LOG("Failed to register irq '%s'", s5pv210_irqs[i].name);
	}

	/* Enable vector interrupt controller */
	vic_enable();

	/* Enable irq and fiq */
	irq_enable();
	fiq_enable();
}

static __exit void s5pv210_irq_exit(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(s5pv210_irqs); i++)
	{
		if(irq_unregister(&s5pv210_irqs[i]))
			LOG("Unregister irq '%s'", s5pv210_irqs[i].name);
		else
			LOG("Failed to unregister irq '%s'", s5pv210_irqs[i].name);
	}

	/* Disable vector interrupt controller */
	vic_disable();

	/* Disable irq and fiq */
	irq_disable();
	fiq_disable();
}

core_initcall(s5pv210_irq_init);
core_exitcall(s5pv210_irq_exit);
