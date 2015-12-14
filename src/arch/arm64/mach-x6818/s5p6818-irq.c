/*
 * s5p6818-irq.c
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
#include <arm64.h>
#include <s5p6818/reg-gpio.h>
#include <s5p6818/reg-alv.h>
#include <s5p6818/reg-gic.h>

static struct irq_handler_t s5p6818_irq_handler[32 + 64];
static struct irq_handler_t s5p6818_irq_handler_gpioa[32];
static struct irq_handler_t s5p6818_irq_handler_gpiob[32];
static struct irq_handler_t s5p6818_irq_handler_gpioc[32];
static struct irq_handler_t s5p6818_irq_handler_gpiod[32];
static struct irq_handler_t s5p6818_irq_handler_gpioe[32];
static struct irq_handler_t s5p6818_irq_handler_gpioalv[6];

static void s5p6818_irq_handler_func_gpioa(void * data)
{
	u32_t det;
	u32_t offset;

	det = read32(phys_to_virt(S5P6818_GPIOA_BASE + GPIO_DET));
	if(det != 0)
	{
		offset = __ffs(det);
		(s5p6818_irq_handler_gpioa[offset].func)(s5p6818_irq_handler_gpioa[offset].data);
		write32(phys_to_virt(S5P6818_GPIOA_BASE + GPIO_DET), (0x1 << offset));
	}
}

static void s5p6818_irq_handler_func_gpiob(void * data)
{
	u32_t det;
	u32_t offset;

	det = read32(phys_to_virt(S5P6818_GPIOB_BASE + GPIO_DET));
	if(det != 0)
	{
		offset = __ffs(det);
		(s5p6818_irq_handler_gpiob[offset].func)(s5p6818_irq_handler_gpiob[offset].data);
		write32(phys_to_virt(S5P6818_GPIOB_BASE + GPIO_DET), (0x1 << offset));
	}
}

static void s5p6818_irq_handler_func_gpioc(void * data)
{
	u32_t det;
	u32_t offset;

	det = read32(phys_to_virt(S5P6818_GPIOC_BASE + GPIO_DET));
	if(det != 0)
	{
		offset = __ffs(det);
		(s5p6818_irq_handler_gpioc[offset].func)(s5p6818_irq_handler_gpioc[offset].data);
		write32(phys_to_virt(S5P6818_GPIOC_BASE + GPIO_DET), (0x1 << offset));
	}
}

static void s5p6818_irq_handler_func_gpiod(void * data)
{
	u32_t det;
	u32_t offset;

	det = read32(phys_to_virt(S5P6818_GPIOD_BASE + GPIO_DET));
	if(det != 0)
	{
		offset = __ffs(det);
		(s5p6818_irq_handler_gpiod[offset].func)(s5p6818_irq_handler_gpiod[offset].data);
		write32(phys_to_virt(S5P6818_GPIOD_BASE + GPIO_DET), (0x1 << offset));
	}
}

static void s5p6818_irq_handler_func_gpioe(void * data)
{
	u32_t det;
	u32_t offset;

	det = read32(phys_to_virt(S5P6818_GPIOE_BASE + GPIO_DET));
	if(det != 0)
	{
		offset = __ffs(det);
		(s5p6818_irq_handler_gpioe[offset].func)(s5p6818_irq_handler_gpioe[offset].data);
		write32(phys_to_virt(S5P6818_GPIOE_BASE + GPIO_DET), (0x1 << offset));
	}
}

static inline void s5p6818_gpio_alv_write_enable(void)
{
	write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_PWRGATEREG), 0x1);
}

static inline void s5p6818_gpio_alv_write_disable(void)
{
	write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_PWRGATEREG), 0x0);
}

static void s5p6818_irq_handler_func_gpioalv(void * data)
{
	u32_t det;
	u32_t offset;

	det = read32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTPENDREG));
	if(det != 0)
	{
		offset = __ffs(det);
		(s5p6818_irq_handler_gpioalv[offset].func)(s5p6818_irq_handler_gpioalv[offset].data);
		s5p6818_gpio_alv_write_enable();
		write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTPENDREG), (0x1 << offset));
		s5p6818_gpio_alv_write_disable();
	}
}

void do_irq(void * regs)
{
	int irq;

	irq = read32(phys_to_virt(S5P6818_GIC_CPU_BASE + CPU_INTACK)) & 0x3ff;
	(s5p6818_irq_handler[irq].func)(s5p6818_irq_handler[irq].data);
	write32(phys_to_virt(S5P6818_GIC_CPU_BASE + CPU_EOI), irq);
}

static void s5p6818_irq_enable(struct irq_t * irq)
{
	int no = irq->no;

	/* SGI */
	if(no < 16)
	{
		write32(phys_to_virt(S5P6818_GIC_DIST_BASE + DIST_ENABLE_SET + (no / 32) * 4), 1 << (no % 32));
	}
	/* PPI */
	else if(no < 32)
	{
		write32(phys_to_virt(S5P6818_GIC_DIST_BASE + DIST_ENABLE_SET + (no / 32) * 4), 1 << (no % 32));
	}
	/* SPI */
	else if(no < 96)
	{
		write32(phys_to_virt(S5P6818_GIC_DIST_BASE + DIST_ENABLE_SET + (no / 32) * 4), 1 << (no % 32));
	}
	/* GPIOA */
	else if(no < 128)
	{
		no = no - 96;
		write32(phys_to_virt(S5P6818_GPIOA_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P6818_GPIOA_BASE + GPIO_INTENB)) | (0x1 << no)));
	}
	/* GPIOB */
	else if(no < 160)
	{
		no = no - 128;
		write32(phys_to_virt(S5P6818_GPIOB_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P6818_GPIOB_BASE + GPIO_INTENB)) | (0x1 << no)));
	}
	/* GPIOC */
	else if(no < 192)
	{
		no = no - 160;
		write32(phys_to_virt(S5P6818_GPIOC_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P6818_GPIOC_BASE + GPIO_INTENB)) | (0x1 << no)));
	}
	/* GPIOD */
	else if(no < 224)
	{
		no = no - 192;
		write32(phys_to_virt(S5P6818_GPIOD_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P6818_GPIOD_BASE + GPIO_INTENB)) | (0x1 << no)));
	}
	/* GPIOE */
	else if(no < 256)
	{
		no = no - 224;
		write32(phys_to_virt(S5P6818_GPIOE_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P6818_GPIOE_BASE + GPIO_INTENB)) | (0x1 << no)));
	}
	/* GPIOALV */
	else if(no < 288)
	{
		no = no - 256;
		s5p6818_gpio_alv_write_enable();
		write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_INTENBSETREG), 0x1 << no);
		s5p6818_gpio_alv_write_disable();
	}
}

