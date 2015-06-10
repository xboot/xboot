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

static void s5p4418_irq_handler_func_gpioc(void * data)
{
	u32_t det;
	u32_t offset;

	det = read32(phys_to_virt(S5P4418_GPIOC_BASE + GPIO_DET));
	if(det != 0)
	{
		offset = irq_offset(det);
		(s5p4418_irq_handler_gpioc[offset].func)(s5p4418_irq_handler_gpioc[offset].data);
		write32(phys_to_virt(S5P4418_GPIOC_BASE + GPIO_DET), (0x1 << offset));
	}
}

static void s5p4418_irq_handler_func_gpiod(void * data)
{
	u32_t det;
	u32_t offset;

	det = read32(phys_to_virt(S5P4418_GPIOD_BASE + GPIO_DET));
	if(det != 0)
	{
		offset = irq_offset(det);
		(s5p4418_irq_handler_gpiod[offset].func)(s5p4418_irq_handler_gpiod[offset].data);
		write32(phys_to_virt(S5P4418_GPIOD_BASE + GPIO_DET), (0x1 << offset));
	}
}

static void s5p4418_irq_handler_func_gpioe(void * data)
{
	u32_t det;
	u32_t offset;

	det = read32(phys_to_virt(S5P4418_GPIOE_BASE + GPIO_DET));
	if(det != 0)
	{
		offset = irq_offset(det);
		(s5p4418_irq_handler_gpioe[offset].func)(s5p4418_irq_handler_gpioe[offset].data);
		write32(phys_to_virt(S5P4418_GPIOE_BASE + GPIO_DET), (0x1 << offset));
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
		write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_SOFTINTCLEAR), 0x1 << (offset - 32));

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
	int no = irq->no;

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
	int no = irq->no;

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
	int no = irq->no;
	u32_t val, cfg;

	switch(type)
	{
	case IRQ_TYPE_NONE:
		cfg = 0xf;
		break;
	case IRQ_TYPE_LEVEL_LOW:
		cfg = 0x0;
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		cfg = 0x1;
		break;
	case IRQ_TYPE_EDGE_FALLING:
		cfg = 0x2;
		break;
	case IRQ_TYPE_EDGE_RISING:
		cfg = 0x3;
		break;
	case IRQ_TYPE_EDGE_BOTH:
		cfg = 0x4;
		break;
	default:
		return;
	}

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
		if(cfg != 0xf)
		{
			if(no < 16)
			{
				val = read32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_DETMODE0));
				val &= ~(0x3 << (no << 0x1));
				val |= (cfg & 0x3) << (no << 0x1);
				write32(S5P4418_GPIOA_BASE + GPIO_DETMODE0, val);
			}
			else if(no < 32)
			{
				val = read32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_DETMODE1));
				val &= ~(0x3 << ((no - 16) << 0x1));
				val |= (cfg & 0x3) << ((no - 16) << 0x1);
				write32(S5P4418_GPIOA_BASE + GPIO_DETMODE1, val);
			}

			val = read32(phys_to_virt(S5P4418_GPIOA_BASE + GPIO_DETMODEEX));
			if(cfg & 0x4)
				val |= 0x1 << no;
			else
				val &= ~(0x1 << no);
			write32(S5P4418_GPIOA_BASE + GPIO_DETMODEEX, val);

			val = read32(S5P4418_GPIOA_BASE + GPIO_DETENB);
			val |= 0x1 << no;
			write32(S5P4418_GPIOA_BASE + GPIO_DETENB, val);
		}
		else
		{
			val = read32(S5P4418_GPIOA_BASE + GPIO_DETENB);
			val &= ~(0x1 << no);
			write32(S5P4418_GPIOA_BASE + GPIO_DETENB, val);
		}
	}
	/* GPIOB */
	else if(no < 128)
	{
		no = no - 96;
		if(cfg != 0xf)
		{
			if(no < 16)
			{
				val = read32(phys_to_virt(S5P4418_GPIOB_BASE + GPIO_DETMODE0));
				val &= ~(0x3 << (no << 0x1));
				val |= (cfg & 0x3) << (no << 0x1);
				write32(S5P4418_GPIOB_BASE + GPIO_DETMODE0, val);
			}
			else if(no < 32)
			{
				val = read32(phys_to_virt(S5P4418_GPIOB_BASE + GPIO_DETMODE1));
				val &= ~(0x3 << ((no - 16) << 0x1));
				val |= (cfg & 0x3) << ((no - 16) << 0x1);
				write32(S5P4418_GPIOB_BASE + GPIO_DETMODE1, val);
			}

			val = read32(phys_to_virt(S5P4418_GPIOB_BASE + GPIO_DETMODEEX));
			if(cfg & 0x4)
				val |= 0x1 << no;
			else
				val &= ~(0x1 << no);
			write32(S5P4418_GPIOB_BASE + GPIO_DETMODEEX, val);

			val = read32(S5P4418_GPIOB_BASE + GPIO_DETENB);
			val |= 0x1 << no;
			write32(S5P4418_GPIOB_BASE + GPIO_DETENB, val);
		}
		else
		{
			val = read32(S5P4418_GPIOB_BASE + GPIO_DETENB);
			val &= ~(0x1 << no);
			write32(S5P4418_GPIOB_BASE + GPIO_DETENB, val);
		}
	}
	/* GPIOC */
	else if(no < 160)
	{
		no = no - 128;
		if(cfg != 0xf)
		{
			if(no < 16)
			{
				val = read32(phys_to_virt(S5P4418_GPIOC_BASE + GPIO_DETMODE0));
				val &= ~(0x3 << (no << 0x1));
				val |= (cfg & 0x3) << (no << 0x1);
				write32(S5P4418_GPIOC_BASE + GPIO_DETMODE0, val);
			}
			else if(no < 32)
			{
				val = read32(phys_to_virt(S5P4418_GPIOC_BASE + GPIO_DETMODE1));
				val &= ~(0x3 << ((no - 16) << 0x1));
				val |= (cfg & 0x3) << ((no - 16) << 0x1);
				write32(S5P4418_GPIOC_BASE + GPIO_DETMODE1, val);
			}

			val = read32(phys_to_virt(S5P4418_GPIOC_BASE + GPIO_DETMODEEX));
			if(cfg & 0x4)
				val |= 0x1 << no;
			else
				val &= ~(0x1 << no);
			write32(S5P4418_GPIOC_BASE + GPIO_DETMODEEX, val);

			val = read32(S5P4418_GPIOC_BASE + GPIO_DETENB);
			val |= 0x1 << no;
			write32(S5P4418_GPIOC_BASE + GPIO_DETENB, val);
		}
		else
		{
			val = read32(S5P4418_GPIOC_BASE + GPIO_DETENB);
			val &= ~(0x1 << no);
			write32(S5P4418_GPIOC_BASE + GPIO_DETENB, val);
		}
	}
	/* GPIOD */
	else if(no < 192)
	{
		no = no - 160;
		if(cfg != 0xf)
		{
			if(no < 16)
			{
				val = read32(phys_to_virt(S5P4418_GPIOD_BASE + GPIO_DETMODE0));
				val &= ~(0x3 << (no << 0x1));
				val |= (cfg & 0x3) << (no << 0x1);
				write32(S5P4418_GPIOD_BASE + GPIO_DETMODE0, val);
			}
			else if(no < 32)
			{
				val = read32(phys_to_virt(S5P4418_GPIOD_BASE + GPIO_DETMODE1));
				val &= ~(0x3 << ((no - 16) << 0x1));
				val |= (cfg & 0x3) << ((no - 16) << 0x1);
				write32(S5P4418_GPIOD_BASE + GPIO_DETMODE1, val);
			}

			val = read32(phys_to_virt(S5P4418_GPIOD_BASE + GPIO_DETMODEEX));
			if(cfg & 0x4)
				val |= 0x1 << no;
			else
				val &= ~(0x1 << no);
			write32(S5P4418_GPIOD_BASE + GPIO_DETMODEEX, val);

			val = read32(S5P4418_GPIOD_BASE + GPIO_DETENB);
			val |= 0x1 << no;
			write32(S5P4418_GPIOD_BASE + GPIO_DETENB, val);
		}
		else
		{
			val = read32(S5P4418_GPIOD_BASE + GPIO_DETENB);
			val &= ~(0x1 << no);
			write32(S5P4418_GPIOD_BASE + GPIO_DETENB, val);
		}
	}
	/* GPIOE */
	else if(no < 224)
	{
		no = no - 192;
		if(cfg != 0xf)
		{
			if(no < 16)
			{
				val = read32(phys_to_virt(S5P4418_GPIOE_BASE + GPIO_DETMODE0));
				val &= ~(0x3 << (no << 0x1));
				val |= (cfg & 0x3) << (no << 0x1);
				write32(S5P4418_GPIOE_BASE + GPIO_DETMODE0, val);
			}
			else if(no < 32)
			{
				val = read32(phys_to_virt(S5P4418_GPIOE_BASE + GPIO_DETMODE1));
				val &= ~(0x3 << ((no - 16) << 0x1));
				val |= (cfg & 0x3) << ((no - 16) << 0x1);
				write32(S5P4418_GPIOE_BASE + GPIO_DETMODE1, val);
			}

			val = read32(phys_to_virt(S5P4418_GPIOE_BASE + GPIO_DETMODEEX));
			if(cfg & 0x4)
				val |= 0x1 << no;
			else
				val &= ~(0x1 << no);
			write32(S5P4418_GPIOE_BASE + GPIO_DETMODEEX, val);

			val = read32(S5P4418_GPIOE_BASE + GPIO_DETENB);
			val |= 0x1 << no;
			write32(S5P4418_GPIOE_BASE + GPIO_DETENB, val);
		}
		else
		{
			val = read32(S5P4418_GPIOE_BASE + GPIO_DETENB);
			val &= ~(0x1 << no);
			write32(S5P4418_GPIOE_BASE + GPIO_DETENB, val);
		}
	}
}

