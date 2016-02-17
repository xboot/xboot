/*
 * s5p4418-irq.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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
#include <arm32.h>
#include <s5p4418-irq.h>
#include <s5p4418/reg-gpio.h>
#include <s5p4418/reg-alv.h>
#include <s5p4418/reg-vic.h>

/*
 * Exception handlers from Start.s
 */
extern void irq(void);

struct irqchip_data_t
{
	const char * name;
	int parent;
	int base;
	int nirq;
	physical_addr_t phys;
};

struct irqchip_pdata_t
{
	const char * name;
	int parent;
	int base;
	int nirq;
	virtual_addr_t virt;
};

static struct irqchip_data_t datas[] = {
	{
		.name	= "VIC0",
		.parent	= 0,
		.base	= 0,
		.nirq	= 32,
		.phys	= S5P4418_VIC0_BASE,
	}, {
		.name	= "VIC1",
		.parent	= 0,
		.base	= 32,
		.nirq	= 32,
		.phys	= S5P4418_VIC1_BASE,
	}
};

static void irqchip_enable(struct irqchip_t * chip, int offset)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;
	write32(pdat->virt + VIC_INTENABLE, (read32(pdat->virt + VIC_INTENABLE) | (0x1 << offset)));
}

static void irqchip_disable(struct irqchip_t * chip, int offset)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;
	write32(pdat->virt + VIC_INTENCLEAR, (read32(pdat->virt + VIC_INTENCLEAR) | (0x1 << offset)));
}

static void irqchip_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
}

static void irqchip_process(struct irqchip_t * chip)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;
	u32_t vic = read32(pdat->virt + VIC_IRQSTATUS);

	if(vic != 0)
	{
		u32_t offset = __ffs(vic);
		if((offset >= 0) && (offset < chip->nirq))
		{
			(chip->handler[offset].func)(chip->handler[offset].data);
			write32(pdat->virt + VIC_SOFTINTCLEAR, 0x1 << offset);
		}
	}
}

static struct irqchip_data_t gpio_datas[] = {
	{
		.name	= "GPIOA",
		.parent	= S5P4418_SUBIRQ_GPIOA,
		.base	= 64,
		.nirq	= 32,
		.phys	= S5P4418_GPIOA_BASE,
	}, {
		.name	= "GPIOB",
		.parent	= S5P4418_SUBIRQ_GPIOB,
		.base	= 96,
		.nirq	= 32,
		.phys	= S5P4418_GPIOB_BASE,
	}, {
		.name	= "GPIOC",
		.parent	= S5P4418_SUBIRQ_GPIOC,
		.base	= 128,
		.nirq	= 32,
		.phys	= S5P4418_GPIOC_BASE,
	}, {
		.name	= "GPIOD",
		.parent	= S5P4418_SUBIRQ_GPIOD,
		.base	= 160,
		.nirq	= 32,
		.phys	= S5P4418_GPIOD_BASE,
	}, {
		.name	= "GPIOE",
		.parent	= S5P4418_SUBIRQ_GPIOE,
		.base	= 192,
		.nirq	= 32,
		.phys	= S5P4418_GPIOE_BASE,
	}
};

static void gpio_irqchip_enable(struct irqchip_t * chip, int offset)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;
	write32(pdat->virt + GPIO_INTENB, (read32(pdat->virt + GPIO_INTENB) | (0x1 << offset)));
}

static void gpio_irqchip_disable(struct irqchip_t * chip, int offset)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;
	write32(pdat->virt + GPIO_INTENB, (read32(pdat->virt + GPIO_INTENB) & ~(0x1 << offset)));
}

static void gpio_irqchip_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;
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

	if(type != IRQ_TYPE_NONE)
	{
		if(offset < 16)
		{
			val = read32(pdat->virt +  + GPIO_DETMODE0);
			val &= ~(0x3 << (offset << 0x1));
			val |= (cfg & 0x3) << (offset << 0x1);
			write32(pdat->virt +  + GPIO_DETMODE0, val);
		}
		else if(offset < 32)
		{
			val = read32(pdat->virt +  + GPIO_DETMODE1);
			val &= ~(0x3 << ((offset - 16) << 0x1));
			val |= (cfg & 0x3) << ((offset - 16) << 0x1);
			write32(pdat->virt +  + GPIO_DETMODE1, val);
		}

		val = read32(pdat->virt +  + GPIO_DETMODEEX);
		if(cfg & 0x4)
			val |= 0x1 << offset;
		else
			val &= ~(0x1 << offset);
		write32(pdat->virt +  + GPIO_DETMODEEX, val);

		val = read32(pdat->virt +  + GPIO_DETENB);
		val |= 0x1 << offset;
		write32(pdat->virt +  + GPIO_DETENB, val);
	}
	else
	{
		val = read32(pdat->virt +  + GPIO_DETENB);
		val &= ~(0x1 << offset);
		write32(pdat->virt +  + GPIO_DETENB, val);
	}
}