static void s5p6818_irq_disable(struct irq_t * irq)
{
	int no = irq->no;

	/* SGI */
	if(no < 16)
	{
		write32(phys_to_virt(S5P6818_GIC_DIST_BASE + DIST_ENABLE_CLEAR + (no / 32) * 4), 1 << (no % 32));
	}
	/* PPI */
	else if(no < 32)
	{
		write32(phys_to_virt(S5P6818_GIC_DIST_BASE + DIST_ENABLE_CLEAR + (no / 32) * 4), 1 << (no % 32));
	}
	/* SPI */
	else if(no < 96)
	{
		write32(phys_to_virt(S5P6818_GIC_DIST_BASE + DIST_ENABLE_CLEAR + (no / 32) * 4), 1 << (no % 32));
	}
	/* GPIOA */
	else if(no < 128)
	{
		no = no - 96;
		write32(phys_to_virt(S5P6818_GPIOA_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P6818_GPIOA_BASE + GPIO_INTENB)) & ~(0x1 << no)));
	}
	/* GPIOB */
	else if(no < 160)
	{
		no = no - 128;
		write32(phys_to_virt(S5P6818_GPIOB_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P6818_GPIOB_BASE + GPIO_INTENB)) & ~(0x1 << no)));
	}
	/* GPIOC */
	else if(no < 192)
	{
		no = no - 160;
		write32(phys_to_virt(S5P6818_GPIOC_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P6818_GPIOC_BASE + GPIO_INTENB)) & ~(0x1 << no)));
	}
	/* GPIOD */
	else if(no < 224)
	{
		no = no - 192;
		write32(phys_to_virt(S5P6818_GPIOD_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P6818_GPIOD_BASE + GPIO_INTENB)) & ~(0x1 << no)));
	}
	/* GPIOE */
	else if(no < 256)
	{
		no = no - 224;
		write32(phys_to_virt(S5P6818_GPIOE_BASE + GPIO_INTENB), (read32(phys_to_virt(S5P6818_GPIOE_BASE + GPIO_INTENB)) & ~(0x1 << no)));
	}
	/* GPIOALV */
	else if(no < 288)
	{
		no = no - 256;
		s5p6818_gpio_alv_write_enable();
		write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_INTENBRSTREG), 0x1 << no);
		s5p6818_gpio_alv_write_disable();
	}
}

