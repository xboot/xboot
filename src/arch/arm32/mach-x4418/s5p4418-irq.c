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
#include <s5p4418/reg-gpio.h>
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
static struct irq_handler_t s5p4418_irq_handler_gpioa[32];
static struct irq_handler_t s5p4418_irq_handler_gpiob[32];
static struct irq_handler_t s5p4418_irq_handler_gpioc[32];
static struct irq_handler_t s5p4418_irq_handler_gpiod[32];
static struct irq_handler_t s5p4418_irq_handler_gpioe[32];

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

static void s5p4418_irq_handler_func_gpioa(void * data)
{
	u32_t det;
	u32_t offset;

	det = read32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_DET));
	if(det != 0)
	{
		offset = irq_offset(det);
		(s5p4418_irq_handler_gpioa[offset].func)(s5p4418_irq_handler_gpioa[offset].data);
		write32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_DET), (0x1 << offset));
	}
}

static void s5p4418_irq_handler_func_gpiob(void * data)
{
	u32_t det;
	u32_t offset;

	det = read32(phys_to_virt(S5P4418_GPIOB_BASE + GPIO_DET));
	if(det != 0)
	{
		offset = irq_offset(det);
		(s5p4418_irq_handler_gpiob[offset].func)(s5p4418_irq_handler_gpiob[offset].data);
		write32(phys_to_virt(S5P4418_GPIOB_BASE + GPIO_DET), (0x1 << offset));
	}
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
		write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_SOFTINTCLEAR), 0x1 << offset);

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
		write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_SOFTINTCLEAR), 0x1 << (offset-32));

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

static void s5p4418_irq_enable(struct irq_t * irq)
{
	u32_t no = irq->no;

	/* VIC0 */
	if(no < 32)
	{
		write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTENABLE), (read32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTENABLE)) | (0x1 << no)));
	}
	/* VIC1 */
	else if(no < 64)
	{
		no = no - 32;
		write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENABLE), (read32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENABLE)) | (0x1 << no)));
	}
	/* GPIOA */
	else if(no < 96)
	{
		no = no - 64;
		write32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_INTENB)) | (0x1 << no)));
	}
	/* GPIOB */
	else if(no < 128)
	{
		no = no - 96;
		write32(phys_to_virt(S5P4418_GPIOB_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOB_BASE + GPIO_INTENB)) | (0x1 << no)));
	}
	/* GPIOC */
	else if(no < 160)
	{
		no = no - 128;
		write32(phys_to_virt(S5P4418_GPIOC_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOC_BASE + GPIO_INTENB)) | (0x1 << no)));
	}
	/* GPIOD */
	else if(no < 192)
	{
		no = no - 160;
		write32(phys_to_virt(S5P4418_GPIOD_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOD_BASE + GPIO_INTENB)) | (0x1 << no)));
	}
	/* GPIOE */
	else if(no < 224)
	{
		no = no - 192;
		write32(phys_to_virt(S5P4418_GPIOE_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOE_BASE + GPIO_INTENB)) | (0x1 << no)));
	}
}

static void s5p4418_irq_disable(struct irq_t * irq)
{
	u32_t no = irq->no;

	/* VIC0 */
	if(no < 32)
	{
		write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTENCLEAR), (read32(phys_to_virt(S5P4418_VIC0_BASE + VIC_INTENCLEAR)) | (0x1 << no)));
	}
	/* VIC1 */
	else if(no < 64)
	{
		no = no - 32;
		write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENCLEAR), (read32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENCLEAR)) | (0x1 << no)));
	}
	/* GPIOA */
	else if(no < 96)
	{
		no = no - 64;
		write32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_INTENB)) & ~(0x1 << no)));
	}
	/* GPIOB */
	else if(no < 128)
	{
		no = no - 96;
		write32(phys_to_virt(S5P4418_GPIOB_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOB_BASE + GPIO_INTENB)) & ~(0x1 << no)));
	}
	/* GPIOC */
	else if(no < 160)
	{
		no = no - 128;
		write32(phys_to_virt(S5P4418_GPIOC_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOC_BASE + GPIO_INTENB)) & ~(0x1 << no)));
	}
	/* GPIOD */
	else if(no < 192)
	{
		no = no - 160;
		write32(phys_to_virt(S5P4418_GPIOD_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOD_BASE + GPIO_INTENB)) & ~(0x1 << no)));
	}
	/* GPIOE */
	else if(no < 224)
	{
		no = no - 192;
		write32(phys_to_virt(S5P4418_GPIOE_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P4418_GPIOE_BASE + GPIO_INTENB)) & ~(0x1 << no)));
	}
}

