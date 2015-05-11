/*
 * s5p4418-irq.c
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
#include <s5p4418/reg-vic.h>

/*
 * Exception handlers for irq from Start.s
 */
extern void irq(void);

struct pt_regs_t {
	u32_t	r0,		r1,		r2,		r3, 	r4,		r5;
	u32_t	r6,		r7,		r8, 	r9, 	r10,	fp;
	u32_t	ip, 	sp, 	lr, 	pc,		cpsr, 	orig_r0;
};

static struct irq_handler_t s5p4418_irq_handler[64];

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

void do_irqs(struct pt_regs_t * regs)
{
	u32_t vic0, vic1;
	u32_t offset;

	/* Read vector interrupt controller's irq status */
	vic0 = read32(phys_to_virt(S5P4418_VIC0_BASE + VIC_IRQSTATUS));
	vic1 = read32(phys_to_virt(S5P4418_VIC1_BASE + VIC_IRQSTATUS));

	if(vic0 != 0)
	{
		/* Get interrupt offset */
		offset = irq_offset(vic0);

		/* Handle interrupt server function */
		(s5p4418_irq_handler[offset].func)(s5p4418_irq_handler[offset].data);

		/* Clear software interrupt */
		write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_SOFTINTCLEAR), 0x1<<offset);

		/* Set vic address to zero */
		write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_ADDRESS), 0x00000000);
	}
	else if(vic1 != 0)
	{
		/* Get interrupt offset */
		offset = irq_offset(vic1);

		/* Handle interrupt server function */
		(s5p4418_irq_handler[offset + 32].func)(s5p4418_irq_handler[offset + 32].data);

		/* Clear software interrupt */
		write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_SOFTINTCLEAR), 0x1<<(offset-32));

		/* Set all vic address to zero */
		write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_ADDRESS), 0x00000000);
	}
	else
	{
		/* Clear all software interrupts */
		write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_SOFTINTCLEAR), 0xffffffff);
		write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_SOFTINTCLEAR), 0xffffffff);

		/* Set vic address to zero */
		write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_ADDRESS), 0x00000000);
		write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_ADDRESS), 0x00000000);
	}
}

static void enable_irqs(struct irq_t * irq, bool_t enable)
{
	u32_t irq_no = irq->irq_no;

	if(irq_no < 32)
	{
		if(enable)
			write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTENABLE), (read32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTENABLE)) | (0x1<<irq_no)));
		else
			write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTENCLEAR), (read32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTENCLEAR)) | (0x1<<irq_no)));
	}
	else if(irq_no < 64)
	{
		irq_no = irq_no - 32;

		if(enable)
			write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENABLE), (read32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENABLE)) | (0x1<<irq_no)));
		else
			write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENCLEAR), (read32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENCLEAR)) | (0x1<<irq_no)));
	}
	else
	{
		/* Not support */
	}
}