static void s5p6818_irq_set_type(struct irq_t * irq, enum irq_type_t type)
{
	int no = irq->no;
	u32_t val, cfg = 0x0;

	switch(type)
	{
	case IRQ_TYPE_NONE:
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

	/* SGI */
	if(no < 16)
	{
	}
	/* PPI */
	else if(no < 32)
	{
	}
	/* SPI */
	else if(no < 96)
	{
	}
	/* GPIOA */
	else if(no < 128)
	{
		no = no - 96;
		if(type != IRQ_TYPE_NONE)
		{
			if(no < 16)
			{
				val = read32(phys_to_virt(S5P6818_GPIOA_BASE + GPIO_DETMODE0));
				val &= ~(0x3 << (no << 0x1));
				val |= (cfg & 0x3) << (no << 0x1);
				write32(phys_to_virt(S5P6818_GPIOA_BASE + GPIO_DETMODE0), val);
			}
			else if(no < 32)
			{
				val = read32(phys_to_virt(S5P6818_GPIOA_BASE + GPIO_DETMODE1));
				val &= ~(0x3 << ((no - 16) << 0x1));
				val |= (cfg & 0x3) << ((no - 16) << 0x1);
				write32(phys_to_virt(S5P6818_GPIOA_BASE + GPIO_DETMODE1), val);
			}

			val = read32(phys_to_virt(S5P6818_GPIOA_BASE + GPIO_DETMODEEX));
			if(cfg & 0x4)
				val |= 0x1 << no;
			else
				val &= ~(0x1 << no);
			write32(phys_to_virt(S5P6818_GPIOA_BASE + GPIO_DETMODEEX), val);

			val = read32(phys_to_virt(S5P6818_GPIOA_BASE + GPIO_DETENB));
			val |= 0x1 << no;
			write32(phys_to_virt(S5P6818_GPIOA_BASE + GPIO_DETENB), val);
		}
		else
		{
			val = read32(phys_to_virt(S5P6818_GPIOA_BASE + GPIO_DETENB));
			val &= ~(0x1 << no);
			write32(phys_to_virt(S5P6818_GPIOA_BASE + GPIO_DETENB), val);
		}
	}
	/* GPIOB */
	else if(no < 160)
	{
		no = no - 128;
		if(type != IRQ_TYPE_NONE)
		{
			if(no < 16)
			{
				val = read32(phys_to_virt(S5P6818_GPIOB_BASE + GPIO_DETMODE0));
				val &= ~(0x3 << (no << 0x1));
				val |= (cfg & 0x3) << (no << 0x1);
				write32(phys_to_virt(S5P6818_GPIOB_BASE + GPIO_DETMODE0), val);
			}
			else if(no < 32)
			{
				val = read32(phys_to_virt(S5P6818_GPIOB_BASE + GPIO_DETMODE1));
				val &= ~(0x3 << ((no - 16) << 0x1));
				val |= (cfg & 0x3) << ((no - 16) << 0x1);
				write32(phys_to_virt(S5P6818_GPIOB_BASE + GPIO_DETMODE1), val);
			}

			val = read32(phys_to_virt(S5P6818_GPIOB_BASE + GPIO_DETMODEEX));
			if(cfg & 0x4)
				val |= 0x1 << no;
			else
				val &= ~(0x1 << no);
			write32(phys_to_virt(S5P6818_GPIOB_BASE + GPIO_DETMODEEX), val);

			val = read32(phys_to_virt(S5P6818_GPIOB_BASE + GPIO_DETENB));
			val |= 0x1 << no;
			write32(phys_to_virt(S5P6818_GPIOB_BASE + GPIO_DETENB), val);
		}
		else
		{
			val = read32(phys_to_virt(S5P6818_GPIOB_BASE + GPIO_DETENB));
			val &= ~(0x1 << no);
			write32(phys_to_virt(S5P6818_GPIOB_BASE + GPIO_DETENB), val);
		}
	}
	/* GPIOC */
	else if(no < 192)
	{
		no = no - 160;
		if(type != IRQ_TYPE_NONE)
		{
			if(no < 16)
			{
				val = read32(phys_to_virt(S5P6818_GPIOC_BASE + GPIO_DETMODE0));
				val &= ~(0x3 << (no << 0x1));
				val |= (cfg & 0x3) << (no << 0x1);
				write32(phys_to_virt(S5P6818_GPIOC_BASE + GPIO_DETMODE0), val);
			}
			else if(no < 32)
			{
				val = read32(phys_to_virt(S5P6818_GPIOC_BASE + GPIO_DETMODE1));
				val &= ~(0x3 << ((no - 16) << 0x1));
				val |= (cfg & 0x3) << ((no - 16) << 0x1);
				write32(phys_to_virt(S5P6818_GPIOC_BASE + GPIO_DETMODE1), val);
			}

			val = read32(phys_to_virt(S5P6818_GPIOC_BASE + GPIO_DETMODEEX));
			if(cfg & 0x4)
				val |= 0x1 << no;
			else
				val &= ~(0x1 << no);
			write32(phys_to_virt(S5P6818_GPIOC_BASE + GPIO_DETMODEEX), val);

			val = read32(phys_to_virt(S5P6818_GPIOC_BASE + GPIO_DETENB));
			val |= 0x1 << no;
			write32(phys_to_virt(S5P6818_GPIOC_BASE + GPIO_DETENB), val);
		}
		else
		{
			val = read32(phys_to_virt(S5P6818_GPIOC_BASE + GPIO_DETENB));
			val &= ~(0x1 << no);
			write32(phys_to_virt(S5P6818_GPIOC_BASE + GPIO_DETENB), val);
		}
	}
	/* GPIOD */
	else if(no < 224)
	{
		no = no - 192;
		if(type != IRQ_TYPE_NONE)
		{
			if(no < 16)
			{
				val = read32(phys_to_virt(S5P6818_GPIOD_BASE + GPIO_DETMODE0));
				val &= ~(0x3 << (no << 0x1));
				val |= (cfg & 0x3) << (no << 0x1);
				write32(phys_to_virt(S5P6818_GPIOD_BASE + GPIO_DETMODE0), val);
			}
			else if(no < 32)
			{
				val = read32(phys_to_virt(S5P6818_GPIOD_BASE + GPIO_DETMODE1));
				val &= ~(0x3 << ((no - 16) << 0x1));
				val |= (cfg & 0x3) << ((no - 16) << 0x1);
				write32(phys_to_virt(S5P6818_GPIOD_BASE + GPIO_DETMODE1), val);
			}

			val = read32(phys_to_virt(S5P6818_GPIOD_BASE + GPIO_DETMODEEX));
			if(cfg & 0x4)
				val |= 0x1 << no;
			else
				val &= ~(0x1 << no);
			write32(phys_to_virt(S5P6818_GPIOD_BASE + GPIO_DETMODEEX), val);

			val = read32(phys_to_virt(S5P6818_GPIOD_BASE + GPIO_DETENB));
			val |= 0x1 << no;
			write32(phys_to_virt(S5P6818_GPIOD_BASE + GPIO_DETENB), val);
		}
		else
		{
			val = read32(phys_to_virt(S5P6818_GPIOD_BASE + GPIO_DETENB));
			val &= ~(0x1 << no);
			write32(phys_to_virt(S5P6818_GPIOD_BASE + GPIO_DETENB), val);
		}
	}
	/* GPIOE */
	else if(no < 256)
	{
		no = no - 224;
		if(type != IRQ_TYPE_NONE)
		{
			if(no < 16)
			{
				val = read32(phys_to_virt(S5P6818_GPIOE_BASE + GPIO_DETMODE0));
				val &= ~(0x3 << (no << 0x1));
				val |= (cfg & 0x3) << (no << 0x1);
				write32(phys_to_virt(S5P6818_GPIOE_BASE + GPIO_DETMODE0), val);
			}
			else if(no < 32)
			{
				val = read32(phys_to_virt(S5P6818_GPIOE_BASE + GPIO_DETMODE1));
				val &= ~(0x3 << ((no - 16) << 0x1));
				val |= (cfg & 0x3) << ((no - 16) << 0x1);
				write32(phys_to_virt(S5P6818_GPIOE_BASE + GPIO_DETMODE1), val);
			}

			val = read32(phys_to_virt(S5P6818_GPIOE_BASE + GPIO_DETMODEEX));
			if(cfg & 0x4)
				val |= 0x1 << no;
			else
				val &= ~(0x1 << no);
			write32(phys_to_virt(S5P6818_GPIOE_BASE + GPIO_DETMODEEX), val);

			val = read32(phys_to_virt(S5P6818_GPIOE_BASE + GPIO_DETENB));
			val |= 0x1 << no;
			write32(phys_to_virt(S5P6818_GPIOE_BASE + GPIO_DETENB), val);
		}
		else
		{
			val = read32(phys_to_virt(S5P6818_GPIOE_BASE + GPIO_DETENB));
			val &= ~(0x1 << no);
			write32(phys_to_virt(S5P6818_GPIOE_BASE + GPIO_DETENB), val);
		}
	}
	/* GPIOALV */
	else if(no < 288)
	{
		no = no - 256;
		if(type != IRQ_TYPE_NONE)
		{
			s5p6818_gpio_alv_write_enable();
			switch(type)
			{
			case IRQ_TYPE_LEVEL_LOW:
				write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTMODERSTREG0), 0x1 << no);
				write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTMODERSTREG1), 0x1 << no);
				write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTMODESETREG2), 0x1 << no);
				write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTMODERSTREG3), 0x1 << no);
				break;

			case IRQ_TYPE_LEVEL_HIGH:
				write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTMODERSTREG0), 0x1 << no);
				write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTMODERSTREG1), 0x1 << no);
				write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTMODERSTREG2), 0x1 << no);
				write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTMODESETREG3), 0x1 << no);
				break;

			case IRQ_TYPE_EDGE_FALLING:
				write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTMODESETREG0), 0x1 << no);
				write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTMODERSTREG1), 0x1 << no);
				write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTMODERSTREG2), 0x1 << no);
				write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTMODERSTREG3), 0x1 << no);
				break;

			case IRQ_TYPE_EDGE_RISING:
				write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTMODERSTREG0), 0x1 << no);
				write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTMODESETREG1), 0x1 << no);
				write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTMODERSTREG2), 0x1 << no);
				write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTMODERSTREG3), 0x1 << no);
				break;

			default:
				break;
			}
			write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTENBSETREG), 0x1 << no);
			s5p6818_gpio_alv_write_disable();
		}
		else
		{
			s5p6818_gpio_alv_write_enable();
			write32(phys_to_virt(S5P6818_GPIOALV_BASE + GPIOALV_DETECTENBRSTREG), 0x1 << no);
			s5p6818_gpio_alv_write_disable();
		}
	}
}