static void s5p4418_irq_set_type(struct irq_t * irq, enum irq_type_t type)
{
	u32_t no = irq->no;

	/* VIC0 */
	if(no < 32)
	{
	}
	/* VIC1 */
	else if(no < 64)
	{
		no = no - 32;
	}
	/* GPIOA */
	else if(no < 96)
	{
		no = no - 64;
	}
	/* GPIOB */
	else if(no < 128)
	{
		no = no - 96;
	}
	/* GPIOC */
	else if(no < 160)
	{
		no = no - 128;
	}
	/* GPIOD */
	else if(no < 192)
	{
		no = no - 160;
	}
	/* GPIOE */
	else if(no < 224)
	{
		no = no - 192;
	}
/*
	switch(type)
	{
	case IRQ_TYPE_NONE:
		break;
	case IRQ_TYPE_LEVEL_LOW:
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		break;
	case IRQ_TYPE_EDGE_RISING:
		break;
	case IRQ_TYPE_EDGE_FALLING:
		break;
	case IRQ_TYPE_EDGE_BOTH:
		break;
	default:
		break;
	}*/
}

static struct irq_t s5p4418_irqs[] = {
	{
		.name		= "MCUSTOP",
		.no			= 0,
		.handler	= &s5p4418_irq_handler[0],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "DMA0",
		.no			= 1,
		.handler	= &s5p4418_irq_handler[1],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "DMA1",
		.no			= 2,
		.handler	= &s5p4418_irq_handler[2],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "CLKPWR0",
		.no			= 3,
		.handler	= &s5p4418_irq_handler[3],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "CLKPWR1",
		.no			= 4,
		.handler	= &s5p4418_irq_handler[4],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "CLKPWR2",
		.no			= 5,
		.handler	= &s5p4418_irq_handler[5],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "UART1",
		.no			= 6,
		.handler	= &s5p4418_irq_handler[6],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "UART0",
		.no			= 7,
		.handler	= &s5p4418_irq_handler[7],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "UART2",
		.no			= 8,
		.handler	= &s5p4418_irq_handler[8],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "UART3",
		.no			= 9,
		.handler	= &s5p4418_irq_handler[9],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "UART4",
		.no			= 10,
		.handler	= &s5p4418_irq_handler[10],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "SSP0",
		.no			= 12,
		.handler	= &s5p4418_irq_handler[12],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "SSP1",
		.no			= 13,
		.handler	= &s5p4418_irq_handler[13],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "SSP2",
		.no			= 14,
		.handler	= &s5p4418_irq_handler[14],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "I2C0",
		.no			= 15,
		.handler	= &s5p4418_irq_handler[15],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "I2C1",
		.no			= 16,
		.handler	= &s5p4418_irq_handler[16],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "I2C2",
		.no			= 17,
		.handler	= &s5p4418_irq_handler[17],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "DEINTERLACE",
		.no			= 18,
		.handler	= &s5p4418_irq_handler[18],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "SCALER",
		.no			= 19,
		.handler	= &s5p4418_irq_handler[19],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "AC97",
		.no			= 20,
		.handler	= &s5p4418_irq_handler[20],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "SPDIFRX",
		.no			= 21,
		.handler	= &s5p4418_irq_handler[21],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "SPDIFTX",
		.no			= 22,
		.handler	= &s5p4418_irq_handler[22],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "TIMER0",
		.no			= 23,
		.handler	= &s5p4418_irq_handler[23],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "TIMER1",
		.no			= 24,
		.handler	= &s5p4418_irq_handler[24],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "TIMER2",
		.no			= 25,
		.handler	= &s5p4418_irq_handler[25],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "TIMER3",
		.no			= 26,
		.handler	= &s5p4418_irq_handler[26],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "PWM0",
		.no			= 27,
		.handler	= &s5p4418_irq_handler[27],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "PWM1",
		.no			= 28,
		.handler	= &s5p4418_irq_handler[28],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "PWM2",
		.no			= 29,
		.handler	= &s5p4418_irq_handler[29],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "PWM3",
		.no			= 30,
		.handler	= &s5p4418_irq_handler[30],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "WDT",
		.no			= 31,
		.handler	= &s5p4418_irq_handler[31],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "MPEGTSI",
		.no			= 32,
		.handler	= &s5p4418_irq_handler[32],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "DISPLAYTOP0",
		.no			= 33,
		.handler	= &s5p4418_irq_handler[33],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "DISPLAYTOP1",
		.no			= 34,
		.handler	= &s5p4418_irq_handler[34],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "DISPLAYTOP2",
		.no			= 35,
		.handler	= &s5p4418_irq_handler[35],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "DISPLAYTOP3",
		.no			= 36,
		.handler	= &s5p4418_irq_handler[36],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "VIP0",
		.no			= 37,
		.handler	= &s5p4418_irq_handler[37],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "VIP1",
		.no			= 38,
		.handler	= &s5p4418_irq_handler[38],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "MIPI",
		.no			= 39,
		.handler	= &s5p4418_irq_handler[39],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "3DGPU",
		.no			= 40,
		.handler	= &s5p4418_irq_handler[40],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "ADC",
		.no			= 41,
		.handler	= &s5p4418_irq_handler[41],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "PPM",
		.no			= 42,
		.handler	= &s5p4418_irq_handler[42],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "SDMMC0",
		.no			= 43,
		.handler	= &s5p4418_irq_handler[43],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "SDMMC1",
		.no			= 44,
		.handler	= &s5p4418_irq_handler[44],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "SDMMC2",
		.no			= 45,
		.handler	= &s5p4418_irq_handler[45],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "CODA9600",
		.no			= 46,
		.handler	= &s5p4418_irq_handler[46],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "CODA9601",
		.no			= 47,
		.handler	= &s5p4418_irq_handler[47],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GMAC",
		.no			= 48,
		.handler	= &s5p4418_irq_handler[48],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "USB20OTG",
		.no			= 49,
		.handler	= &s5p4418_irq_handler[49],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "USB20HOST",
		.no			= 50,
		.handler	= &s5p4418_irq_handler[50],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA",
		.no			= 53,
		.handler	= &s5p4418_irq_handler[53],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB",
		.no			= 54,
		.handler	= &s5p4418_irq_handler[54],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC",
		.no			= 55,
		.handler	= &s5p4418_irq_handler[55],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD",
		.no			= 56,
		.handler	= &s5p4418_irq_handler[56],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE",
		.no			= 57,
		.handler	= &s5p4418_irq_handler[57],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "CRYPTO",
		.no			= 58,
		.handler	= &s5p4418_irq_handler[58],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "PDM",
		.no			= 59,
		.handler	= &s5p4418_irq_handler[59],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}
};

static __init void s5p4418_irq_init(void)
{
	int i;

	/* VIC to core, pass through GIC */
	write32(phys_to_virt(0xf0000100), 0);

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

	for(i = 0; i < 32; i++)
	{
		write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_VECPRIORITY0 + 4 * i), 0xf);
		write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_VECPRIORITY0 + 4 * i), 0xf);
	}

	for(i = 0; i < 32; i++)
	{
		write32(phys_to_virt(S5P4418_VIC0_BASE + VIC_VECTADDR0 + 4 * i), (u32_t)irq);
		write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_VECTADDR0 + 4 * i), (u32_t)irq);
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
