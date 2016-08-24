/*
 * bcm2836-gpio.c
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
#include <bcm2836-gpio.h>
#include <bcm2836/reg-gpio.h>

enum {
	ALT_FUNC0 = 0x4,
	ALT_FUNC1 = 0x5,
	ALT_FUNC2 = 0x6,
	ALT_FUNC3 = 0x7,
	ALT_FUNC4 = 0x3,
	ALT_FUNC5 = 0x2,
	ALT_FUNC6_GPIO_OUTPUT = 0x1,
	ALT_FUNC7_GPIO_INPUT = 0x0,
};

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
		.name	= "GPIO",
		.base	= BCM2836_GPIO(0),
		.ngpio	= 54,
		.phys	= BCM2836_GPIO_BASE,
	}
};

static void gpiochip_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	int bank = offset / 10;
	int field = (offset - 10 * bank) * 3;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(cfg & 0x7)
	{
	case 0: cfg = ALT_FUNC0; break;
	case 1: cfg = ALT_FUNC1; break;
	case 2: cfg = ALT_FUNC2; break;
	case 3: cfg = ALT_FUNC3; break;
	case 4: cfg = ALT_FUNC4; break;
	case 5: cfg = ALT_FUNC5; break;
	case 6: cfg = ALT_FUNC6_GPIO_OUTPUT; break;
	case 7: cfg = ALT_FUNC7_GPIO_INPUT; break;
	default: break;
	}

	val = read32(pdat->virt + GPIO_FSEL(bank));
	val &= ~(0x7 << field);
	val |= cfg << field;
	write32(pdat->virt + GPIO_FSEL(bank), val);
}

static int gpiochip_get_cfg(struct gpiochip_t * chip, int offset)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	int bank = offset / 10;
	int field = (offset - 10 * bank) * 3;
	int cfg;
	u32_t val;

	val = read32(pdat->virt + GPIO_FSEL(bank));
	switch((val >> field) & 0x7)
	{
	case ALT_FUNC0: cfg = 0; break;
	case ALT_FUNC1: cfg = 1; break;
	case ALT_FUNC2: cfg = 2; break;
	case ALT_FUNC3: cfg = 3; break;
	case ALT_FUNC4: cfg = 4; break;
	case ALT_FUNC5: cfg = 5; break;
	case ALT_FUNC6_GPIO_OUTPUT: cfg = 6; break;
	case ALT_FUNC7_GPIO_INPUT: cfg = 7; break;
	default: break;
	}
	return cfg;
}

static void gpiochip_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	int bank = offset / 32;
	int field = (offset - 32 * bank);

	if(offset >= chip->ngpio)
		return;

	switch(pull)
	{
	case GPIO_PULL_UP:
		write32(pdat->virt + GPIO_UD(0), 2);
		break;

	case GPIO_PULL_DOWN:
		write32(pdat->virt + GPIO_UD(0), 1);
		break;

	case GPIO_PULL_NONE:
		write32(pdat->virt + GPIO_UD(0), 0);
		break;

	default:
		return;
	}

	udelay(5);
	write32(pdat->virt + GPIO_UDCLK(bank), 1 << field);
	udelay(5);
	write32(pdat->virt + GPIO_UD(0), 0);
	write32(pdat->virt + GPIO_UDCLK(bank), 0 << field);
}

static enum gpio_pull_t gpiochip_get_pull(struct gpiochip_t * chip, int offset)
{
	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;
	return GPIO_PULL_NONE;
}

static void gpiochip_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
}

static enum gpio_drv_t gpiochip_get_drv(struct gpiochip_t * chip, int offset)
{
	return GPIO_DRV_LOW;
}

static void gpiochip_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t gpiochip_get_rate(struct gpiochip_t * chip, int offset)
{
	return GPIO_RATE_SLOW;
}

static void gpiochip_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	if(offset >= chip->ngpio)
		return;

	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		gpiochip_set_cfg(chip, offset, 7);
		break;

	case GPIO_DIRECTION_OUTPUT:
		gpiochip_set_cfg(chip, offset, 6);
		break;

	default:
		break;
	}
}

static enum gpio_direction_t gpiochip_get_dir(struct gpiochip_t * chip, int offset)
{
	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_UNKOWN;

	switch(gpiochip_get_cfg(chip, offset))
	{
	case 6:
		return GPIO_DIRECTION_OUTPUT;
	case 7:
		return GPIO_DIRECTION_INPUT;
	default:
		break;
	}
	return GPIO_DIRECTION_UNKOWN;
}

static void gpiochip_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	int bank = offset / 32;
	int field = (offset - 32 * bank);

	if(offset >= chip->ngpio)
		return;

	if(value)
		write32(pdat->virt + GPIO_SET(bank), 1 << field);
	else
		write32(pdat->virt + GPIO_CLR(bank), 1 << field);
}

static int gpiochip_get_value(struct gpiochip_t * chip, int offset)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	int bank = offset / 32;
	int field = (offset - 32 * bank);
	u32_t lev;

	if(offset >= chip->ngpio)
		return 0;

	lev = read32(pdat->virt + GPIO_LEV(bank));
	return (lev & (1 << field)) ? 1 : 0;
}

static int gpiochip_to_irq(struct gpiochip_t * chip, int offset)
{
	return -1;
}

static __init void bcm2836_gpiochip_init(void)
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
core_initcall(bcm2836_gpiochip_init);