static struct irq_t s5p6818_irqs[] = {
	/* PPI */
	{
		.name		= "VMI",
		.no			= 25,
		.handler	= &s5p6818_irq_handler[25],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "HYPTIMER",
		.no			= 26,
		.handler	= &s5p6818_irq_handler[26],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "VIRTIMER",
		.no			= 27,
		.handler	= &s5p6818_irq_handler[27],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "LEGACYFIQ",
		.no			= 28,
		.handler	= &s5p6818_irq_handler[28],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "STIMER",
		.no			= 29,
		.handler	= &s5p6818_irq_handler[29],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "NSTIMER",
		.no			= 30,
		.handler	= &s5p6818_irq_handler[30],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "LEGACYIRQ",
		.no			= 31,
		.handler	= &s5p6818_irq_handler[31],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	},
	/* SPI */
	{
		.name		= "MCUSTOP",
		.no			= 32 + 0,
		.handler	= &s5p6818_irq_handler[32 + 0],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "DMA0",
		.no			= 32 + 1,
		.handler	= &s5p6818_irq_handler[32 + 1],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "DMA1",
		.no			= 32 + 2,
		.handler	= &s5p6818_irq_handler[32 + 2],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "CLKPWR0",
		.no			= 32 + 3,
		.handler	= &s5p6818_irq_handler[32 + 3],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "CLKPWR2",
		.no			= 32 + 5,
		.handler	= &s5p6818_irq_handler[32 + 5],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "UART1",
		.no			= 32 + 6,
		.handler	= &s5p6818_irq_handler[32 + 6],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "UART0",
		.no			= 32 + 7,
		.handler	= &s5p6818_irq_handler[32 + 7],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "UART2",
		.no			= 32 + 8,
		.handler	= &s5p6818_irq_handler[32 + 8],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "UART3",
		.no			= 32 + 9,
		.handler	= &s5p6818_irq_handler[32 + 9],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "UART4",
		.no			= 32 + 10,
		.handler	= &s5p6818_irq_handler[32 + 10],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "UART5",
		.no			= 32 + 11,
		.handler	= &s5p6818_irq_handler[32 + 11],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "SSP0",
		.no			= 32 + 12,
		.handler	= &s5p6818_irq_handler[32 + 12],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "SSP1",
		.no			= 32 + 13,
		.handler	= &s5p6818_irq_handler[32 + 13],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "SSP2",
		.no			= 32 + 14,
		.handler	= &s5p6818_irq_handler[32 + 14],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "I2C0",
		.no			= 32 + 15,
		.handler	= &s5p6818_irq_handler[32 + 15],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "I2C1",
		.no			= 32 + 16,
		.handler	= &s5p6818_irq_handler[32 + 16],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "I2C2",
		.no			= 32 + 17,
		.handler	= &s5p6818_irq_handler[32 + 17],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "DEINTERLACE",
		.no			= 32 + 18,
		.handler	= &s5p6818_irq_handler[32 + 18],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "SCALER",
		.no			= 32 + 19,
		.handler	= &s5p6818_irq_handler[32 + 19],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "AC97",
		.no			= 32 + 20,
		.handler	= &s5p6818_irq_handler[32 + 20],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "SPDIFRX",
		.no			= 32 + 21,
		.handler	= &s5p6818_irq_handler[32 + 21],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "SPDIFTX",
		.no			= 32 + 22,
		.handler	= &s5p6818_irq_handler[32 + 22],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "TIMER0",
		.no			= 32 + 23,
		.handler	= &s5p6818_irq_handler[32 + 23],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "TIMER1",
		.no			= 32 + 24,
		.handler	= &s5p6818_irq_handler[32 + 24],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "TIMER2",
		.no			= 32 + 25,
		.handler	= &s5p6818_irq_handler[32 + 25],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "TIMER3",
		.no			= 32 + 26,
		.handler	= &s5p6818_irq_handler[32 + 26],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "PWM0",
		.no			= 32 + 27,
		.handler	= &s5p6818_irq_handler[32 + 27],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "PWM1",
		.no			= 32 + 28,
		.handler	= &s5p6818_irq_handler[32 + 28],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "PWM2",
		.no			= 32 + 29,
		.handler	= &s5p6818_irq_handler[32 + 29],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "PWM3",
		.no			= 32 + 30,
		.handler	= &s5p6818_irq_handler[32 + 30],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "WDT",
		.no			= 32 + 31,
		.handler	= &s5p6818_irq_handler[32 + 31],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "MPEGTSI",
		.no			= 32 + 32,
		.handler	= &s5p6818_irq_handler[32 + 32],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "DISPLAYTOP0",
		.no			= 32 + 33,
		.handler	= &s5p6818_irq_handler[32 + 33],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "DISPLAYTOP1",
		.no			= 32 + 34,
		.handler	= &s5p6818_irq_handler[32 + 34],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "DISPLAYTOP2",
		.no			= 32 + 35,
		.handler	= &s5p6818_irq_handler[32 + 35],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "DISPLAYTOP3",
		.no			= 32 + 36,
		.handler	= &s5p6818_irq_handler[32 + 36],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "VIP0",
		.no			= 32 + 37,
		.handler	= &s5p6818_irq_handler[32 + 37],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "VIP1",
		.no			= 32 + 38,
		.handler	= &s5p6818_irq_handler[32 + 38],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "MIPI",
		.no			= 32 + 39,
		.handler	= &s5p6818_irq_handler[32 + 39],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "3DGPU",
		.no			= 32 + 40,
		.handler	= &s5p6818_irq_handler[32 + 40],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "ADC",
		.no			= 32 + 41,
		.handler	= &s5p6818_irq_handler[32 + 41],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "PPM",
		.no			= 32 + 42,
		.handler	= &s5p6818_irq_handler[32 + 42],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "SDMMC0",
		.no			= 32 + 43,
		.handler	= &s5p6818_irq_handler[32 + 43],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "SDMMC1",
		.no			= 32 + 44,
		.handler	= &s5p6818_irq_handler[32 + 44],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "SDMMC2",
		.no			= 32 + 45,
		.handler	= &s5p6818_irq_handler[32 + 45],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "CODA9600",
		.no			= 32 + 46,
		.handler	= &s5p6818_irq_handler[32 + 46],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "CODA9601",
		.no			= 32 + 47,
		.handler	= &s5p6818_irq_handler[32 + 47],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GMAC",
		.no			= 32 + 48,
		.handler	= &s5p6818_irq_handler[32 + 48],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "USB20OTG",
		.no			= 32 + 49,
		.handler	= &s5p6818_irq_handler[32 + 49],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "USB20HOST",
		.no			= 32 + 50,
		.handler	= &s5p6818_irq_handler[32 + 50],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "CRYPTO",
		.no			= 32 + 58,
		.handler	= &s5p6818_irq_handler[32 + 58],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "PDM",
		.no			= 32 + 59,
		.handler	= &s5p6818_irq_handler[32 + 59],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "TMU0",
		.no			= 32 + 60,
		.handler	= &s5p6818_irq_handler[32 + 60],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "TMU1",
		.no			= 32 + 61,
		.handler	= &s5p6818_irq_handler[32 + 61],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	},

