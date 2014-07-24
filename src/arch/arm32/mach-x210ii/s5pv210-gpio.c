/*
 * s5pv210-gpio.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <s5pv210/reg-gpio.h>

struct s5pv210_gpio_data_t
{
	const char * name;
	int base;
	int ngpio;

	physical_addr_t regbase;
};

static void s5pv210_gpio_set_cfg(struct gpio_t * gpio, int offset, int cfg)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return;

	offset <<= 0x2;
	val = readl(dat->regbase + S5PV210_GPIO_CON);
	val &= ~(0xf << offset);
	val |= cfg << offset;
	writel(dat->regbase + S5PV210_GPIO_CON, val);
}

static int s5pv210_gpio_get_cfg(struct gpio_t * gpio, int offset)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return 0;

	offset <<= 0x2;
	val = readl(dat->regbase + S5PV210_GPIO_CON);
	return ((val >> offset) & 0xf);
}

static void s5pv210_gpio_set_pull(struct gpio_t * gpio, int offset, enum gpio_pull_t pull)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val, p;

	if(offset >= gpio->ngpio)
		return;

	switch(pull)
	{
	case GPIO_PULL_UP:
		p = 0x2;
		break;
	case GPIO_PULL_DOWN:
		p = 0x1;
		break;
	case GPIO_PULL_NONE:
		p = 0x0;
		break;
	default:
		p = 0x0;
		break;
	}

	offset <<= 0x1;
	val = readl(dat->regbase + S5PV210_GPIO_PUD);
	val &= ~(0x3 << offset);
	val |= p << offset;
	writel(dat->regbase + S5PV210_GPIO_PUD, val);
}

static enum gpio_pull_t s5pv210_gpio_get_pull(struct gpio_t * gpio, int offset)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val, p;

	if(offset >= gpio->ngpio)
		return GPIO_PULL_NONE;

	offset <<= 0x1;
	val = readl(dat->regbase + S5PV210_GPIO_PUD);
	p = (val >> offset) & 0x3;
	switch(p)
	{
	case 0x0:
		return GPIO_PULL_NONE;
	case 0x1:
		return GPIO_PULL_DOWN;
	case 0x2:
		return GPIO_PULL_UP;
	default:
		break;
	}
	return GPIO_PULL_NONE;
}

static void s5pv210_gpio_set_drv(struct gpio_t * gpio, int offset, enum gpio_drv_t drv)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val, d;

	if(offset >= gpio->ngpio)
		return;

	switch(drv)
	{
	case GPIO_DRV_LOW:
		d = 0x0;
		break;
	case GPIO_DRV_MEDIAN:
		d = 0x2;
		break;
	case GPIO_DRV_HIGH:
		d = 0x3;
		break;
	default:
		d = 0x0;
		break;
	}

	offset <<= 0x1;
	val = readl(dat->regbase + S5PV210_GPIO_DRV);
	val &= ~(0x3 << offset);
	val |= d << offset;
	writel(dat->regbase + S5PV210_GPIO_DRV, val);
}

static enum gpio_drv_t s5pv210_gpio_get_drv(struct gpio_t * gpio, int offset)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val, d;

	if(offset >= gpio->ngpio)
		return GPIO_DRV_NONE;

	offset <<= 0x1;
	val = readl(dat->regbase + S5PV210_GPIO_DRV);
	d = (val >> offset) & 0x3;
	switch(d)
	{
	case 0x0:
		return GPIO_DRV_LOW;
	case 0x1:
		return GPIO_DRV_LOW;
	case 0x2:
		return GPIO_DRV_MEDIAN;
	case 0x3:
		return GPIO_DRV_HIGH;
	default:
		break;
	}
	return GPIO_DRV_NONE;
}

static void s5pv210_gpio_set_rate(struct gpio_t * gpio, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t s5pv210_gpio_get_rate(struct gpio_t * gpio, int offset)
{
	return GPIO_RATE_NONE;
}

static void s5pv210_gpio_set_dir(struct gpio_t * gpio, int offset, enum gpio_direction_t dir)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return;

	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		offset <<= 0x2;
		val = readl(dat->regbase + S5PV210_GPIO_CON);
		val &= ~(0xf << offset);
		writel(dat->regbase + S5PV210_GPIO_CON, val);
		break;

	case GPIO_DIRECTION_OUTPUT:
		offset <<= 0x2;
		val = readl(dat->regbase + S5PV210_GPIO_CON);
		val &= ~(0xf << offset);
		val |= 0x1 << offset;
		writel(dat->regbase + S5PV210_GPIO_CON, val);
		break;

	default:
		break;
	}
}

static enum gpio_direction_t s5pv210_gpio_get_dir(struct gpio_t * gpio, int offset)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val, d;

	if(offset >= gpio->ngpio)
		return GPIO_DIRECTION_NONE;

	offset <<= 0x2;
	val = readl(dat->regbase + S5PV210_GPIO_CON);
	d = (val >> offset) & 0x4;
	switch(d)
	{
	case 0x0:
		return GPIO_DIRECTION_INPUT;
	case 0x1:
		return GPIO_DIRECTION_OUTPUT;
	default:
		break;
	}
	return GPIO_DIRECTION_NONE;
}

static void s5pv210_gpio_set_value(struct gpio_t * gpio, int offset, int value)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return;

	val = readl(dat->regbase + S5PV210_GPIO_DAT);
	val &= ~(1 << offset);
	val |= (!!value) << offset;
	writel(dat->regbase + S5PV210_GPIO_DAT, val);
}

static int s5pv210_gpio_get_value(struct gpio_t * gpio, int offset)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return 0;

	val = readl(dat->regbase + S5PV210_GPIO_DAT);
	return !!(val & (1 << offset));
}

static struct s5pv210_gpio_data_t gpio_datas[] = {
	{
		.name		= "GPA0",
		.base		= S5PV210_GPA0(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPA0_BASE,
	}, {
		.name		= "GPA1",
		.base		= S5PV210_GPA1(0),
		.ngpio		= 4,
		.regbase	= S5PV210_GPA1_BASE,
	}, {
		.name		= "GPB",
		.base		= S5PV210_GPB(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPB_BASE,
	}, {
		.name		= "GPC0",
		.base		= S5PV210_GPC0(0),
		.ngpio		= 5,
		.regbase	= S5PV210_GPC0_BASE,
	}, {
		.name		= "GPC1",
		.base		= S5PV210_GPC1(0),
		.ngpio		= 5,
		.regbase	= S5PV210_GPC1_BASE,
	}, {
		.name		= "GPD0",
		.base		= S5PV210_GPD0(0),
		.ngpio		= 4,
		.regbase	= S5PV210_GPD0_BASE,
	}, {
		.name		= "GPD1",
		.base		= S5PV210_GPD1(0),
		.ngpio		= 6,
		.regbase	= S5PV210_GPD1_BASE,
	}, {
		.name		= "GPE0",
		.base		= S5PV210_GPE0(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPE0_BASE,
	}, {
		.name		= "GPE1",
		.base		= S5PV210_GPE1(0),
		.ngpio		= 5,
		.regbase	= S5PV210_GPE1_BASE,
	}, {
		.name		= "GPF0",
		.base		= S5PV210_GPF0(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPF0_BASE,
	}, {
		.name		= "GPF1",
		.base		= S5PV210_GPF1(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPF1_BASE,
	}, {
		.name		= "GPF2",
		.base		= S5PV210_GPF2(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPF2_BASE,
	}, {
		.name		= "GPF3",
		.base		= S5PV210_GPF3(0),
		.ngpio		= 6,
		.regbase	= S5PV210_GPF3_BASE,
	}, {
		.name		= "GPG0",
		.base		= S5PV210_GPG0(0),
		.ngpio		= 7,
		.regbase	= S5PV210_GPG0_BASE,
	}, {
		.name		= "GPG1",
		.base		= S5PV210_GPG1(0),
		.ngpio		= 7,
		.regbase	= S5PV210_GPG1_BASE,
	}, {
		.name		= "GPG2",
		.base		= S5PV210_GPG2(0),
		.ngpio		= 7,
		.regbase	= S5PV210_GPG2_BASE,
	}, {
		.name		= "GPG3",
		.base		= S5PV210_GPG3(0),
		.ngpio		= 7,
		.regbase	= S5PV210_GPG3_BASE,
	}, {
		.name		= "GPH0",
		.base		= S5PV210_GPH0(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPH0_BASE,
	}, {
		.name		= "GPH1",
		.base		= S5PV210_GPH1(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPH1_BASE,
	}, {
		.name		= "GPH2",
		.base		= S5PV210_GPH2(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPH2_BASE,
	}, {
		.name		= "GPH3",
		.base		= S5PV210_GPH3(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPH3_BASE,
	}, {
		.name		= "GPI",
		.base		= S5PV210_GPI(0),
		.ngpio		= 7,
		.regbase	= S5PV210_GPI_BASE,
	}, {
		.name		= "GPJ0",
		.base		= S5PV210_GPJ0(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPJ0_BASE,
	}, {
		.name		= "GPJ1",
		.base		= S5PV210_GPJ1(0),
		.ngpio		= 6,
		.regbase	= S5PV210_GPJ1_BASE,
	}, {
		.name		= "GPJ2",
		.base		= S5PV210_GPJ2(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPJ2_BASE,
	}, {
		.name		= "GPJ3",
		.base		= S5PV210_GPJ3(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPJ3_BASE,
	}, {
		.name		= "GPJ4",
		.base		= S5PV210_GPJ4(0),
		.ngpio		= 5,
		.regbase	= S5PV210_GPJ4_BASE,
	}, {
		.name		= "GPMP01",
		.base		= S5PV210_GPMP01(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPMP01_BASE,
	}, {
		.name		= "GPMP02",
		.base		= S5PV210_GPMP02(0),
		.ngpio		= 4,
		.regbase	= S5PV210_GPMP02_BASE,
	}, {
		.name		= "GPMP03",
		.base		= S5PV210_GPMP03(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPMP03_BASE,
	}, {
		.name		= "GPMP04",
		.base		= S5PV210_GPMP04(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPMP04_BASE,
	}, {
		.name		= "GPMP05",
		.base		= S5PV210_GPMP05(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPMP05_BASE,
	}, {
		.name		= "GPMP06",
		.base		= S5PV210_GPMP06(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPMP06_BASE,
	}, {
		.name		= "GPMP07",
		.base		= S5PV210_GPMP07(0),
		.ngpio		= 8,
		.regbase	= S5PV210_GPMP07_BASE,
	}
};

static __init void s5pv210_gpio_init(void)
{
	struct gpio_t * gpio;
	int i;

	for(i = 0; i < ARRAY_SIZE(gpio_datas); i++)
	{
		gpio = malloc(sizeof(struct gpio_t));
		if(!gpio)
			continue;

		gpio->name = gpio_datas[i].name;
		gpio->base = gpio_datas[i].base;
		gpio->ngpio = gpio_datas[i].ngpio;
		gpio->set_cfg = s5pv210_gpio_set_cfg;
		gpio->get_cfg = s5pv210_gpio_get_cfg;
		gpio->set_pull = s5pv210_gpio_set_pull;
		gpio->get_pull = s5pv210_gpio_get_pull;
		gpio->set_drv = s5pv210_gpio_set_drv;
		gpio->get_drv = s5pv210_gpio_get_drv;
		gpio->set_rate = s5pv210_gpio_set_rate;
		gpio->get_rate = s5pv210_gpio_get_rate;
		gpio->set_dir = s5pv210_gpio_set_dir;
		gpio->get_dir = s5pv210_gpio_get_dir;
		gpio->set_value = s5pv210_gpio_set_value;
		gpio->get_value = s5pv210_gpio_get_value;
		gpio->priv = &gpio_datas[i];

		if(register_gpio(gpio))
			LOG("Register gpio '%s'", gpio->name);
		else
			LOG("Failed to register gpio '%s'", gpio->name);
	}
}

static __exit void s5pv210_gpio_exit(void)
{
	struct gpio_t * gpio;
	int i;

	for(i = 0; i < ARRAY_SIZE(gpio_datas); i++)
	{
		gpio = search_gpio(gpio_datas[i].name);
		if(!gpio)
			continue;
		if(unregister_gpio(gpio))
			LOG("Unregister gpio '%s'", gpio->name);
		else
			LOG("Failed to unregister gpio '%s'", gpio->name);
		free(gpio);
	}
}

postcore_initcall(s5pv210_gpio_init);
postcore_exitcall(s5pv210_gpio_exit);
