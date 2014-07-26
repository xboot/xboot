/*
 * realview-gpio.c
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
#include <realview/reg-gpio.h>

struct realview_gpio_data_t
{
	const char * name;
	int base;
	int ngpio;

	physical_addr_t regbase;
};

static void realview_gpio_set_cfg(struct gpio_t * gpio, int offset, int cfg)
{
}

static int realview_gpio_get_cfg(struct gpio_t * gpio, int offset)
{
	return 0;
}

static void realview_gpio_set_pull(struct gpio_t * gpio, int offset, enum gpio_pull_t pull)
{
}

static enum gpio_pull_t realview_gpio_get_pull(struct gpio_t * gpio, int offset)
{
	return GPIO_PULL_NONE;
}

static void realview_gpio_set_drv(struct gpio_t * gpio, int offset, enum gpio_drv_t drv)
{
}

static enum gpio_drv_t realview_gpio_get_drv(struct gpio_t * gpio, int offset)
{
	return GPIO_DRV_NONE;
}

static void realview_gpio_set_rate(struct gpio_t * gpio, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t realview_gpio_get_rate(struct gpio_t * gpio, int offset)
{
	return GPIO_RATE_NONE;
}

static void realview_gpio_set_dir(struct gpio_t * gpio, int offset, enum gpio_direction_t dir)
{
	struct realview_gpio_data_t * dat = (struct realview_gpio_data_t *)gpio->priv;
	u8_t val;

	if(offset >= gpio->ngpio)
		return;

	switch(dir)
	{
	case GPIO_DIRECTION_NONE:
		break;

	case GPIO_DIRECTION_INPUT:
		val = readb(dat->regbase + REALVIEW_GPIO_DIR);
		val &= ~(1 << offset);
		writeb(dat->regbase  + REALVIEW_GPIO_DIR, val);
		break;

	case GPIO_DIRECTION_OUTPUT:
		val = readb(dat->regbase + REALVIEW_GPIO_DIR);
		val |= 1 << offset;
		writeb(dat->regbase  + REALVIEW_GPIO_DIR, val);
		break;

	default:
		break;
	}
}

static enum gpio_direction_t realview_gpio_get_dir(struct gpio_t * gpio, int offset)
{
	struct realview_gpio_data_t * dat = (struct realview_gpio_data_t *)gpio->priv;
	u8_t val;

	if(offset >= gpio->ngpio)
		return GPIO_DIRECTION_NONE;

	val = readb(dat->regbase + REALVIEW_GPIO_DIR);
	if((val & (1 << offset)) == 0)
		return GPIO_DIRECTION_INPUT;
	return GPIO_DIRECTION_OUTPUT;
}

static void realview_gpio_set_value(struct gpio_t * gpio, int offset, int value)
{
	struct realview_gpio_data_t * dat = (struct realview_gpio_data_t *)gpio->priv;
	writeb(dat->regbase + (1 << (offset + 2)), !!value << offset);
}

static int realview_gpio_get_value(struct gpio_t * gpio, int offset)
{
	struct realview_gpio_data_t * dat = (struct realview_gpio_data_t *)gpio->priv;
	return !!readb(dat->regbase + (1 << (offset + 2)));
}

static struct realview_gpio_data_t gpio_datas[] = {
	{
		.name		= "GPIO0",
		.base		= REALVIEW_GPIO0(0),
		.ngpio		= 8,
		.regbase	= REALVIEW_GPIO0_BASE,
	}, {
		.name		= "GPIO1",
		.base		= REALVIEW_GPIO1(0),
		.ngpio		= 8,
		.regbase	= REALVIEW_GPIO1_BASE,
	}, {
		.name		= "GPIO2",
		.base		= REALVIEW_GPIO2(0),
		.ngpio		= 8,
		.regbase	= REALVIEW_GPIO2_BASE,
	},
};

static __init void realview_gpio_init(void)
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
		gpio->set_cfg = realview_gpio_set_cfg;
		gpio->get_cfg = realview_gpio_get_cfg;
		gpio->set_pull = realview_gpio_set_pull;
		gpio->get_pull = realview_gpio_get_pull;
		gpio->set_drv = realview_gpio_set_drv;
		gpio->get_drv = realview_gpio_get_drv;
		gpio->set_rate = realview_gpio_set_rate;
		gpio->get_rate = realview_gpio_get_rate;
		gpio->set_dir = realview_gpio_set_dir;
		gpio->get_dir = realview_gpio_get_dir;
		gpio->set_value = realview_gpio_set_value;
		gpio->get_value = realview_gpio_get_value;
		gpio->priv = &gpio_datas[i];

		if(register_gpio(gpio))
			LOG("Register gpio '%s'", gpio->name);
		else
			LOG("Failed to register gpio '%s'", gpio->name);
	}
}

static __exit void realview_gpio_exit(void)
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

core_initcall(realview_gpio_init);
core_exitcall(realview_gpio_exit);