	/* GPIOA */
	{
		.name		= "GPIOA0",
		.no			= 96 + 0,
		.handler	= &s5p6818_irq_handler_gpioa[0],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA1",
		.no			= 96 + 1,
		.handler	= &s5p6818_irq_handler_gpioa[1],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA2",
		.no			= 96 + 2,
		.handler	= &s5p6818_irq_handler_gpioa[2],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA3",
		.no			= 96 + 3,
		.handler	= &s5p6818_irq_handler_gpioa[3],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA4",
		.no			= 96 + 4,
		.handler	= &s5p6818_irq_handler_gpioa[4],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA5",
		.no			= 96 + 5,
		.handler	= &s5p6818_irq_handler_gpioa[5],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA6",
		.no			= 96 + 6,
		.handler	= &s5p6818_irq_handler_gpioa[6],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA7",
		.no			= 96 + 7,
		.handler	= &s5p6818_irq_handler_gpioa[7],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA8",
		.no			= 96 + 8,
		.handler	= &s5p6818_irq_handler_gpioa[8],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA9",
		.no			= 96 + 9,
		.handler	= &s5p6818_irq_handler_gpioa[9],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA10",
		.no			= 96 + 10,
		.handler	= &s5p6818_irq_handler_gpioa[10],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA11",
		.no			= 96 + 11,
		.handler	= &s5p6818_irq_handler_gpioa[11],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA12",
		.no			= 96 + 12,
		.handler	= &s5p6818_irq_handler_gpioa[12],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA13",
		.no			= 96 + 13,
		.handler	= &s5p6818_irq_handler_gpioa[13],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA14",
		.no			= 96 + 14,
		.handler	= &s5p6818_irq_handler_gpioa[14],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA15",
		.no			= 96 + 15,
		.handler	= &s5p6818_irq_handler_gpioa[15],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA16",
		.no			= 96 + 16,
		.handler	= &s5p6818_irq_handler_gpioa[16],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA17",
		.no			= 96 + 17,
		.handler	= &s5p6818_irq_handler_gpioa[17],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA18",
		.no			= 96 + 18,
		.handler	= &s5p6818_irq_handler_gpioa[18],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA19",
		.no			= 96 + 19,
		.handler	= &s5p6818_irq_handler_gpioa[19],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA20",
		.no			= 96 + 20,
		.handler	= &s5p6818_irq_handler_gpioa[20],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA21",
		.no			= 96 + 21,
		.handler	= &s5p6818_irq_handler_gpioa[21],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA22",
		.no			= 96 + 22,
		.handler	= &s5p6818_irq_handler_gpioa[22],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA23",
		.no			= 96 + 23,
		.handler	= &s5p6818_irq_handler_gpioa[23],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA24",
		.no			= 96 + 24,
		.handler	= &s5p6818_irq_handler_gpioa[24],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA25",
		.no			= 96 + 25,
		.handler	= &s5p6818_irq_handler_gpioa[25],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA26",
		.no			= 96 + 26,
		.handler	= &s5p6818_irq_handler_gpioa[26],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA27",
		.no			= 96 + 27,
		.handler	= &s5p6818_irq_handler_gpioa[27],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA28",
		.no			= 96 + 28,
		.handler	= &s5p6818_irq_handler_gpioa[28],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA29",
		.no			= 96 + 29,
		.handler	= &s5p6818_irq_handler_gpioa[29],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA30",
		.no			= 96 + 30,
		.handler	= &s5p6818_irq_handler_gpioa[30],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOA31",
		.no			= 96 + 31,
		.handler	= &s5p6818_irq_handler_gpioa[31],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	},