static struct irq_t s5p4418_irqs[] = {
	{
		.name		= "MCUSTOP",
		.irq_no		= 0,
		.handler	= &s5p4418_irq_handler[0],
		.enable		= enable_irqs,
	}, {
		.name		= "DMA0",
		.irq_no		= 1,
		.handler	= &s5p4418_irq_handler[1],
		.enable		= enable_irqs,
	}, {
		.name		= "DMA1",
		.irq_no		= 2,
		.handler	= &s5p4418_irq_handler[2],
		.enable		= enable_irqs,
	}, {
		.name		= "CLKPWR0",
		.irq_no		= 3,
		.handler	= &s5p4418_irq_handler[3],
		.enable		= enable_irqs,
	}, {
		.name		= "CLKPWR1",
		.irq_no		= 4,
		.handler	= &s5p4418_irq_handler[4],
		.enable		= enable_irqs,
	}, {
		.name		= "CLKPWR2",
		.irq_no		= 5,
		.handler	= &s5p4418_irq_handler[5],
		.enable		= enable_irqs,
	}, {
		.name		= "UART1",
		.irq_no		= 6,
		.handler	= &s5p4418_irq_handler[6],
		.enable		= enable_irqs,
	}, {
		.name		= "UART0",
		.irq_no		= 7,
		.handler	= &s5p4418_irq_handler[7],
		.enable		= enable_irqs,
	}, {
		.name		= "UART2",
		.irq_no		= 8,
		.handler	= &s5p4418_irq_handler[8],
		.enable		= enable_irqs,
	}, {
		.name		= "UART3",
		.irq_no		= 9,
		.handler	= &s5p4418_irq_handler[9],
		.enable		= enable_irqs,
	}, {
		.name		= "UART4",
		.irq_no		= 10,
		.handler	= &s5p4418_irq_handler[10],
		.enable		= enable_irqs,
	}, {
		.name		= "SSP0",
		.irq_no		= 12,
		.handler	= &s5p4418_irq_handler[12],
		.enable		= enable_irqs,
	}, {
		.name		= "SSP1",
		.irq_no		= 13,
		.handler	= &s5p4418_irq_handler[13],
		.enable		= enable_irqs,
	}, {
		.name		= "SSP2",
		.irq_no		= 14,
		.handler	= &s5p4418_irq_handler[14],
		.enable		= enable_irqs,
	}, {
		.name		= "I2C0",
		.irq_no		= 15,
		.handler	= &s5p4418_irq_handler[15],
		.enable		= enable_irqs,
	}, {
		.name		= "I2C1",
		.irq_no		= 16,
		.handler	= &s5p4418_irq_handler[16],
		.enable		= enable_irqs,
	}, {
		.name		= "I2C2",
		.irq_no		= 17,
		.handler	= &s5p4418_irq_handler[17],
		.enable		= enable_irqs,
	}, {
		.name		= "DEINTERLACE",
		.irq_no		= 18,
		.handler	= &s5p4418_irq_handler[18],
		.enable		= enable_irqs,
	}, {
		.name		= "SCALER",
		.irq_no		= 19,
		.handler	= &s5p4418_irq_handler[19],
		.enable		= enable_irqs,
	}, {
		.name		= "AC97",
		.irq_no		= 20,
		.handler	= &s5p4418_irq_handler[20],
		.enable		= enable_irqs,
	}, {
		.name		= "SPDIFRX",
		.irq_no		= 21,
		.handler	= &s5p4418_irq_handler[21],
		.enable		= enable_irqs,
	}, {
		.name		= "SPDIFTX",
		.irq_no		= 22,
		.handler	= &s5p4418_irq_handler[22],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER0",
		.irq_no		= 23,
		.handler	= &s5p4418_irq_handler[23],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER1",
		.irq_no		= 24,
		.handler	= &s5p4418_irq_handler[24],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER2",
		.irq_no		= 25,
		.handler	= &s5p4418_irq_handler[25],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER3",
		.irq_no		= 26,
		.handler	= &s5p4418_irq_handler[26],
		.enable		= enable_irqs,
	}, {
		.name		= "PWM0",
		.irq_no		= 27,
		.handler	= &s5p4418_irq_handler[27],
		.enable		= enable_irqs,
	}, {
		.name		= "PWM1",
		.irq_no		= 28,
		.handler	= &s5p4418_irq_handler[28],
		.enable		= enable_irqs,
	}, {
		.name		= "PWM2",
		.irq_no		= 29,
		.handler	= &s5p4418_irq_handler[29],
		.enable		= enable_irqs,
	}, {
		.name		= "PWM3",
		.irq_no		= 30,
		.handler	= &s5p4418_irq_handler[30],
		.enable		= enable_irqs,
	}, {
		.name		= "WDT",
		.irq_no		= 31,
		.handler	= &s5p4418_irq_handler[31],
		.enable		= enable_irqs,
	}, {
		.name		= "MPEGTSI",
		.irq_no		= 32,
		.handler	= &s5p4418_irq_handler[32],
		.enable		= enable_irqs,
	}, {
		.name		= "DISPLAYTOP0",
		.irq_no		= 33,
		.handler	= &s5p4418_irq_handler[33],
		.enable		= enable_irqs,
	}, {
		.name		= "DISPLAYTOP1",
		.irq_no		= 34,
		.handler	= &s5p4418_irq_handler[34],
		.enable		= enable_irqs,
	}, {
		.name		= "DISPLAYTOP2",
		.irq_no		= 35,
		.handler	= &s5p4418_irq_handler[35],
		.enable		= enable_irqs,
	}, {
		.name		= "DISPLAYTOP3",
		.irq_no		= 36,
		.handler	= &s5p4418_irq_handler[36],
		.enable		= enable_irqs,
	}, {
		.name		= "VIP0",
		.irq_no		= 37,
		.handler	= &s5p4418_irq_handler[37],
		.enable		= enable_irqs,
	}, {
		.name		= "VIP1",
		.irq_no		= 38,
		.handler	= &s5p4418_irq_handler[38],
		.enable		= enable_irqs,
	}, {
		.name		= "MIPI",
		.irq_no		= 39,
		.handler	= &s5p4418_irq_handler[39],
		.enable		= enable_irqs,
	}, {
		.name		= "3DGPU",
		.irq_no		= 40,
		.handler	= &s5p4418_irq_handler[40],
		.enable		= enable_irqs,
	}, {
		.name		= "ADC",
		.irq_no		= 41,
		.handler	= &s5p4418_irq_handler[41],
		.enable		= enable_irqs,
	}, {
		.name		= "PPM",
		.irq_no		= 42,
		.handler	= &s5p4418_irq_handler[42],
		.enable		= enable_irqs,
	}, {
		.name		= "SDMMC0",
		.irq_no		= 43,
		.handler	= &s5p4418_irq_handler[43],
		.enable		= enable_irqs,
	}, {
		.name		= "SDMMC1",
		.irq_no		= 44,
		.handler	= &s5p4418_irq_handler[44],
		.enable		= enable_irqs,
	}, {
		.name		= "SDMMC2",
		.irq_no		= 45,
		.handler	= &s5p4418_irq_handler[45],
		.enable		= enable_irqs,
	}, {
		.name		= "CODA9600",
		.irq_no		= 46,
		.handler	= &s5p4418_irq_handler[46],
		.enable		= enable_irqs,
	}, {
		.name		= "CODA9601",
		.irq_no		= 47,
		.handler	= &s5p4418_irq_handler[47],
		.enable		= enable_irqs,
	}, {
		.name		= "GMAC",
		.irq_no		= 48,
		.handler	= &s5p4418_irq_handler[48],
		.enable		= enable_irqs,
	}, {
		.name		= "USB20OTG",
		.irq_no		= 49,
		.handler	= &s5p4418_irq_handler[49],
		.enable		= enable_irqs,
	}, {
		.name		= "USB20HOST",
		.irq_no		= 50,
		.handler	= &s5p4418_irq_handler[50],
		.enable		= enable_irqs,
	}, {
		.name		= "GPIOA",
		.irq_no		= 53,
		.handler	= &s5p4418_irq_handler[53],
		.enable		= enable_irqs,
	}, {
		.name		= "GPIOB",
		.irq_no		= 54,
		.handler	= &s5p4418_irq_handler[54],
		.enable		= enable_irqs,
	}, {
		.name		= "GPIOC",
		.irq_no		= 55,
		.handler	= &s5p4418_irq_handler[55],
		.enable		= enable_irqs,
	}, {
		.name		= "GPIOD",
		.irq_no		= 56,
		.handler	= &s5p4418_irq_handler[56],
		.enable		= enable_irqs,
	}, {
		.name		= "GPIOE",
		.irq_no		= 57,
		.handler	= &s5p4418_irq_handler[57],
		.enable		= enable_irqs,
	}, {
		.name		= "CRYPTO",
		.irq_no		= 58,
		.handler	= &s5p4418_irq_handler[58],
		.enable		= enable_irqs,
	}, {
		.name		= "PDM",
		.irq_no		= 59,
		.handler	= &s5p4418_irq_handler[59],
		.enable		= enable_irqs,
	}
};