static void gpio_irqchip_process(struct irqchip_t * chip)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;
	u32_t det = read32(pdat->virt + GPIO_DET);

	if(det != 0)
	{
		u32_t offset = __ffs(det);
		if((offset >= 0) && (offset < chip->nirq))
		{
			(chip->handler[offset].func)(chip->handler[offset].data);
			write32(pdat->virt + GPIO_DET, (0x1 << offset));
		}
	}
}

static struct irqchip_data_t gpioalv_datas[] = {
	{
		.name	= "GPIOALV",
		.parent	= S5P4418_SUBIRQ_GPIOALV,
		.base	= 224,
		.nirq	= 6,
		.phys	= S5P4418_GPIOALV_BASE,
	}
};

static inline void gpioalv_write_enable(virtual_addr_t virt)
{
	write32(virt + GPIOALV_PWRGATEREG, 0x1);
}

static inline void gpioalv_write_disable(virtual_addr_t virt)
{
	write32(virt + GPIOALV_PWRGATEREG, 0x0);
}

static void gpioalv_irqchip_enable(struct irqchip_t * chip, int offset)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;
	gpioalv_write_enable(pdat->virt);
	write32(pdat->virt + GPIOALV_INTENBSETREG, 0x1 << offset);
	gpioalv_write_disable(pdat->virt);
}

static void gpioalv_irqchip_disable(struct irqchip_t * chip, int offset)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;
	gpioalv_write_enable(pdat->virt);
	write32(pdat->virt + GPIOALV_INTENBRSTREG, 0x1 << offset);
	gpioalv_write_disable(pdat->virt);
}

static void gpioalv_irqchip_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;

	if(type != IRQ_TYPE_NONE)
	{
		gpioalv_write_enable(pdat->virt);
		switch(type)
		{
		case IRQ_TYPE_LEVEL_LOW:
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG0, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG1, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODESETREG2, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG3, 0x1 << offset);
			break;

		case IRQ_TYPE_LEVEL_HIGH:
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG0, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG1, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG2, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODESETREG3, 0x1 << offset);
			break;

		case IRQ_TYPE_EDGE_FALLING:
			write32(pdat->virt + GPIOALV_DETECTMODESETREG0, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG1, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG2, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG3, 0x1 << offset);
			break;

		case IRQ_TYPE_EDGE_RISING:
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG0, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODESETREG1, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG2, 0x1 << offset);
			write32(pdat->virt + GPIOALV_DETECTMODERSTREG3, 0x1 << offset);
			break;

		default:
			break;
		}
		write32(pdat->virt + GPIOALV_DETECTENBSETREG, 0x1 << offset);
		gpioalv_write_disable(pdat->virt);
	}
	else
	{
		gpioalv_write_enable(pdat->virt);
		write32(pdat->virt + GPIOALV_DETECTENBRSTREG, 0x1 << offset);
		gpioalv_write_disable(pdat->virt);
	}
}

static void gpioalv_irqchip_process(struct irqchip_t * chip)
{
	struct irqchip_pdata_t * pdat = (struct irqchip_pdata_t *)chip->priv;
	u32_t det = read32(pdat->virt + GPIOALV_DETECTPENDREG);

	if(det != 0)
	{
		u32_t offset = __ffs(det);
		if((offset >= 0) && (offset < chip->nirq))
		{
			(chip->handler[offset].func)(chip->handler[offset].data);
			gpioalv_write_enable(pdat->virt);
			write32(pdat->virt + GPIOALV_DETECTPENDREG, (0x1 << offset));
			gpioalv_write_disable(pdat->virt);
		}
	}
}