	/* GPIOB */
	{
		.name		= "GPIOB0",
		.no			= 128 + 0,
		.handler	= &s5p6818_irq_handler_gpiob[0],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB1",
		.no			= 128 + 1,
		.handler	= &s5p6818_irq_handler_gpiob[1],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB2",
		.no			= 128 + 2,
		.handler	= &s5p6818_irq_handler_gpiob[2],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB3",
		.no			= 128 + 3,
		.handler	= &s5p6818_irq_handler_gpiob[3],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB4",
		.no			= 128 + 4,
		.handler	= &s5p6818_irq_handler_gpiob[4],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB5",
		.no			= 128 + 5,
		.handler	= &s5p6818_irq_handler_gpiob[5],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB6",
		.no			= 128 + 6,
		.handler	= &s5p6818_irq_handler_gpiob[6],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB7",
		.no			= 128 + 7,
		.handler	= &s5p6818_irq_handler_gpiob[7],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB8",
		.no			= 128 + 8,
		.handler	= &s5p6818_irq_handler_gpiob[8],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB9",
		.no			= 128 + 9,
		.handler	= &s5p6818_irq_handler_gpiob[9],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB10",
		.no			= 128 + 10,
		.handler	= &s5p6818_irq_handler_gpiob[10],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB11",
		.no			= 128 + 11,
		.handler	= &s5p6818_irq_handler_gpiob[11],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB12",
		.no			= 128 + 12,
		.handler	= &s5p6818_irq_handler_gpiob[12],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB13",
		.no			= 128 + 13,
		.handler	= &s5p6818_irq_handler_gpiob[13],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB14",
		.no			= 128 + 14,
		.handler	= &s5p6818_irq_handler_gpiob[14],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB15",
		.no			= 128 + 15,
		.handler	= &s5p6818_irq_handler_gpiob[15],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB16",
		.no			= 128 + 16,
		.handler	= &s5p6818_irq_handler_gpiob[16],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB17",
		.no			= 128 + 17,
		.handler	= &s5p6818_irq_handler_gpiob[17],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB18",
		.no			= 128 + 18,
		.handler	= &s5p6818_irq_handler_gpiob[18],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB19",
		.no			= 128 + 19,
		.handler	= &s5p6818_irq_handler_gpiob[19],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB20",
		.no			= 128 + 20,
		.handler	= &s5p6818_irq_handler_gpiob[20],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB21",
		.no			= 128 + 21,
		.handler	= &s5p6818_irq_handler_gpiob[21],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB22",
		.no			= 128 + 22,
		.handler	= &s5p6818_irq_handler_gpiob[22],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB23",
		.no			= 128 + 23,
		.handler	= &s5p6818_irq_handler_gpiob[23],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB24",
		.no			= 128 + 24,
		.handler	= &s5p6818_irq_handler_gpiob[24],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB25",
		.no			= 128 + 25,
		.handler	= &s5p6818_irq_handler_gpiob[25],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB26",
		.no			= 128 + 26,
		.handler	= &s5p6818_irq_handler_gpiob[26],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB27",
		.no			= 128 + 27,
		.handler	= &s5p6818_irq_handler_gpiob[27],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB28",
		.no			= 128 + 28,
		.handler	= &s5p6818_irq_handler_gpiob[28],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB29",
		.no			= 128 + 29,
		.handler	= &s5p6818_irq_handler_gpiob[29],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB30",
		.no			= 128 + 30,
		.handler	= &s5p6818_irq_handler_gpiob[30],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOB31",
		.no			= 128 + 31,
		.handler	= &s5p6818_irq_handler_gpiob[31],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	},

	/* GPIOC */
	{
		.name		= "GPIOC0",
		.no			= 160 + 0,
		.handler	= &s5p6818_irq_handler_gpioc[0],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC1",
		.no			= 160 + 1,
		.handler	= &s5p6818_irq_handler_gpioc[1],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC2",
		.no			= 160 + 2,
		.handler	= &s5p6818_irq_handler_gpioc[2],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC3",
		.no			= 160 + 3,
		.handler	= &s5p6818_irq_handler_gpioc[3],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC4",
		.no			= 160 + 4,
		.handler	= &s5p6818_irq_handler_gpioc[4],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC5",
		.no			= 160 + 5,
		.handler	= &s5p6818_irq_handler_gpioc[5],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC6",
		.no			= 160 + 6,
		.handler	= &s5p6818_irq_handler_gpioc[6],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC7",
		.no			= 160 + 7,
		.handler	= &s5p6818_irq_handler_gpioc[7],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC8",
		.no			= 160 + 8,
		.handler	= &s5p6818_irq_handler_gpioc[8],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC9",
		.no			= 160 + 9,
		.handler	= &s5p6818_irq_handler_gpioc[9],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC10",
		.no			= 160 + 10,
		.handler	= &s5p6818_irq_handler_gpioc[10],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC11",
		.no			= 160 + 11,
		.handler	= &s5p6818_irq_handler_gpioc[11],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC12",
		.no			= 160 + 12,
		.handler	= &s5p6818_irq_handler_gpioc[12],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC13",
		.no			= 160 + 13,
		.handler	= &s5p6818_irq_handler_gpioc[13],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC14",
		.no			= 160 + 14,
		.handler	= &s5p6818_irq_handler_gpioc[14],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC15",
		.no			= 160 + 15,
		.handler	= &s5p6818_irq_handler_gpioc[15],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC16",
		.no			= 160 + 16,
		.handler	= &s5p6818_irq_handler_gpioc[16],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC17",
		.no			= 160 + 17,
		.handler	= &s5p6818_irq_handler_gpioc[17],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC18",
		.no			= 160 + 18,
		.handler	= &s5p6818_irq_handler_gpioc[18],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC19",
		.no			= 160 + 19,
		.handler	= &s5p6818_irq_handler_gpioc[19],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC20",
		.no			= 160 + 20,
		.handler	= &s5p6818_irq_handler_gpioc[20],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC21",
		.no			= 160 + 21,
		.handler	= &s5p6818_irq_handler_gpioc[21],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC22",
		.no			= 160 + 22,
		.handler	= &s5p6818_irq_handler_gpioc[22],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC23",
		.no			= 160 + 23,
		.handler	= &s5p6818_irq_handler_gpioc[23],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC24",
		.no			= 160 + 24,
		.handler	= &s5p6818_irq_handler_gpioc[24],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC25",
		.no			= 160 + 25,
		.handler	= &s5p6818_irq_handler_gpioc[25],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC26",
		.no			= 160 + 26,
		.handler	= &s5p6818_irq_handler_gpioc[26],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC27",
		.no			= 160 + 27,
		.handler	= &s5p6818_irq_handler_gpioc[27],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC28",
		.no			= 160 + 28,
		.handler	= &s5p6818_irq_handler_gpioc[28],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC29",
		.no			= 160 + 29,
		.handler	= &s5p6818_irq_handler_gpioc[29],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC30",
		.no			= 160 + 30,
		.handler	= &s5p6818_irq_handler_gpioc[30],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOC31",
		.no			= 160 + 31,
		.handler	= &s5p6818_irq_handler_gpioc[31],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	},

