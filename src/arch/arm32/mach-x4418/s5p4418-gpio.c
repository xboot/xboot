/*
 * s5p4418-gpio.c
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
#include <s5p4418/reg-gpio.h>

struct s5p4418_gpiochip_data_t
{
	const char * name;
	int base;
	int ngpio;
	physical_addr_t regbase;
};

static void s5p4418_gpiochip_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
	struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	if(offset < 16)
	{
		offset <<= 0x1;
		val = readl(dat->regbase + S5P4418_GPIO_ALTFN0);
		val &= ~(0x3 << offset);
		val |= cfg << offset;
		writel(dat->regbase + S5P4418_GPIO_ALTFN0, val);
	}
	else if(offset < 32)
	{
		offset <<= 0x1;
		val = readl(dat->regbase + S5P4418_GPIO_ALTFN1);
		val &= ~(0x3 << offset);
		val |= cfg << offset;
		writel(dat->regbase + S5P4418_GPIO_ALTFN1, val);
	}
}

static int s5p4418_gpiochip_get_cfg(struct gpiochip_t * chip, int offset)
{
	struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	if(offset < 16)
	{
		offset <<= 0x1;
		val = readl(dat->regbase + S5P4418_GPIO_ALTFN0);
		return ((val >> offset) & 0x3);
	}
	else if(offset < 32)
	{
		offset <<= 0x1;
		val = readl(dat->regbase + S5P4418_GPIO_ALTFN1);
		return ((val >> offset) & 0x3);
	}

	return 0;
}

static void s5p4418_gpiochip_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(pull)
	{
	case GPIO_PULL_UP:
		val = readl(dat->regbase + S5P4418_GPIO_PULLSEL);
		val |= 1 << offset;
		writel(dat->regbase + S5P4418_GPIO_PULLSEL, val);
		val = readl(dat->regbase + S5P4418_GPIO_PULLENB);
		val |= 1 << offset;
		writel(dat->regbase + S5P4418_GPIO_PULLENB, val);
		break;

	case GPIO_PULL_DOWN:
		val = readl(dat->regbase + S5P4418_GPIO_PULLSEL);
		val &= ~(1 << offset);
		writel(dat->regbase + S5P4418_GPIO_PULLSEL, val);
		val = readl(dat->regbase + S5P4418_GPIO_PULLENB);
		val |= 1 << offset;
		writel(dat->regbase + S5P4418_GPIO_PULLENB, val);
		break;

	case GPIO_PULL_NONE:
		val = readl(dat->regbase + S5P4418_GPIO_PULLENB);
		val &= ~(1 << offset);
		writel(dat->regbase + S5P4418_GPIO_PULLENB, val);
		break;

	default:
		break;
	}

	val = readl(dat->regbase + S5P4418_GPIO_PULLSEL_DISABLE_DEFAULT);
	val |= 1 << offset;
	writel(dat->regbase + S5P4418_GPIO_PULLSEL_DISABLE_DEFAULT, val);
	val = readl(dat->regbase + S5P4418_GPIO_PULLENB_DISABLE_DEFAULT);
	val |= 1 << offset;
	writel(dat->regbase + S5P4418_GPIO_PULLENB_DISABLE_DEFAULT, val);
}

static enum gpio_pull_t s5p4418_gpiochip_get_pull(struct gpiochip_t * chip, int offset)
{
	struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;

	val = readl(dat->regbase + S5P4418_GPIO_PULLENB);
	if(!((val >> offset) & 0x1))
	{
		val = readl(dat->regbase + S5P4418_GPIO_PULLSEL);
		if((val >> offset) & 0x1)
			return GPIO_PULL_UP;
		else
			return GPIO_PULL_DOWN;
	}
	return GPIO_PULL_NONE;
}

static void s5p4418_gpiochip_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
	struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(drv)
	{
	case GPIO_DRV_LOW:
		val = readl(dat->regbase + S5P4418_GPIO_DRV0);
		val &= ~(1 << offset);
		writel(dat->regbase + S5P4418_GPIO_DRV0, val);
		val = readl(dat->regbase + S5P4418_GPIO_DRV1);
		val &= ~(1 << offset);
		writel(dat->regbase + S5P4418_GPIO_DRV1, val);
		break;

	case GPIO_DRV_MEDIAN:
		val = readl(dat->regbase + S5P4418_GPIO_DRV0);
		val &= ~(1 << offset);
		writel(dat->regbase + S5P4418_GPIO_DRV0, val);
		val = readl(dat->regbase + S5P4418_GPIO_DRV1);
		val |= 1 << offset;
		writel(dat->regbase + S5P4418_GPIO_DRV1, val);
		break;

	case GPIO_DRV_HIGH:
		val = readl(dat->regbase + S5P4418_GPIO_DRV0);
		val |= 1 << offset;
		writel(dat->regbase + S5P4418_GPIO_DRV0, val);
		val = readl(dat->regbase + S5P4418_GPIO_DRV1);
		val |= 1 << offset;
		writel(dat->regbase + S5P4418_GPIO_DRV1, val);
		break;

	default:
		break;
	}

	val = readl(dat->regbase + S5P4418_GPIO_DRV0_DISABLE_DEFAULT);
	val |= 1 << offset;
	writel(dat->regbase + S5P4418_GPIO_DRV0_DISABLE_DEFAULT, val);
	val = readl(dat->regbase + S5P4418_GPIO_DRV1_DISABLE_DEFAULT);
	val |= 1 << offset;
	writel(dat->regbase + S5P4418_GPIO_DRV1_DISABLE_DEFAULT, val);
}

static enum gpio_drv_t s5p4418_gpiochip_get_drv(struct gpiochip_t * chip, int offset)
{
	struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
	u32_t val, d;

	if(offset >= chip->ngpio)
		return GPIO_DRV_LOW;

	val = readl(dat->regbase + S5P4418_GPIO_DRV0);
	d = (val >> offset) & 0x1;
	val = readl(dat->regbase + S5P4418_GPIO_DRV1);
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

static void s5p4418_gpiochip_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t s5p4418_gpiochip_get_rate(struct gpiochip_t * chip, int offset)
{
	return GPIO_RATE_SLOW;
}

static void s5p4418_gpiochip_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		val = readl(dat->regbase + S5P4418_GPIO_OUTENB);
		val &= ~(0x1 << offset);
		writel(dat->regbase + S5P4418_GPIO_OUTENB, val);
		break;

	case GPIO_DIRECTION_OUTPUT:
		val = readl(dat->regbase + S5P4418_GPIO_OUTENB);
		val |= 0x1 << offset;
		writel(dat->regbase + S5P4418_GPIO_OUTENB, val);
		break;

	default:
		break;
	}
}

static enum gpio_direction_t s5p4418_gpiochip_get_dir(struct gpiochip_t * chip, int offset)
{
	struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
	u32_t val, d;

	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_UNKOWN;

	val = readl(dat->regbase + S5P4418_GPIO_OUTENB);
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

static void s5p4418_gpiochip_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	val = readl(dat->regbase + S5P4418_GPIO_OUT);
	val &= ~(1 << offset);
	val |= (!!value) << offset;
	writel(dat->regbase + S5P4418_GPIO_OUT, val);
}

static int s5p4418_gpiochip_get_value(struct gpiochip_t * chip, int offset)
{
	struct s5p4418_gpiochip_data_t * dat = (struct s5p4418_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	val = readl(dat->regbase + S5P4418_GPIO_OUT);
	return !!(val & (1 << offset));
}

static struct s5p4418_gpiochip_data_t gpiochip_datas[] = {
	{
		.name		= "GPIOA",
		.base		= S5P4418_GPIOA(0),
		.ngpio		= 32,
		.regbase	= S5P4418_GPIOA_BASE,
	}, {
		.name		= "GPIOB",
		.base		= S5P4418_GPIOB(0),
		.ngpio		= 32,
		.regbase	= S5P4418_GPIOB_BASE,
	}, {
		.name		= "GPIOC",
		.base		= S5P4418_GPIOC(0),
		.ngpio		= 32,
		.regbase	= S5P4418_GPIOC_BASE,
	}, {
		.name		= "GPIOD",
		.base		= S5P4418_GPIOD(0),
		.ngpio		= 32,
		.regbase	= S5P4418_GPIOD_BASE,
	}, {
		.name		= "GPIOE",
		.base		= S5P4418_GPIOE(0),
		.ngpio		= 32,
		.regbase	= S5P4418_GPIOE_BASE,
	}
};

static __init void s5p4418_gpiochip_init(void)
{
	struct gpiochip_t * chip;
	int i;

	for(i = 0; i < ARRAY_SIZE(gpiochip_datas); i++)
	{
		chip = malloc(sizeof(struct gpiochip_t));
		if(!chip)
			continue;

		chip->name = gpiochip_datas[i].name;
		chip->base = gpiochip_datas[i].base;
		chip->ngpio = gpiochip_datas[i].ngpio;
		chip->set_cfg = s5p4418_gpiochip_set_cfg;
		chip->get_cfg = s5p4418_gpiochip_get_cfg;
		chip->set_pull = s5p4418_gpiochip_set_pull;
		chip->get_pull = s5p4418_gpiochip_get_pull;
		chip->set_drv = s5p4418_gpiochip_set_drv;
		chip->get_drv = s5p4418_gpiochip_get_drv;
		chip->set_rate = s5p4418_gpiochip_set_rate;
		chip->get_rate = s5p4418_gpiochip_get_rate;
		chip->set_dir = s5p4418_gpiochip_set_dir;
		chip->get_dir = s5p4418_gpiochip_get_dir;
		chip->set_value = s5p4418_gpiochip_set_value;
		chip->get_value = s5p4418_gpiochip_get_value;
		chip->priv = &gpiochip_datas[i];

		if(register_gpiochip(chip))
			LOG("Register gpiochip '%s'", chip->name);
		else
			LOG("Failed to register gpiochip '%s'", chip->name);
	}
}

static __exit void s5p4418_gpiochip_exit(void)
{
	struct gpiochip_t * chip;
	int i;

	for(i = 0; i < ARRAY_SIZE(gpiochip_datas); i++)
	{
		chip = search_gpiochip(gpiochip_datas[i].name);
		if(!chip)
			continue;
		if(unregister_gpiochip(chip))
			LOG("Unregister gpiochip '%s'", chip->name);
		else
			LOG("Failed to unregister gpiochip '%s'", chip->name);
		free(chip);
	}
}

core_initcall(s5p4418_gpiochip_init);
core_exitcall(s5p4418_gpiochip_exit);
