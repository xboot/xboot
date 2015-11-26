/*
 * s5p6818-gpio.c
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
#include <s5p6818/reg-gpio.h>
#include <s5p6818-gpio.h>

struct s5p6818_gpiochip_data_t
{
	const char * name;
	int base;
	int ngpio;
	physical_addr_t regbase;
};

static void s5p6818_gpiochip_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
	struct s5p6818_gpiochip_data_t * dat = (struct s5p6818_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	if(offset < 16)
	{
		offset = offset << 0x1;
		val = read32(phys_to_virt(dat->regbase + GPIO_ALTFN0));
		val &= ~(0x3 << offset);
		val |= cfg << offset;
		write32(phys_to_virt(dat->regbase + GPIO_ALTFN0), val);
	}
	else if(offset < 32)
	{
		offset = (offset - 16) << 0x1;
		val = read32(phys_to_virt(dat->regbase + GPIO_ALTFN1));
		val &= ~(0x3 << offset);
		val |= cfg << offset;
		write32(phys_to_virt(dat->regbase + GPIO_ALTFN1), val);
	}
}

static int s5p6818_gpiochip_get_cfg(struct gpiochip_t * chip, int offset)
{
	struct s5p6818_gpiochip_data_t * dat = (struct s5p6818_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	if(offset < 16)
	{
		offset = offset << 0x1;
		val = read32(phys_to_virt(dat->regbase + GPIO_ALTFN0));
		return ((val >> offset) & 0x3);
	}
	else if(offset < 32)
	{
		offset = (offset - 16) << 0x1;
		val = read32(phys_to_virt(dat->regbase + GPIO_ALTFN1));
		return ((val >> offset) & 0x3);
	}

	return 0;
}

static void s5p6818_gpiochip_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct s5p6818_gpiochip_data_t * dat = (struct s5p6818_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(pull)
	{
	case GPIO_PULL_UP:
		val = read32(phys_to_virt(dat->regbase + GPIO_PULLSEL));
		val |= 1 << offset;
		write32(phys_to_virt(dat->regbase + GPIO_PULLSEL), val);
		val = read32(phys_to_virt(dat->regbase + GPIO_PULLENB));
		val |= 1 << offset;
		write32(phys_to_virt(dat->regbase + GPIO_PULLENB), val);
		break;

	case GPIO_PULL_DOWN:
		val = read32(phys_to_virt(dat->regbase + GPIO_PULLSEL));
		val &= ~(1 << offset);
		write32(phys_to_virt(dat->regbase + GPIO_PULLSEL), val);
		val = read32(phys_to_virt(dat->regbase + GPIO_PULLENB));
		val |= 1 << offset;
		write32(phys_to_virt(dat->regbase + GPIO_PULLENB), val);
		break;

	case GPIO_PULL_NONE:
		val = read32(phys_to_virt(dat->regbase + GPIO_PULLENB));
		val &= ~(1 << offset);
		write32(phys_to_virt(dat->regbase + GPIO_PULLENB), val);
		break;

	default:
		break;
	}

	val = read32(phys_to_virt(dat->regbase + GPIO_PULLSEL_DISABLE_DEFAULT));
	val |= 1 << offset;
	write32(phys_to_virt(dat->regbase + GPIO_PULLSEL_DISABLE_DEFAULT), val);
	val = read32(phys_to_virt(dat->regbase + GPIO_PULLENB_DISABLE_DEFAULT));
	val |= 1 << offset;
	write32(phys_to_virt(dat->regbase + GPIO_PULLENB_DISABLE_DEFAULT), val);
}

static enum gpio_pull_t s5p6818_gpiochip_get_pull(struct gpiochip_t * chip, int offset)
{
	struct s5p6818_gpiochip_data_t * dat = (struct s5p6818_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;

	val = read32(phys_to_virt(dat->regbase + GPIO_PULLENB));
	if(!((val >> offset) & 0x1))
	{
		val = read32(phys_to_virt(dat->regbase + GPIO_PULLSEL));
		if((val >> offset) & 0x1)
			return GPIO_PULL_UP;
		else
			return GPIO_PULL_DOWN;
	}
	return GPIO_PULL_NONE;
}

static void s5p6818_gpiochip_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
	struct s5p6818_gpiochip_data_t * dat = (struct s5p6818_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(drv)
	{
	case GPIO_DRV_LOW:
		val = read32(phys_to_virt(dat->regbase + GPIO_DRV0));
		val &= ~(1 << offset);
		write32(phys_to_virt(dat->regbase + GPIO_DRV0), val);
		val = read32(phys_to_virt(dat->regbase + GPIO_DRV1));
		val &= ~(1 << offset);
		write32(phys_to_virt(dat->regbase + GPIO_DRV1), val);
		break;

	case GPIO_DRV_MEDIAN:
		val = read32(phys_to_virt(dat->regbase + GPIO_DRV0));
		val &= ~(1 << offset);
		write32(phys_to_virt(dat->regbase + GPIO_DRV0), val);
		val = read32(phys_to_virt(dat->regbase + GPIO_DRV1));
		val |= 1 << offset;
		write32(phys_to_virt(dat->regbase + GPIO_DRV1), val);
		break;

	case GPIO_DRV_HIGH:
		val = read32(phys_to_virt(dat->regbase + GPIO_DRV0));
		val |= 1 << offset;
		write32(phys_to_virt(dat->regbase + GPIO_DRV0), val);
		val = read32(phys_to_virt(dat->regbase + GPIO_DRV1));
		val |= 1 << offset;
		write32(phys_to_virt(dat->regbase + GPIO_DRV1), val);
		break;

	default:
		break;
	}

	val = read32(phys_to_virt(dat->regbase + GPIO_DRV0_DISABLE_DEFAULT));
	val |= 1 << offset;
	write32(phys_to_virt(dat->regbase + GPIO_DRV0_DISABLE_DEFAULT), val);
	val = read32(phys_to_virt(dat->regbase + GPIO_DRV1_DISABLE_DEFAULT));
	val |= 1 << offset;
	write32(phys_to_virt(dat->regbase + GPIO_DRV1_DISABLE_DEFAULT), val);
}

static enum gpio_drv_t s5p6818_gpiochip_get_drv(struct gpiochip_t * chip, int offset)
{
	struct s5p6818_gpiochip_data_t * dat = (struct s5p6818_gpiochip_data_t *)chip->priv;
	u32_t val, d;

	if(offset >= chip->ngpio)
		return GPIO_DRV_LOW;

	val = read32(phys_to_virt(dat->regbase + GPIO_DRV0));
	d = (val >> offset) & 0x1;
	val = read32(phys_to_virt(dat->regbase + GPIO_DRV1));
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

static void s5p6818_gpiochip_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
	struct s5p6818_gpiochip_data_t * dat = (struct s5p6818_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(rate)
	{
	case GPIO_RATE_SLOW:
		val = read32(phys_to_virt(dat->regbase + GPIO_SLEW));
		val |= 1 << offset;
		write32(phys_to_virt(dat->regbase + GPIO_SLEW), val);
		break;

	case GPIO_RATE_FAST:
		val = read32(phys_to_virt(dat->regbase + GPIO_SLEW));
		val &= ~(1 << offset);
		write32(phys_to_virt(dat->regbase + GPIO_SLEW), val);
		break;

	default:
		break;
	}

	val = read32(phys_to_virt(dat->regbase + GPIO_SLEW_DISABLE_DEFAULT));
	val |= 1 << offset;
	write32(phys_to_virt(dat->regbase + GPIO_SLEW_DISABLE_DEFAULT), val);
}

static enum gpio_rate_t s5p6818_gpiochip_get_rate(struct gpiochip_t * chip, int offset)
{
	struct s5p6818_gpiochip_data_t * dat = (struct s5p6818_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return GPIO_RATE_SLOW;

	val = read32(phys_to_virt(dat->regbase + GPIO_SLEW));
	if((val >> offset) & 0x1)
		return GPIO_RATE_SLOW;
	else
		return GPIO_RATE_FAST;
}

static void s5p6818_gpiochip_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	struct s5p6818_gpiochip_data_t * dat = (struct s5p6818_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		val = read32(phys_to_virt(dat->regbase + GPIO_OUTENB));
		val &= ~(0x1 << offset);
		write32(phys_to_virt(dat->regbase + GPIO_OUTENB), val);
		break;

	case GPIO_DIRECTION_OUTPUT:
		val = read32(phys_to_virt(dat->regbase + GPIO_OUTENB));
		val |= 0x1 << offset;
		write32(phys_to_virt(dat->regbase + GPIO_OUTENB), val);
		break;

	default:
		break;
	}
}

static enum gpio_direction_t s5p6818_gpiochip_get_dir(struct gpiochip_t * chip, int offset)
{
	struct s5p6818_gpiochip_data_t * dat = (struct s5p6818_gpiochip_data_t *)chip->priv;
	u32_t val, d;

	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_UNKOWN;

	val = read32(phys_to_virt(dat->regbase + GPIO_OUTENB));
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

static void s5p6818_gpiochip_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct s5p6818_gpiochip_data_t * dat = (struct s5p6818_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	val = read32(phys_to_virt(dat->regbase + GPIO_OUT));
	val &= ~(1 << offset);
	val |= (!!value) << offset;
	write32(phys_to_virt(dat->regbase + GPIO_OUT), val);
}

static int s5p6818_gpiochip_get_value(struct gpiochip_t * chip, int offset)
{
	struct s5p6818_gpiochip_data_t * dat = (struct s5p6818_gpiochip_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	val = read32(phys_to_virt(dat->regbase + GPIO_PAD));
	return !!(val & (1 << offset));
}

static const char * s5p6818_gpiochip_to_irq(struct gpiochip_t * chip, int offset)
{
	struct s5p6818_gpiochip_data_t * dat = (struct s5p6818_gpiochip_data_t *)chip->priv;
	static const char * irq_gpioa[] = {
		"GPIOA0",  "GPIOA1",  "GPIOA2",  "GPIOA3",  "GPIOA4",  "GPIOA5",  "GPIOA6",  "GPIOA7",
		"GPIOA8",  "GPIOA9",  "GPIOA10", "GPIOA11", "GPIOA12", "GPIOA13", "GPIOA14", "GPIOA15",
		"GPIOA16", "GPIOA17", "GPIOA18", "GPIOA19", "GPIOA20", "GPIOA21", "GPIOA22", "GPIOA23",
		"GPIOA24", "GPIOA25", "GPIOA26", "GPIOA27", "GPIOA28", "GPIOA29", "GPIOA30", "GPIOA31",
	};
	static const char * irq_gpiob[] = {
		"GPIOB0",  "GPIOB1",  "GPIOB2",  "GPIOB3",  "GPIOB4",  "GPIOB5",  "GPIOB6",  "GPIOB7",
		"GPIOB8",  "GPIOB9",  "GPIOB10", "GPIOB11", "GPIOB12", "GPIOB13", "GPIOB14", "GPIOB15",
		"GPIOB16", "GPIOB17", "GPIOB18", "GPIOB19", "GPIOB20", "GPIOB21", "GPIOB22", "GPIOB23",
		"GPIOB24", "GPIOB25", "GPIOB26", "GPIOB27", "GPIOB28", "GPIOB29", "GPIOB30", "GPIOB31",
	};
	static const char * irq_gpioc[] = {
		"GPIOC0",  "GPIOC1",  "GPIOC2",  "GPIOC3",  "GPIOC4",  "GPIOC5",  "GPIOC6",  "GPIOC7",
		"GPIOC8",  "GPIOC9",  "GPIOC10", "GPIOC11", "GPIOC12", "GPIOC13", "GPIOC14", "GPIOC15",
		"GPIOC16", "GPIOC17", "GPIOC18", "GPIOC19", "GPIOC20", "GPIOC21", "GPIOC22", "GPIOC23",
		"GPIOC24", "GPIOC25", "GPIOC26", "GPIOC27", "GPIOC28", "GPIOC29", "GPIOC30", "GPIOC31",
	};
	static const char * irq_gpiod[] = {
		"GPIOD0",  "GPIOD1",  "GPIOD2",  "GPIOD3",  "GPIOD4",  "GPIOD5",  "GPIOD6",  "GPIOD7",
		"GPIOD8",  "GPIOD9",  "GPIOD10", "GPIOD11", "GPIOD12", "GPIOD13", "GPIOD14", "GPIOD15",
		"GPIOD16", "GPIOD17", "GPIOD18", "GPIOD19", "GPIOD20", "GPIOD21", "GPIOD22", "GPIOD23",
		"GPIOD24", "GPIOD25", "GPIOD26", "GPIOD27", "GPIOD28", "GPIOD29", "GPIOD30", "GPIOD31",
	};
	static const char * irq_gpioe[] = {
		"GPIOE0",  "GPIOE1",  "GPIOE2",  "GPIOE3",  "GPIOE4",  "GPIOE5",  "GPIOE6",  "GPIOE7",
		"GPIOE8",  "GPIOE9",  "GPIOE10", "GPIOE11", "GPIOE12", "GPIOE13", "GPIOE14", "GPIOE15",
		"GPIOE16", "GPIOE17", "GPIOE18", "GPIOE19", "GPIOE20", "GPIOE21", "GPIOE22", "GPIOE23",
		"GPIOE24", "GPIOE25", "GPIOE26", "GPIOE27", "GPIOE28", "GPIOE29", "GPIOE30", "GPIOE31",
	};

	if(offset >= chip->ngpio)
		return 0;

	if(strcmp(dat->name, "GPIOA") == 0)
		return irq_gpioa[offset];
	else if(strcmp(dat->name, "GPIOB") == 0)
		return irq_gpiob[offset];
	else if(strcmp(dat->name, "GPIOC") == 0)
		return irq_gpioc[offset];
	else if(strcmp(dat->name, "GPIOD") == 0)
		return irq_gpiod[offset];
	else if(strcmp(dat->name, "GPIOE") == 0)
		return irq_gpioe[offset];
	return 0;
}

static struct s5p6818_gpiochip_data_t gpiochip_datas[] = {
	{
		.name		= "GPIOA",
		.base		= S5P6818_GPIOA(0),
		.ngpio		= 32,
		.regbase	= S5P6818_GPIOA_BASE,
	}, {
		.name		= "GPIOB",
		.base		= S5P6818_GPIOB(0),
		.ngpio		= 32,
		.regbase	= S5P6818_GPIOB_BASE,
	}, {
		.name		= "GPIOC",
		.base		= S5P6818_GPIOC(0),
		.ngpio		= 32,
		.regbase	= S5P6818_GPIOC_BASE,
	}, {
		.name		= "GPIOD",
		.base		= S5P6818_GPIOD(0),
		.ngpio		= 32,
		.regbase	= S5P6818_GPIOD_BASE,
	}, {
		.name		= "GPIOE",
		.base		= S5P6818_GPIOE(0),
		.ngpio		= 32,
		.regbase	= S5P6818_GPIOE_BASE,
	}
};

static __init void s5p6818_gpiochip_init(void)
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
		chip->set_cfg = s5p6818_gpiochip_set_cfg;
		chip->get_cfg = s5p6818_gpiochip_get_cfg;
		chip->set_pull = s5p6818_gpiochip_set_pull;
		chip->get_pull = s5p6818_gpiochip_get_pull;
		chip->set_drv = s5p6818_gpiochip_set_drv;
		chip->get_drv = s5p6818_gpiochip_get_drv;
		chip->set_rate = s5p6818_gpiochip_set_rate;
		chip->get_rate = s5p6818_gpiochip_get_rate;
		chip->set_dir = s5p6818_gpiochip_set_dir;
		chip->get_dir = s5p6818_gpiochip_get_dir;
		chip->set_value = s5p6818_gpiochip_set_value;
		chip->get_value = s5p6818_gpiochip_get_value;
		chip->to_irq = s5p6818_gpiochip_to_irq;
		chip->priv = &gpiochip_datas[i];

		if(register_gpiochip(chip))
			LOG("Register gpiochip '%s'", chip->name);
		else
			LOG("Failed to register gpiochip '%s'", chip->name);
	}
}

static __exit void s5p6818_gpiochip_exit(void)
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

core_initcall(s5p6818_gpiochip_init);
core_exitcall(s5p6818_gpiochip_exit);