	/* GPIOD */
	{
		.name		= "GPIOD0",
		.no			= 192 + 0,
		.handler	= &s5p6818_irq_handler_gpiod[0],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD1",
		.no			= 192 + 1,
		.handler	= &s5p6818_irq_handler_gpiod[1],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD2",
		.no			= 192 + 2,
		.handler	= &s5p6818_irq_handler_gpiod[2],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD3",
		.no			= 192 + 3,
		.handler	= &s5p6818_irq_handler_gpiod[3],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD4",
		.no			= 192 + 4,
		.handler	= &s5p6818_irq_handler_gpiod[4],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD5",
		.no			= 192 + 5,
		.handler	= &s5p6818_irq_handler_gpiod[5],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD6",
		.no			= 192 + 6,
		.handler	= &s5p6818_irq_handler_gpiod[6],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD7",
		.no			= 192 + 7,
		.handler	= &s5p6818_irq_handler_gpiod[7],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD8",
		.no			= 192 + 8,
		.handler	= &s5p6818_irq_handler_gpiod[8],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD9",
		.no			= 192 + 9,
		.handler	= &s5p6818_irq_handler_gpiod[9],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD10",
		.no			= 192 + 10,
		.handler	= &s5p6818_irq_handler_gpiod[10],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD11",
		.no			= 192 + 11,
		.handler	= &s5p6818_irq_handler_gpiod[11],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD12",
		.no			= 192 + 12,
		.handler	= &s5p6818_irq_handler_gpiod[12],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD13",
		.no			= 192 + 13,
		.handler	= &s5p6818_irq_handler_gpiod[13],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD14",
		.no			= 192 + 14,
		.handler	= &s5p6818_irq_handler_gpiod[14],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD15",
		.no			= 192 + 15,
		.handler	= &s5p6818_irq_handler_gpiod[15],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD16",
		.no			= 192 + 16,
		.handler	= &s5p6818_irq_handler_gpiod[16],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD17",
		.no			= 192 + 17,
		.handler	= &s5p6818_irq_handler_gpiod[17],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD18",
		.no			= 192 + 18,
		.handler	= &s5p6818_irq_handler_gpiod[18],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD19",
		.no			= 192 + 19,
		.handler	= &s5p6818_irq_handler_gpiod[19],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD20",
		.no			= 192 + 20,
		.handler	= &s5p6818_irq_handler_gpiod[20],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD21",
		.no			= 192 + 21,
		.handler	= &s5p6818_irq_handler_gpiod[21],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD22",
		.no			= 192 + 22,
		.handler	= &s5p6818_irq_handler_gpiod[22],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD23",
		.no			= 192 + 23,
		.handler	= &s5p6818_irq_handler_gpiod[23],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD24",
		.no			= 192 + 24,
		.handler	= &s5p6818_irq_handler_gpiod[24],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD25",
		.no			= 192 + 25,
		.handler	= &s5p6818_irq_handler_gpiod[25],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD26",
		.no			= 192 + 26,
		.handler	= &s5p6818_irq_handler_gpiod[26],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD27",
		.no			= 192 + 27,
		.handler	= &s5p6818_irq_handler_gpiod[27],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD28",
		.no			= 192 + 28,
		.handler	= &s5p6818_irq_handler_gpiod[28],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD29",
		.no			= 192 + 29,
		.handler	= &s5p6818_irq_handler_gpiod[29],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD30",
		.no			= 192 + 30,
		.handler	= &s5p6818_irq_handler_gpiod[30],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOD31",
		.no			= 192 + 31,
		.handler	= &s5p6818_irq_handler_gpiod[31],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	},

	/* GPIOE */
	{
		.name		= "GPIOE0",
		.no			= 224 + 0,
		.handler	= &s5p6818_irq_handler_gpioe[0],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE1",
		.no			= 224 + 1,
		.handler	= &s5p6818_irq_handler_gpioe[1],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE2",
		.no			= 224 + 2,
		.handler	= &s5p6818_irq_handler_gpioe[2],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE3",
		.no			= 224 + 3,
		.handler	= &s5p6818_irq_handler_gpioe[3],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE4",
		.no			= 224 + 4,
		.handler	= &s5p6818_irq_handler_gpioe[4],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE5",
		.no			= 224 + 5,
		.handler	= &s5p6818_irq_handler_gpioe[5],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE6",
		.no			= 224 + 6,
		.handler	= &s5p6818_irq_handler_gpioe[6],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE7",
		.no			= 224 + 7,
		.handler	= &s5p6818_irq_handler_gpioe[7],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE8",
		.no			= 224 + 8,
		.handler	= &s5p6818_irq_handler_gpioe[8],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE9",
		.no			= 224 + 9,
		.handler	= &s5p6818_irq_handler_gpioe[9],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE10",
		.no			= 224 + 10,
		.handler	= &s5p6818_irq_handler_gpioe[10],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE11",
		.no			= 224 + 11,
		.handler	= &s5p6818_irq_handler_gpioe[11],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE12",
		.no			= 224 + 12,
		.handler	= &s5p6818_irq_handler_gpioe[12],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE13",
		.no			= 224 + 13,
		.handler	= &s5p6818_irq_handler_gpioe[13],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE14",
		.no			= 224 + 14,
		.handler	= &s5p6818_irq_handler_gpioe[14],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE15",
		.no			= 224 + 15,
		.handler	= &s5p6818_irq_handler_gpioe[15],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE16",
		.no			= 224 + 16,
		.handler	= &s5p6818_irq_handler_gpioe[16],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE17",
		.no			= 224 + 17,
		.handler	= &s5p6818_irq_handler_gpioe[17],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE18",
		.no			= 224 + 18,
		.handler	= &s5p6818_irq_handler_gpioe[18],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE19",
		.no			= 224 + 19,
		.handler	= &s5p6818_irq_handler_gpioe[19],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE20",
		.no			= 224 + 20,
		.handler	= &s5p6818_irq_handler_gpioe[20],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE21",
		.no			= 224 + 21,
		.handler	= &s5p6818_irq_handler_gpioe[21],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE22",
		.no			= 224 + 22,
		.handler	= &s5p6818_irq_handler_gpioe[22],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE23",
		.no			= 224 + 23,
		.handler	= &s5p6818_irq_handler_gpioe[23],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE24",
		.no			= 224 + 24,
		.handler	= &s5p6818_irq_handler_gpioe[24],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE25",
		.no			= 224 + 25,
		.handler	= &s5p6818_irq_handler_gpioe[25],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE26",
		.no			= 224 + 26,
		.handler	= &s5p6818_irq_handler_gpioe[26],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE27",
		.no			= 224 + 27,
		.handler	= &s5p6818_irq_handler_gpioe[27],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE28",
		.no			= 224 + 28,
		.handler	= &s5p6818_irq_handler_gpioe[28],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE29",
		.no			= 224 + 29,
		.handler	= &s5p6818_irq_handler_gpioe[29],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE30",
		.no			= 224 + 30,
		.handler	= &s5p6818_irq_handler_gpioe[30],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOE31",
		.no			= 224 + 31,
		.handler	= &s5p6818_irq_handler_gpioe[31],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	},

