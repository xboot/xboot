/*
 * exynos4412-gpio.c
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
#include <exynos4412/reg-gpio.h>

struct exynos4412_gpio_data_t
{
	const char * name;
	int base;
	int ngpio;

	physical_addr_t regbase;
};

static void exynos4412_gpio_set_cfg(struct gpio_t * gpio, int offset, int cfg)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return;

	offset <<= 0x2;
	val = readl(dat->regbase + EXYNOS4412_GPIO_CON);
	val &= ~(0xf << offset);
	val |= cfg << offset;
	writel(dat->regbase + EXYNOS4412_GPIO_CON, val);
}

static int exynos4412_gpio_get_cfg(struct gpio_t * gpio, int offset)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return 0;

	offset <<= 0x2;
	val = readl(dat->regbase + EXYNOS4412_GPIO_CON);
	return ((val >> offset) & 0xf);
}

static void exynos4412_gpio_set_pull(struct gpio_t * gpio, int offset, enum gpio_pull_t pull)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
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
	val = readl(dat->regbase + EXYNOS4412_GPIO_PUD);
	val &= ~(0x3 << offset);
	val |= p << offset;
	writel(dat->regbase + EXYNOS4412_GPIO_PUD, val);
}

static enum gpio_pull_t exynos4412_gpio_get_pull(struct gpio_t * gpio, int offset)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
	u32_t val, p;

	if(offset >= gpio->ngpio)
		return GPIO_PULL_NONE;

	offset <<= 0x1;
	val = readl(dat->regbase + EXYNOS4412_GPIO_PUD);
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

static void exynos4412_gpio_set_drv(struct gpio_t * gpio, int offset, enum gpio_drv_t drv)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
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
	val = readl(dat->regbase + EXYNOS4412_GPIO_DRV);
	val &= ~(0x3 << offset);
	val |= d << offset;
	writel(dat->regbase + EXYNOS4412_GPIO_DRV, val);
}

static enum gpio_drv_t exynos4412_gpio_get_drv(struct gpio_t * gpio, int offset)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
	u32_t val, d;

	if(offset >= gpio->ngpio)
		return GPIO_DRV_NONE;

	offset <<= 0x1;
	val = readl(dat->regbase + EXYNOS4412_GPIO_DRV);
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

static void exynos4412_gpio_set_rate(struct gpio_t * gpio, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t exynos4412_gpio_get_rate(struct gpio_t * gpio, int offset)
{
	return GPIO_RATE_NONE;
}

static void exynos4412_gpio_set_dir(struct gpio_t * gpio, int offset, enum gpio_direction_t dir)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return;

	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		offset <<= 0x2;
		val = readl(dat->regbase + EXYNOS4412_GPIO_CON);
		val &= ~(0xf << offset);
		writel(dat->regbase + EXYNOS4412_GPIO_CON, val);
		break;

	case GPIO_DIRECTION_OUTPUT:
		offset <<= 0x2;
		val = readl(dat->regbase + EXYNOS4412_GPIO_CON);
		val &= ~(0xf << offset);
		val |= 0x1 << offset;
		writel(dat->regbase + EXYNOS4412_GPIO_CON, val);
		break;

	default:
		break;
	}
}

static enum gpio_direction_t exynos4412_gpio_get_dir(struct gpio_t * gpio, int offset)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
	u32_t val, d;

	if(offset >= gpio->ngpio)
		return GPIO_DIRECTION_NONE;

	offset <<= 0x2;
	val = readl(dat->regbase + EXYNOS4412_GPIO_CON);
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

static void exynos4412_gpio_set_value(struct gpio_t * gpio, int offset, int value)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return;

	val = readl(dat->regbase + EXYNOS4412_GPIO_DAT);
	val &= ~(1 << offset);
	val |= (!!value) << offset;
	writel(dat->regbase + EXYNOS4412_GPIO_DAT, val);
}

static int exynos4412_gpio_get_value(struct gpio_t * gpio, int offset)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return 0;

	val = readl(dat->regbase + EXYNOS4412_GPIO_DAT);
	return !!(val & (1 << offset));
}

static struct exynos4412_gpio_data_t gpio_datas[] = {
	{
		.name		= "GPA0",
		.base		= EXYNOS4412_GPA0(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPA0_BASE,
	}, {
		.name		= "GPA1",
		.base		= EXYNOS4412_GPA1(0),
		.ngpio		= 6,
		.regbase	= EXYNOS4412_GPA1_BASE,
	}, {
		.name		= "GPB",
		.base		= EXYNOS4412_GPB(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPB_BASE,
	}, {
		.name		= "GPC0",
		.base		= EXYNOS4412_GPC0(0),
		.ngpio		= 5,
		.regbase	= EXYNOS4412_GPC0_BASE,
	}, {
		.name		= "GPC1",
		.base		= EXYNOS4412_GPC1(0),
		.ngpio		= 5,
		.regbase	= EXYNOS4412_GPC1_BASE,
	}, {
		.name		= "GPD0",
		.base		= EXYNOS4412_GPD0(0),
		.ngpio		= 4,
		.regbase	= EXYNOS4412_GPD0_BASE,
	}, {
		.name		= "GPD1",
		.base		= EXYNOS4412_GPD1(0),
		.ngpio		= 4,
		.regbase	= EXYNOS4412_GPD1_BASE,
	}, {
		.name		= "GPF0",
		.base		= EXYNOS4412_GPF0(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPF0_BASE,
	}, {
		.name		= "GPF1",
		.base		= EXYNOS4412_GPF1(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPF1_BASE,
	}, {
		.name		= "GPF2",
		.base		= EXYNOS4412_GPF2(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPF2_BASE,
	}, {
		.name		= "GPF3",
		.base		= EXYNOS4412_GPF3(0),
		.ngpio		= 6,
		.regbase	= EXYNOS4412_GPF3_BASE,
	}, {
		.name		= "GPJ0",
		.base		= EXYNOS4412_GPJ0(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPJ0_BASE,
	}, {
		.name		= "GPJ1",
		.base		= EXYNOS4412_GPJ1(0),
		.ngpio		= 5,
		.regbase	= EXYNOS4412_GPJ1_BASE,
	}, {
		.name		= "GPK0",
		.base		= EXYNOS4412_GPK0(0),
		.ngpio		= 7,
		.regbase	= EXYNOS4412_GPK0_BASE,
	}, {
		.name		= "GPK1",
		.base		= EXYNOS4412_GPK1(0),
		.ngpio		= 7,
		.regbase	= EXYNOS4412_GPK1_BASE,
	}, {
		.name		= "GPK2",
		.base		= EXYNOS4412_GPK2(0),
		.ngpio		= 7,
		.regbase	= EXYNOS4412_GPK2_BASE,
	}, {
		.name		= "GPK3",
		.base		= EXYNOS4412_GPK3(0),
		.ngpio		= 7,
		.regbase	= EXYNOS4412_GPK3_BASE,
	}, {
		.name		= "GPL0",
		.base		= EXYNOS4412_GPL0(0),
		.ngpio		= 7,
		.regbase	= EXYNOS4412_GPL0_BASE,
	}, {
		.name		= "GPL1",
		.base		= EXYNOS4412_GPL1(0),
		.ngpio		= 2,
		.regbase	= EXYNOS4412_GPL1_BASE,
	}, {
		.name		= "GPL2",
		.base		= EXYNOS4412_GPL2(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPL2_BASE,
	}, {
		.name		= "GPY0",
		.base		= EXYNOS4412_GPY0(0),
		.ngpio		= 6,
		.regbase	= EXYNOS4412_GPY0_BASE,
	}, {
		.name		= "GPY1",
		.base		= EXYNOS4412_GPY1(0),
		.ngpio		= 4,
		.regbase	= EXYNOS4412_GPY1_BASE,
	}, {
		.name		= "GPY2",
		.base		= EXYNOS4412_GPY2(0),
		.ngpio		= 6,
		.regbase	= EXYNOS4412_GPY2_BASE,
	}, {
		.name		= "GPY3",
		.base		= EXYNOS4412_GPY3(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPY3_BASE,
	}, {
		.name		= "GPY4",
		.base		= EXYNOS4412_GPY4(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPY4_BASE,
	}, {
		.name		= "GPY5",
		.base		= EXYNOS4412_GPY5(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPY5_BASE,
	}, {
		.name		= "GPY6",
		.base		= EXYNOS4412_GPY6(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPY6_BASE,
	}, {
		.name		= "GPM0",
		.base		= EXYNOS4412_GPM0(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPM0_BASE,
	}, {
		.name		= "GPM1",
		.base		= EXYNOS4412_GPM1(0),
		.ngpio		= 7,
		.regbase	= EXYNOS4412_GPM1_BASE,
	}, {
		.name		= "GPM2",
		.base		= EXYNOS4412_GPM2(0),
		.ngpio		= 5,
		.regbase	= EXYNOS4412_GPM2_BASE,
	}, {
		.name		= "GPM3",
		.base		= EXYNOS4412_GPM3(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPM3_BASE,
	}, {
		.name		= "GPM4",
		.base		= EXYNOS4412_GPM4(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPM4_BASE,
	}, {
		.name		= "GPX0",
		.base		= EXYNOS4412_GPX0(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPX0_BASE,
	}, {
		.name		= "GPX1",
		.base		= EXYNOS4412_GPX1(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPX1_BASE,
	}, {
		.name		= "GPX2",
		.base		= EXYNOS4412_GPX2(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPX2_BASE,
	}, {
		.name		= "GPX3",
		.base		= EXYNOS4412_GPX3(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPX3_BASE,
	}, {
		.name		= "GPZ",
		.base		= EXYNOS4412_GPZ(0),
		.ngpio		= 7,
		.regbase	= EXYNOS4412_GPZ_BASE,
	}, {
		.name		= "GPV0",
		.base		= EXYNOS4412_GPV0(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPV0_BASE,
	}, {
		.name		= "GPV1",
		.base		= EXYNOS4412_GPV1(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPV1_BASE,
	}, {
		.name		= "GPV2",
		.base		= EXYNOS4412_GPV2(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPV2_BASE,
	}, {
		.name		= "GPV3",
		.base		= EXYNOS4412_GPV3(0),
		.ngpio		= 8,
		.regbase	= EXYNOS4412_GPV3_BASE,
	}, {
		.name		= "GPV4",
		.base		= EXYNOS4412_GPV4(0),
		.ngpio		= 2,
		.regbase	= EXYNOS4412_GPV4_BASE,
	}
};

static __init void exynos4412_gpio_init(void)
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
		gpio->set_cfg = exynos4412_gpio_set_cfg;
		gpio->get_cfg = exynos4412_gpio_get_cfg;
		gpio->set_pull = exynos4412_gpio_set_pull;
		gpio->get_pull = exynos4412_gpio_get_pull;
		gpio->set_drv = exynos4412_gpio_set_drv;
		gpio->get_drv = exynos4412_gpio_get_drv;
		gpio->set_rate = exynos4412_gpio_set_rate;
		gpio->get_rate = exynos4412_gpio_get_rate;
		gpio->set_dir = exynos4412_gpio_set_dir;
		gpio->get_dir = exynos4412_gpio_get_dir;
		gpio->set_value = exynos4412_gpio_set_value;
		gpio->get_value = exynos4412_gpio_get_value;
		gpio->priv = &gpio_datas[i];

		if(register_gpio(gpio))
			LOG("Register gpio '%s'", gpio->name);
		else
			LOG("Failed to register gpio '%s'", gpio->name);
	}
}

static __exit void exynos4412_gpio_exit(void)
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

postcore_initcall(exynos4412_gpio_init);
postcore_exitcall(exynos4412_gpio_exit);