static struct irq_t s5p4418_irqs[] = {
	/* VIC0 */
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
	},

	/* VIC1 */
	{
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
	},

	/* GPIOA */
	{
		.name		= "GPIOA0",
		.no			= 64 + 0,
		.handler	= &s5p4418_irq_handler_gpioa[0],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA1",
		.no			= 64 + 1,
		.handler	= &s5p4418_irq_handler_gpioa[1],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA2",
		.no			= 64 + 2,
		.handler	= &s5p4418_irq_handler_gpioa[2],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA3",
		.no			= 64 + 3,
		.handler	= &s5p4418_irq_handler_gpioa[3],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA4",
		.no			= 64 + 4,
		.handler	= &s5p4418_irq_handler_gpioa[4],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA5",
		.no			= 64 + 5,
		.handler	= &s5p4418_irq_handler_gpioa[5],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA6",
		.no			= 64 + 6,
		.handler	= &s5p4418_irq_handler_gpioa[6],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA7",
		.no			= 64 + 7,
		.handler	= &s5p4418_irq_handler_gpioa[7],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA8",
		.no			= 64 + 8,
		.handler	= &s5p4418_irq_handler_gpioa[8],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA9",
		.no			= 64 + 9,
		.handler	= &s5p4418_irq_handler_gpioa[9],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA10",
		.no			= 64 + 10,
		.handler	= &s5p4418_irq_handler_gpioa[10],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA11",
		.no			= 64 + 11,
		.handler	= &s5p4418_irq_handler_gpioa[11],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA12",
		.no			= 64 + 12,
		.handler	= &s5p4418_irq_handler_gpioa[12],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA13",
		.no			= 64 + 13,
		.handler	= &s5p4418_irq_handler_gpioa[13],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA14",
		.no			= 64 + 14,
		.handler	= &s5p4418_irq_handler_gpioa[14],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA15",
		.no			= 64 + 15,
		.handler	= &s5p4418_irq_handler_gpioa[15],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA16",
		.no			= 64 + 16,
		.handler	= &s5p4418_irq_handler_gpioa[16],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA17",
		.no			= 64 + 17,
		.handler	= &s5p4418_irq_handler_gpioa[17],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA18",
		.no			= 64 + 18,
		.handler	= &s5p4418_irq_handler_gpioa[18],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA19",
		.no			= 64 + 19,
		.handler	= &s5p4418_irq_handler_gpioa[19],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA20",
		.no			= 64 + 20,
		.handler	= &s5p4418_irq_handler_gpioa[20],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA21",
		.no			= 64 + 21,
		.handler	= &s5p4418_irq_handler_gpioa[21],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA22",
		.no			= 64 + 22,
		.handler	= &s5p4418_irq_handler_gpioa[22],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA23",
		.no			= 64 + 23,
		.handler	= &s5p4418_irq_handler_gpioa[23],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA24",
		.no			= 64 + 24,
		.handler	= &s5p4418_irq_handler_gpioa[24],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA25",
		.no			= 64 + 25,
		.handler	= &s5p4418_irq_handler_gpioa[25],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA26",
		.no			= 64 + 26,
		.handler	= &s5p4418_irq_handler_gpioa[26],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA27",
		.no			= 64 + 27,
		.handler	= &s5p4418_irq_handler_gpioa[27],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA28",
		.no			= 64 + 28,
		.handler	= &s5p4418_irq_handler_gpioa[28],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA29",
		.no			= 64 + 29,
		.handler	= &s5p4418_irq_handler_gpioa[29],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA30",
		.no			= 64 + 30,
		.handler	= &s5p4418_irq_handler_gpioa[30],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOA31",
		.no			= 64 + 31,
		.handler	= &s5p4418_irq_handler_gpioa[31],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	},

	/* GPIOB */
	{
		.name		= "GPIOB0",
		.no			= 96 + 0,
		.handler	= &s5p4418_irq_handler_gpiob[0],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB1",
		.no			= 96 + 1,
		.handler	= &s5p4418_irq_handler_gpiob[1],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB2",
		.no			= 96 + 2,
		.handler	= &s5p4418_irq_handler_gpiob[2],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB3",
		.no			= 96 + 3,
		.handler	= &s5p4418_irq_handler_gpiob[3],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB4",
		.no			= 96 + 4,
		.handler	= &s5p4418_irq_handler_gpiob[4],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB5",
		.no			= 96 + 5,
		.handler	= &s5p4418_irq_handler_gpiob[5],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB6",
		.no			= 96 + 6,
		.handler	= &s5p4418_irq_handler_gpiob[6],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB7",
		.no			= 96 + 7,
		.handler	= &s5p4418_irq_handler_gpiob[7],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB8",
		.no			= 96 + 8,
		.handler	= &s5p4418_irq_handler_gpiob[8],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB9",
		.no			= 96 + 9,
		.handler	= &s5p4418_irq_handler_gpiob[9],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB10",
		.no			= 96 + 10,
		.handler	= &s5p4418_irq_handler_gpiob[10],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB11",
		.no			= 96 + 11,
		.handler	= &s5p4418_irq_handler_gpiob[11],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB12",
		.no			= 96 + 12,
		.handler	= &s5p4418_irq_handler_gpiob[12],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB13",
		.no			= 96 + 13,
		.handler	= &s5p4418_irq_handler_gpiob[13],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB14",
		.no			= 96 + 14,
		.handler	= &s5p4418_irq_handler_gpiob[14],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB15",
		.no			= 96 + 15,
		.handler	= &s5p4418_irq_handler_gpiob[15],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB16",
		.no			= 96 + 16,
		.handler	= &s5p4418_irq_handler_gpiob[16],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB17",
		.no			= 96 + 17,
		.handler	= &s5p4418_irq_handler_gpiob[17],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB18",
		.no			= 96 + 18,
		.handler	= &s5p4418_irq_handler_gpiob[18],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB19",
		.no			= 96 + 19,
		.handler	= &s5p4418_irq_handler_gpiob[19],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB20",
		.no			= 96 + 20,
		.handler	= &s5p4418_irq_handler_gpiob[20],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB21",
		.no			= 96 + 21,
		.handler	= &s5p4418_irq_handler_gpiob[21],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB22",
		.no			= 96 + 22,
		.handler	= &s5p4418_irq_handler_gpiob[22],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB23",
		.no			= 96 + 23,
		.handler	= &s5p4418_irq_handler_gpiob[23],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB24",
		.no			= 96 + 24,
		.handler	= &s5p4418_irq_handler_gpiob[24],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB25",
		.no			= 96 + 25,
		.handler	= &s5p4418_irq_handler_gpiob[25],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB26",
		.no			= 96 + 26,
		.handler	= &s5p4418_irq_handler_gpiob[26],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB27",
		.no			= 96 + 27,
		.handler	= &s5p4418_irq_handler_gpiob[27],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB28",
		.no			= 96 + 28,
		.handler	= &s5p4418_irq_handler_gpiob[28],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB29",
		.no			= 96 + 29,
		.handler	= &s5p4418_irq_handler_gpiob[29],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB30",
		.no			= 96 + 30,
		.handler	= &s5p4418_irq_handler_gpiob[30],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOB31",
		.no			= 96 + 31,
		.handler	= &s5p4418_irq_handler_gpiob[31],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	},

	/* GPIOC */
	{
		.name		= "GPIOC0",
		.no			= 128 + 0,
		.handler	= &s5p4418_irq_handler_gpioc[0],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC1",
		.no			= 128 + 1,
		.handler	= &s5p4418_irq_handler_gpioc[1],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC2",
		.no			= 128 + 2,
		.handler	= &s5p4418_irq_handler_gpioc[2],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC3",
		.no			= 128 + 3,
		.handler	= &s5p4418_irq_handler_gpioc[3],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC4",
		.no			= 128 + 4,
		.handler	= &s5p4418_irq_handler_gpioc[4],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC5",
		.no			= 128 + 5,
		.handler	= &s5p4418_irq_handler_gpioc[5],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC6",
		.no			= 128 + 6,
		.handler	= &s5p4418_irq_handler_gpioc[6],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC7",
		.no			= 128 + 7,
		.handler	= &s5p4418_irq_handler_gpioc[7],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC8",
		.no			= 128 + 8,
		.handler	= &s5p4418_irq_handler_gpioc[8],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC9",
		.no			= 128 + 9,
		.handler	= &s5p4418_irq_handler_gpioc[9],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC10",
		.no			= 128 + 10,
		.handler	= &s5p4418_irq_handler_gpioc[10],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC11",
		.no			= 128 + 11,
		.handler	= &s5p4418_irq_handler_gpioc[11],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC12",
		.no			= 128 + 12,
		.handler	= &s5p4418_irq_handler_gpioc[12],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC13",
		.no			= 128 + 13,
		.handler	= &s5p4418_irq_handler_gpioc[13],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC14",
		.no			= 128 + 14,
		.handler	= &s5p4418_irq_handler_gpioc[14],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC15",
		.no			= 128 + 15,
		.handler	= &s5p4418_irq_handler_gpioc[15],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC16",
		.no			= 128 + 16,
		.handler	= &s5p4418_irq_handler_gpioc[16],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC17",
		.no			= 128 + 17,
		.handler	= &s5p4418_irq_handler_gpioc[17],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC18",
		.no			= 128 + 18,
		.handler	= &s5p4418_irq_handler_gpioc[18],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC19",
		.no			= 128 + 19,
		.handler	= &s5p4418_irq_handler_gpioc[19],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC20",
		.no			= 128 + 20,
		.handler	= &s5p4418_irq_handler_gpioc[20],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC21",
		.no			= 128 + 21,
		.handler	= &s5p4418_irq_handler_gpioc[21],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC22",
		.no			= 128 + 22,
		.handler	= &s5p4418_irq_handler_gpioc[22],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC23",
		.no			= 128 + 23,
		.handler	= &s5p4418_irq_handler_gpioc[23],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC24",
		.no			= 128 + 24,
		.handler	= &s5p4418_irq_handler_gpioc[24],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC25",
		.no			= 128 + 25,
		.handler	= &s5p4418_irq_handler_gpioc[25],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC26",
		.no			= 128 + 26,
		.handler	= &s5p4418_irq_handler_gpioc[26],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC27",
		.no			= 128 + 27,
		.handler	= &s5p4418_irq_handler_gpioc[27],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC28",
		.no			= 128 + 28,
		.handler	= &s5p4418_irq_handler_gpioc[28],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC29",
		.no			= 128 + 29,
		.handler	= &s5p4418_irq_handler_gpioc[29],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC30",
		.no			= 128 + 30,
		.handler	= &s5p4418_irq_handler_gpioc[30],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOC31",
		.no			= 128 + 31,
		.handler	= &s5p4418_irq_handler_gpioc[31],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	},

	/* GPIOD */
	{
		.name		= "GPIOD0",
		.no			= 160 + 0,
		.handler	= &s5p4418_irq_handler_gpiod[0],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD1",
		.no			= 160 + 1,
		.handler	= &s5p4418_irq_handler_gpiod[1],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD2",
		.no			= 160 + 2,
		.handler	= &s5p4418_irq_handler_gpiod[2],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD3",
		.no			= 160 + 3,
		.handler	= &s5p4418_irq_handler_gpiod[3],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD4",
		.no			= 160 + 4,
		.handler	= &s5p4418_irq_handler_gpiod[4],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD5",
		.no			= 160 + 5,
		.handler	= &s5p4418_irq_handler_gpiod[5],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD6",
		.no			= 160 + 6,
		.handler	= &s5p4418_irq_handler_gpiod[6],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD7",
		.no			= 160 + 7,
		.handler	= &s5p4418_irq_handler_gpiod[7],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD8",
		.no			= 160 + 8,
		.handler	= &s5p4418_irq_handler_gpiod[8],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD9",
		.no			= 160 + 9,
		.handler	= &s5p4418_irq_handler_gpiod[9],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD10",
		.no			= 160 + 10,
		.handler	= &s5p4418_irq_handler_gpiod[10],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD11",
		.no			= 160 + 11,
		.handler	= &s5p4418_irq_handler_gpiod[11],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD12",
		.no			= 160 + 12,
		.handler	= &s5p4418_irq_handler_gpiod[12],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD13",
		.no			= 160 + 13,
		.handler	= &s5p4418_irq_handler_gpiod[13],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD14",
		.no			= 160 + 14,
		.handler	= &s5p4418_irq_handler_gpiod[14],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD15",
		.no			= 160 + 15,
		.handler	= &s5p4418_irq_handler_gpiod[15],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD16",
		.no			= 160 + 16,
		.handler	= &s5p4418_irq_handler_gpiod[16],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD17",
		.no			= 160 + 17,
		.handler	= &s5p4418_irq_handler_gpiod[17],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD18",
		.no			= 160 + 18,
		.handler	= &s5p4418_irq_handler_gpiod[18],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD19",
		.no			= 160 + 19,
		.handler	= &s5p4418_irq_handler_gpiod[19],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD20",
		.no			= 160 + 20,
		.handler	= &s5p4418_irq_handler_gpiod[20],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD21",
		.no			= 160 + 21,
		.handler	= &s5p4418_irq_handler_gpiod[21],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD22",
		.no			= 160 + 22,
		.handler	= &s5p4418_irq_handler_gpiod[22],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD23",
		.no			= 160 + 23,
		.handler	= &s5p4418_irq_handler_gpiod[23],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD24",
		.no			= 160 + 24,
		.handler	= &s5p4418_irq_handler_gpiod[24],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD25",
		.no			= 160 + 25,
		.handler	= &s5p4418_irq_handler_gpiod[25],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD26",
		.no			= 160 + 26,
		.handler	= &s5p4418_irq_handler_gpiod[26],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD27",
		.no			= 160 + 27,
		.handler	= &s5p4418_irq_handler_gpiod[27],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD28",
		.no			= 160 + 28,
		.handler	= &s5p4418_irq_handler_gpiod[28],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD29",
		.no			= 160 + 29,
		.handler	= &s5p4418_irq_handler_gpiod[29],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD30",
		.no			= 160 + 30,
		.handler	= &s5p4418_irq_handler_gpiod[30],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOD31",
		.no			= 160 + 31,
		.handler	= &s5p4418_irq_handler_gpiod[31],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	},

	/* GPIOE */
	{
		.name		= "GPIOE0",
		.no			= 192 + 0,
		.handler	= &s5p4418_irq_handler_gpioe[0],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE1",
		.no			= 192 + 1,
		.handler	= &s5p4418_irq_handler_gpioe[1],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE2",
		.no			= 192 + 2,
		.handler	= &s5p4418_irq_handler_gpioe[2],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE3",
		.no			= 192 + 3,
		.handler	= &s5p4418_irq_handler_gpioe[3],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE4",
		.no			= 192 + 4,
		.handler	= &s5p4418_irq_handler_gpioe[4],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE5",
		.no			= 192 + 5,
		.handler	= &s5p4418_irq_handler_gpioe[5],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE6",
		.no			= 192 + 6,
		.handler	= &s5p4418_irq_handler_gpioe[6],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE7",
		.no			= 192 + 7,
		.handler	= &s5p4418_irq_handler_gpioe[7],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE8",
		.no			= 192 + 8,
		.handler	= &s5p4418_irq_handler_gpioe[8],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE9",
		.no			= 192 + 9,
		.handler	= &s5p4418_irq_handler_gpioe[9],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE10",
		.no			= 192 + 10,
		.handler	= &s5p4418_irq_handler_gpioe[10],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE11",
		.no			= 192 + 11,
		.handler	= &s5p4418_irq_handler_gpioe[11],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE12",
		.no			= 192 + 12,
		.handler	= &s5p4418_irq_handler_gpioe[12],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE13",
		.no			= 192 + 13,
		.handler	= &s5p4418_irq_handler_gpioe[13],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE14",
		.no			= 192 + 14,
		.handler	= &s5p4418_irq_handler_gpioe[14],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE15",
		.no			= 192 + 15,
		.handler	= &s5p4418_irq_handler_gpioe[15],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE16",
		.no			= 192 + 16,
		.handler	= &s5p4418_irq_handler_gpioe[16],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE17",
		.no			= 192 + 17,
		.handler	= &s5p4418_irq_handler_gpioe[17],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE18",
		.no			= 192 + 18,
		.handler	= &s5p4418_irq_handler_gpioe[18],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE19",
		.no			= 192 + 19,
		.handler	= &s5p4418_irq_handler_gpioe[19],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE20",
		.no			= 192 + 20,
		.handler	= &s5p4418_irq_handler_gpioe[20],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE21",
		.no			= 192 + 21,
		.handler	= &s5p4418_irq_handler_gpioe[21],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE22",
		.no			= 192 + 22,
		.handler	= &s5p4418_irq_handler_gpioe[22],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE23",
		.no			= 192 + 23,
		.handler	= &s5p4418_irq_handler_gpioe[23],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE24",
		.no			= 192 + 24,
		.handler	= &s5p4418_irq_handler_gpioe[24],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE25",
		.no			= 192 + 25,
		.handler	= &s5p4418_irq_handler_gpioe[25],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE26",
		.no			= 192 + 26,
		.handler	= &s5p4418_irq_handler_gpioe[26],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE27",
		.no			= 192 + 27,
		.handler	= &s5p4418_irq_handler_gpioe[27],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE28",
		.no			= 192 + 28,
		.handler	= &s5p4418_irq_handler_gpioe[28],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE29",
		.no			= 192 + 29,
		.handler	= &s5p4418_irq_handler_gpioe[29],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE30",
		.no			= 192 + 30,
		.handler	= &s5p4418_irq_handler_gpioe[30],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	}, {
		.name		= "GPIOE31",
		.no			= 192 + 31,
		.handler	= &s5p4418_irq_handler_gpioe[31],
		.enable		= s5p4418_irq_enable,
		.disable	= s5p4418_irq_disable,
		.set_type	= s5p4418_irq_set_type,
	},
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

	/*
	 * Install interrupt chains
	 */
	s5p4418_irq_handler[53].func = s5p4418_irq_handler_func_gpioa;
	s5p4418_irq_handler[54].func = s5p4418_irq_handler_func_gpiob;
	s5p4418_irq_handler[55].func = s5p4418_irq_handler_func_gpioc;
	s5p4418_irq_handler[56].func = s5p4418_irq_handler_func_gpiod;
	s5p4418_irq_handler[57].func = s5p4418_irq_handler_func_gpioe;

	/*
	 * Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE vic interrupt.
	 */
	write32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENABLE), (read32(phys_to_virt(S5P4418_VIC1_BASE + VIC_INTENABLE)) | (0x1f << (53 - 32))));

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