static void vic_controller_init(virtual_addr_t virt)
{
	int i;

	/*
	 * Select irq mode
	 */
	write32(virt + VIC_INTSELECT, 0x00000000);
	/*
	 * Disable all interrupts
	 */
	write32(virt + VIC_INTENABLE, 0x00000000);
	/*
	 * Clear all interrupts
	 */
	write32(virt + VIC_INTENCLEAR, 0xffffffff);
	/*
	 * Clear all irq status
	 */
	write32(virt + VIC_IRQSTATUS, 0x00000000);
	/*
	 * Clear all fiq status
	 */
	write32(virt + VIC_FIQSTATUS, 0x00000000);
	/*
	 * Clear all software interrupts
	 */
	write32(virt + VIC_SOFTINTCLEAR, 0xffffffff);
	/*
	 * Set vic address to zero
	 */
	write32(virt + VIC_ADDRESS, 0x00000000);

	for(i = 0; i < 32; i++)
		write32(virt + VIC_VECPRIORITY0 + 4 * i, 0xf);

	for(i = 0; i < 32; i++)
		write32(virt + VIC_VECTADDR0 + 4 * i, (u32_t)irq);
}

static __init void s5p4418_irqchip_init(void)
{
	struct irqchip_pdata_t * pdat;
	struct irqchip_t * chip;
	int i;

	/*
	 * VIC to core, pass through GIC
	 */
	write32(phys_to_virt(0xf0000100), 0);

	/*
	 * Main interrupt
	 */
	for(i = 0; i < ARRAY_SIZE(datas); i++)
	{
		pdat = malloc(sizeof(struct irqchip_pdata_t));
		if(!pdat)
			continue;

		chip = malloc(sizeof(struct irqchip_t));
		if(!chip)
		{
			free(pdat);
			continue;
		}

		pdat->name = datas[i].name;
		pdat->parent = datas[i].parent;
		pdat->base = datas[i].base;
		pdat->nirq = datas[i].nirq;
		pdat->virt = phys_to_virt(datas[i].phys);

		chip->name = pdat->name;
		chip->base = pdat->base;
		chip->nirq = pdat->nirq;
		chip->handler = malloc(sizeof(struct irq_handler_t) * pdat->nirq);
		chip->enable = irqchip_enable;
		chip->disable = irqchip_disable;
		chip->settype = irqchip_settype;
		chip->process = irqchip_process;
		chip->priv = pdat;

		vic_controller_init(pdat->virt);
		register_irqchip(chip);
	}

	/*
	 * Sub gpio interrupt
	 */
	for(i = 0; i < ARRAY_SIZE(gpio_datas); i++)
	{
		pdat = malloc(sizeof(struct irqchip_pdata_t));
		if(!pdat)
			continue;

		chip = malloc(sizeof(struct irqchip_t));
		if(!chip)
		{
			free(pdat);
			continue;
		}

		pdat->name = gpio_datas[i].name;
		pdat->parent = gpio_datas[i].parent;
		pdat->base = gpio_datas[i].base;
		pdat->nirq = gpio_datas[i].nirq;
		pdat->virt = phys_to_virt(gpio_datas[i].phys);

		chip->name = pdat->name;
		chip->base = pdat->base;
		chip->nirq = pdat->nirq;
		chip->handler = malloc(sizeof(struct irq_handler_t) * pdat->nirq);
		chip->enable = gpio_irqchip_enable;
		chip->disable = gpio_irqchip_disable;
		chip->settype = gpio_irqchip_settype;
		chip->process = gpio_irqchip_process;
		chip->priv = pdat;

		register_sub_irqchip(pdat->parent, chip);
	}

	/*
	 * Sub gpioalv interrupt
	 */
	for(i = 0; i < ARRAY_SIZE(gpioalv_datas); i++)
	{
		pdat = malloc(sizeof(struct irqchip_pdata_t));
		if(!pdat)
			continue;

		chip = malloc(sizeof(struct irqchip_t));
		if(!chip)
		{
			free(pdat);
			continue;
		}

		pdat->name = gpioalv_datas[i].name;
		pdat->parent = gpioalv_datas[i].parent;
		pdat->base = gpioalv_datas[i].base;
		pdat->nirq = gpioalv_datas[i].nirq;
		pdat->virt = phys_to_virt(gpioalv_datas[i].phys);

		chip->name = pdat->name;
		chip->base = pdat->base;
		chip->nirq = pdat->nirq;
		chip->handler = malloc(sizeof(struct irq_handler_t) * pdat->nirq);
		chip->enable = gpioalv_irqchip_enable;
		chip->disable = gpioalv_irqchip_disable;
		chip->settype = gpioalv_irqchip_settype;
		chip->process = gpioalv_irqchip_process;
		chip->priv = pdat;

		register_sub_irqchip(pdat->parent, chip);
	}

	arm32_interrupt_enable();
}
core_initcall(s5p4418_irqchip_init);
