/*
 * s5p4418-gpio.c
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
#include <s5p4418/reg-gpio.h>
#include <s5p4418-gpio.h>

struct gpiochip_data_t
{
	const char * name;
	int base;
	int ngpio;
	physical_addr_t phys;
};

struct gpiochip_pdata_t
{
	const char * name;
	int base;
	int ngpio;
	virtual_addr_t virt;
};

static struct gpiochip_data_t datas[] = {
	{
		.name	= "GPIOA",
		.base	= S5P4418_GPIOA(0),
		.ngpio	= 32,
		.phys	= S5P4418_GPIOA_BASE,
	}, {
		.name	= "GPIOB",
		.base	= S5P4418_GPIOB(0),
		.ngpio	= 32,
		.phys	= S5P4418_GPIOB_BASE,
	}, {
		.name	= "GPIOC",
		.base	= S5P4418_GPIOC(0),
		.ngpio	= 32,
		.phys	= S5P4418_GPIOC_BASE,
	}, {
		.name	= "GPIOD",
		.base	= S5P4418_GPIOD(0),
		.ngpio	= 32,
		.phys	= S5P4418_GPIOD_BASE,
	}, {
		.name	= "GPIOE",
		.base	= S5P4418_GPIOE(0),
		.ngpio	= 32,
		.phys	= S5P4418_GPIOE_BASE,
	}
};

static void gpiochip_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	if(offset < 16)
	{
		offset = offset << 0x1;
		val = read32(pdat->virt + GPIO_ALTFN0);
		val &= ~(0x3 << offset);
		val |= cfg << offset;
		write32(pdat->virt + GPIO_ALTFN0, val);
	}
	else if(offset < 32)
	{
		offset = (offset - 16) << 0x1;
		val = read32(pdat->virt + GPIO_ALTFN1);
		val &= ~(0x3 << offset);
		val |= cfg << offset;
		write32(pdat->virt + GPIO_ALTFN1, val);
	}
}

static int gpiochip_get_cfg(struct gpiochip_t * chip, int offset)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	if(offset < 16)
	{
		offset = offset << 0x1;
		val = read32(pdat->virt + GPIO_ALTFN0);
		return ((val >> offset) & 0x3);
	}
	else if(offset < 32)
	{
		offset = (offset - 16) << 0x1;
		val = read32(pdat->virt + GPIO_ALTFN1);
		return ((val >> offset) & 0x3);
	}

	return 0;
}

static void gpiochip_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(pull)
	{
	case GPIO_PULL_UP:
		val = read32(pdat->virt + GPIO_PULLSEL);
		val |= 1 << offset;
		write32(pdat->virt + GPIO_PULLSEL, val);
		val = read32(pdat->virt + GPIO_PULLENB);
		val |= 1 << offset;
		write32(pdat->virt + GPIO_PULLENB, val);
		break;

	case GPIO_PULL_DOWN:
		val = read32(pdat->virt + GPIO_PULLSEL);
		val &= ~(1 << offset);
		write32(pdat->virt + GPIO_PULLSEL, val);
		val = read32(pdat->virt + GPIO_PULLENB);
		val |= 1 << offset;
		write32(pdat->virt + GPIO_PULLENB, val);
		break;

	case GPIO_PULL_NONE:
		val = read32(pdat->virt + GPIO_PULLENB);
		val &= ~(1 << offset);
		write32(pdat->virt + GPIO_PULLENB, val);
		break;

	default:
		break;
	}

	val = read32(pdat->virt + GPIO_PULLSEL_DISABLE_DEFAULT);
	val |= 1 << offset;
	write32(pdat->virt + GPIO_PULLSEL_DISABLE_DEFAULT, val);
	val = read32(pdat->virt + GPIO_PULLENB_DISABLE_DEFAULT);
	val |= 1 << offset;
	write32(pdat->virt + GPIO_PULLENB_DISABLE_DEFAULT, val);
}

static enum gpio_pull_t gpiochip_get_pull(struct gpiochip_t * chip, int offset)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;

	val = read32(pdat->virt + GPIO_PULLENB);
	if(!((val >> offset) & 0x1))
	{
		val = read32(pdat->virt + GPIO_PULLSEL);
		if((val >> offset) & 0x1)
			return GPIO_PULL_UP;
		else
			return GPIO_PULL_DOWN;
	}
	return GPIO_PULL_NONE;
}

static void gpiochip_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(drv)
	{
	case GPIO_DRV_LOW:
		val = read32(pdat->virt + GPIO_DRV0);
		val &= ~(1 << offset);
		write32(pdat->virt + GPIO_DRV0, val);
		val = read32(pdat->virt + GPIO_DRV1);
		val &= ~(1 << offset);
		write32(pdat->virt + GPIO_DRV1, val);
		break;

	case GPIO_DRV_MEDIAN:
		val = read32(pdat->virt + GPIO_DRV0);
		val &= ~(1 << offset);
		write32(pdat->virt + GPIO_DRV0, val);
		val = read32(pdat->virt + GPIO_DRV1);
		val |= 1 << offset;
		write32(pdat->virt + GPIO_DRV1, val);
		break;

	case GPIO_DRV_HIGH:
		val = read32(pdat->virt + GPIO_DRV0);
		val |= 1 << offset;
		write32(pdat->virt + GPIO_DRV0, val);
		val = read32(pdat->virt + GPIO_DRV1);
		val |= 1 << offset;
		write32(pdat->virt + GPIO_DRV1, val);
		break;

	default:
		break;
	}

	val = read32(pdat->virt + GPIO_DRV0_DISABLE_DEFAULT);
	val |= 1 << offset;
	write32(pdat->virt + GPIO_DRV0_DISABLE_DEFAULT, val);
	val = read32(pdat->virt + GPIO_DRV1_DISABLE_DEFAULT);
	val |= 1 << offset;
	write32(pdat->virt + GPIO_DRV1_DISABLE_DEFAULT, val);
}

static enum gpio_drv_t gpiochip_get_drv(struct gpiochip_t * chip, int offset)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val, d;

	if(offset >= chip->ngpio)
		return GPIO_DRV_LOW;

	val = read32(pdat->virt + GPIO_DRV0);
	d = (val >> offset) & 0x1;
	val = read32(pdat->virt + GPIO_DRV1);
	d |= ((val >> offset) & 0x1) << 1;

	switch(d)
	{
	case 0x0:
		return GPIO_DRV_LOW;
	case 0x1:
		return GPIO_DRV_MEDIAN;
	case 0x2:
		return GPIO_DRV_MEDIAN;
	case 0x3:
		return GPIO_DRV_HIGH;
	default:
		break;
	}
	return GPIO_DRV_LOW;
}

static void gpiochip_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(rate)
	{
	case GPIO_RATE_SLOW:
		val = read32(pdat->virt + GPIO_SLEW);
		val |= 1 << offset;
		write32(pdat->virt + GPIO_SLEW, val);
		break;

	case GPIO_RATE_FAST:
		val = read32(pdat->virt + GPIO_SLEW);
		val &= ~(1 << offset);
		write32(pdat->virt + GPIO_SLEW, val);
		break;

	default:
		break;
	}

	val = read32(pdat->virt + GPIO_SLEW_DISABLE_DEFAULT);
	val |= 1 << offset;
	write32(pdat->virt + GPIO_SLEW_DISABLE_DEFAULT, val);
}

static enum gpio_rate_t gpiochip_get_rate(struct gpiochip_t * chip, int offset)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return GPIO_RATE_SLOW;

	val = read32(pdat->virt + GPIO_SLEW);
	if((val >> offset) & 0x1)
		return GPIO_RATE_SLOW;
	else
		return GPIO_RATE_FAST;
}

static void gpiochip_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		val = read32(pdat->virt + GPIO_OUTENB);
		val &= ~(0x1 << offset);
		write32(pdat->virt + GPIO_OUTENB, val);
		break;

	case GPIO_DIRECTION_OUTPUT:
		val = read32(pdat->virt + GPIO_OUTENB);
		val |= 0x1 << offset;
		write32(pdat->virt + GPIO_OUTENB, val);
		break;

	default:
		break;
	}
}

static enum gpio_direction_t gpiochip_get_dir(struct gpiochip_t * chip, int offset)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val, d;

	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_UNKOWN;

	val = read32(pdat->virt + GPIO_OUTENB);
	d = (val >> offset) & 0x1;
	switch(d)
	{
	case 0x0:
		return GPIO_DIRECTION_INPUT;
	case 0x1:
		return GPIO_DIRECTION_OUTPUT;
	default:
		break;
	}
	return GPIO_DIRECTION_UNKOWN;
}

static void gpiochip_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	val = read32(pdat->virt + GPIO_OUT);
	val &= ~(1 << offset);
	val |= (!!value) << offset;
	write32(pdat->virt + GPIO_OUT, val);
}

static int gpiochip_get_value(struct gpiochip_t * chip, int offset)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	val = read32(pdat->virt + GPIO_PAD);
	return !!(val & (1 << offset));
}

static int gpiochip_to_irq(struct gpiochip_t * chip, int offset)
{
	return -1;
}

static __init void s5p4418_gpiochip_init(void)
{
	struct gpiochip_pdata_t * pdat;
	struct gpiochip_t * chip;
	int i;

	for(i = 0; i < ARRAY_SIZE(datas); i++)
	{
		pdat = malloc(sizeof(struct gpiochip_pdata_t));
		if(!pdat)
			continue;

		chip = malloc(sizeof(struct gpiochip_t));
		if(!chip)
		{
			free(pdat);
			continue;
		}

		pdat->name = datas[i].name;
		pdat->base = datas[i].base;
		pdat->ngpio = datas[i].ngpio;
		pdat->virt = phys_to_virt(datas[i].phys);

		chip->name = pdat->name;
		chip->base = pdat->base;
		chip->ngpio = pdat->ngpio;
		chip->set_cfg = gpiochip_set_cfg;
		chip->get_cfg = gpiochip_get_cfg;
		chip->set_pull = gpiochip_set_pull;
		chip->get_pull = gpiochip_get_pull;
		chip->set_drv = gpiochip_set_drv;
		chip->get_drv = gpiochip_get_drv;
		chip->set_rate = gpiochip_set_rate;
		chip->get_rate = gpiochip_get_rate;
		chip->set_dir = gpiochip_set_dir;
		chip->get_dir = gpiochip_get_dir;
		chip->set_value = gpiochip_set_value;
		chip->get_value = gpiochip_get_value;
		chip->to_irq = gpiochip_to_irq;
		chip->priv = pdat;

		register_gpiochip(chip);
	}
}
core_initcall(s5p4418_gpiochip_init);
