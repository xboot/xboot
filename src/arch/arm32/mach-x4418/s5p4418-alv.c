/*
 * s5p4418-alv.c
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
#include <s5p4418/reg-alv.h>
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
		.name	= "GPIOALV",
		.base	= S5P4418_GPIOALV(0),
		.ngpio	= 6,
		.phys	= S5P4418_GPIOALV_BASE,
	}
};

static inline void gpiochip_write_enable(struct gpiochip_t * chip)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	write32(pdat->virt + GPIOALV_PWRGATEREG, 0x1);
}

static inline void gpiochip_write_disable(struct gpiochip_t * chip)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	write32(pdat->virt + GPIOALV_PWRGATEREG, 0x0);
}

static void gpiochip_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
}

static int gpiochip_get_cfg(struct gpiochip_t * chip, int offset)
{
	return 0;
}

static void gpiochip_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val = 0;

	if(offset >= chip->ngpio)
		return;

	gpiochip_write_enable(chip);
	switch(pull)
	{
	case GPIO_PULL_UP:
		val |= 0x1 << offset;
		write32(pdat->virt + GPIOALV_PADPULLUPSETREG, val);
		break;

	case GPIO_PULL_DOWN:
		val |= 0x1 << offset;
		write32(pdat->virt + GPIOALV_PADPULLUPRSTREG, val);
		break;

	case GPIO_PULL_NONE:
		break;

	default:
		break;
	}
	gpiochip_write_disable(chip);
}

static enum gpio_pull_t gpiochip_get_pull(struct gpiochip_t * chip, int offset)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;

	val = read32(pdat->virt + GPIOALV_PADPULLUPREADREG);
	if(!((val >> offset) & 0x1))
		return GPIO_PULL_DOWN;
	else
		return GPIO_PULL_UP;

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
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val = 0;

	if(offset >= chip->ngpio)
		return;

	gpiochip_write_enable(chip);
	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		val |= 0x1 << offset;
		write32(pdat->virt + GPIOALV_PADOUTENBRSTREG, val);
		break;

	case GPIO_DIRECTION_OUTPUT:
		val |= 0x1 << offset;
		write32(pdat->virt + GPIOALV_PADOUTENBSETREG, val);
		break;

	default:
		break;
	}
	gpiochip_write_disable(chip);
}

static enum gpio_direction_t gpiochip_get_dir(struct gpiochip_t * chip, int offset)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val, d;

	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_UNKOWN;

	val = read32(pdat->virt + GPIOALV_PADOUTENBREADREG);
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
	u32_t val = 0;

	if(offset >= chip->ngpio)
		return;

	gpiochip_write_enable(chip);
	if(value)
	{
		val |= 0x1 << offset;
		write32(pdat->virt + GPIOALV_PADOUTSETREG, val);
	}
	else
	{
		val |= 0x1 << offset;
		write32(pdat->virt + GPIOALV_PADOUTRSTREG, val);
	}
	gpiochip_write_disable(chip);
}

static int gpiochip_get_value(struct gpiochip_t * chip, int offset)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	val = read32(pdat->virt + GPIOALV_GPIOINPUTVALUE);
	return !!(val & (1 << offset));
}

static int gpiochip_to_irq(struct gpiochip_t * chip, int offset)
{
	return -1;
}

static __init void s5p4418_gpiochip_alv_init(void)
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
core_initcall(s5p4418_gpiochip_alv_init);