static __init void s5p4418_irq_init(void)
{
	int i;

	/* Select irq mode */
	write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTSELECT), 0x00000000);
	write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTSELECT), 0x00000000);

	/* Disable all interrupts */
	write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTENABLE), 0x00000000);
	write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENABLE), 0x00000000);

	/* Clear all interrupts */
	write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTENCLEAR), 0xffffffff);
	write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENCLEAR), 0xffffffff);

	/* Clear all irq status */
	write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_IRQSTATUS), 0x00000000);
	write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_IRQSTATUS), 0x00000000);

	/* Clear all fiq status */
	write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_FIQSTATUS), 0x00000000);
	write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_FIQSTATUS), 0x00000000);

	/* Clear all software interrupts */
	write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_SOFTINTCLEAR), 0xffffffff);
	write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_SOFTINTCLEAR), 0xffffffff);

	/* Set vic address to zero */
	write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_ADDRESS), 0x00000000);
	write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_ADDRESS), 0x00000000);

	for(i = 0; i< 32; i++)
	{
		write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_VECTADDR0 + 4 * i), (u32_t)irq);
		write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_VECTADDR0 + 4 * i), (u32_t)irq);
	}

	for(i = 0; i< 32; i++)
	{
		write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_VECPRIORITY0 + 4 * i), 0xf);
		write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_VECPRIORITY0 + 4 * i), 0xf);
	}

	for(i = 0; i < ARRAY_SIZE(s5p4418_irqs); i++)
	{
		if(irq_register(&s5p4418_irqs[i]))
			LOG("Register irq '%s'", s5p4418_irqs[i].name);
		else
			LOG("Failed to register irq '%s'", s5p4418_irqs[i].name);
	}

	/* Enable vector interrupt controller */
	vic_enable();

	/* Enable irq and fiq */
	irq_enable();
	fiq_enable();
}

static __exit void s5p4418_irq_exit(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(s5p4418_irqs); i++)
	{
		if(irq_unregister(&s5p4418_irqs[i]))
			LOG("Unregister irq '%s'", s5p4418_irqs[i].name);
		else
			LOG("Failed to unregister irq '%s'", s5p4418_irqs[i].name);
	}

	/* Disable vector interrupt controller */
	vic_disable();

	/* Disable irq and fiq */
	irq_disable();
	fiq_disable();
}

core_initcall(s5p4418_irq_init);
core_exitcall(s5p4418_irq_exit);