	/* GPIOALV */
	{
		.name		= "GPIOALV0",
		.no			= 256 + 0,
		.handler	= &s5p6818_irq_handler_gpioalv[0],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOALV1",
		.no			= 256 + 1,
		.handler	= &s5p6818_irq_handler_gpioalv[1],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOALV2",
		.no			= 256 + 2,
		.handler	= &s5p6818_irq_handler_gpioalv[2],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOALV3",
		.no			= 256 + 3,
		.handler	= &s5p6818_irq_handler_gpioalv[3],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOALV4",
		.no			= 256 + 4,
		.handler	= &s5p6818_irq_handler_gpioalv[4],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	}, {
		.name		= "GPIOALV5",
		.no			= 256 + 5,
		.handler	= &s5p6818_irq_handler_gpioalv[5],
		.enable		= s5p6818_irq_enable,
		.disable	= s5p6818_irq_disable,
		.set_type	= s5p6818_irq_set_type,
	},
};

static void gic_dist_init(physical_addr_t dist)
{
	u32_t gic_irqs;
	u32_t cpumask;
	int i;

	write32(phys_to_virt(dist + DIST_CTRL), 0x0);

	/*
	 * Find out how many interrupts are supported.
	 * The GIC only supports up to 1020 interrupt sources.
	 */
	gic_irqs = read32(phys_to_virt(dist + DIST_CTR)) & 0x1f;
	gic_irqs = (gic_irqs + 1) * 32;
	if(gic_irqs > 1020)
		gic_irqs = 1020;

	/*
	 * Set all global interrupts to this CPU only.
	 */
	cpumask = 1 << arm64_smp_processor_id();
	cpumask |= cpumask << 8;
	cpumask |= cpumask << 16;
	for(i = 32; i < gic_irqs; i += 4)
		write32(phys_to_virt(dist + DIST_TARGET + i * 4 / 4), cpumask);

	/*
	 * Set all global interrupts to be level triggered, active low.
	 */
	for(i = 32; i < gic_irqs; i += 16)
		write32(phys_to_virt(dist + DIST_CONFIG + i * 4 / 16), 0);

	/*
	 * Set priority on all global interrupts.
	 */
	for(i = 32; i < gic_irqs; i += 4)
		write32(phys_to_virt(dist + DIST_PRI + i * 4 / 4), 0xa0a0a0a0);

	/*
	 * Disable all interrupts.  Leave the PPI and SGIs alone
	 * as these enables are banked registers.
	 */
	for(i = 32; i < gic_irqs; i += 32)
		write32(phys_to_virt(dist + DIST_ENABLE_CLEAR + i * 4 / 32), 0xffffffff);

	write32(phys_to_virt(dist + DIST_CTRL), 0x1);
}

static void gic_cpu_init(physical_addr_t dist, physical_addr_t cpu)
{
	int i;

	/*
	 * Deal with the banked PPI and SGI interrupts - disable all
	 * PPI interrupts, ensure all SGI interrupts are enabled.
	 */
	write32(phys_to_virt(dist + DIST_ENABLE_CLEAR), 0xffff0000);
	write32(phys_to_virt(dist + DIST_ENABLE_SET), 0x0000ffff);

	/*
	 * Set priority on PPI and SGI interrupts
	 */
	for(i = 0; i < 32; i += 4)
		write32(phys_to_virt(dist + DIST_PRI + i * 4 / 4), 0xa0a0a0a0);

	write32(phys_to_virt(cpu + CPU_PRIMASK), 0xf0);
	write32(phys_to_virt(cpu + CPU_CTRL), 0x1);
}

static __init void s5p6818_irq_init(void)
{
	int i;

	gic_dist_init(S5P6818_GIC_DIST_BASE);
	gic_cpu_init(S5P6818_GIC_DIST_BASE, S5P6818_GIC_CPU_BASE);

	s5p6818_irq_handler[53].func = s5p6818_irq_handler_func_gpioa;
	s5p6818_irq_handler[54].func = s5p6818_irq_handler_func_gpiob;
	s5p6818_irq_handler[55].func = s5p6818_irq_handler_func_gpioc;
	s5p6818_irq_handler[56].func = s5p6818_irq_handler_func_gpiod;
	s5p6818_irq_handler[57].func = s5p6818_irq_handler_func_gpioe;
	s5p6818_irq_handler[4].func = s5p6818_irq_handler_func_gpioalv;

	for(i = 0; i < ARRAY_SIZE(s5p6818_irqs); i++)
	{
		if(irq_register(&s5p6818_irqs[i]))
			LOG("Register irq '%s'", s5p6818_irqs[i].name);
		else
			LOG("Failed to register irq '%s'", s5p6818_irqs[i].name);
	}

	arm64_interrupt_enable();
}

static __exit void s5p6818_irq_exit(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(s5p6818_irqs); i++)
	{
		if(irq_unregister(&s5p6818_irqs[i]))
			LOG("Unregister irq '%s'", s5p6818_irqs[i].name);
		else
			LOG("Failed to unregister irq '%s'", s5p6818_irqs[i].name);
	}

	arm64_interrupt_disable();
}

core_initcall(s5p6818_irq_init);
core_exitcall(s5p6818_irq_exit);
