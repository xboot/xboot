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

static void gpio_pull(struct gpio_t * gpio, int offset, enum gpio_pull_t mode)
{
}

static void gpio_input(struct gpio_t * gpio, int offset)
{
	u8_t dir;

	if(offset >= gpio->ngpio)
		return;

	dir = readb(gpio->regbase + REALVIEW_GPIO_OFFSET_DIR);
	dir &= ~(1 << offset);
	writeb(gpio->regbase + REALVIEW_GPIO_OFFSET_DIR, dir);
}

static void gpio_output(struct gpio_t * gpio, int offset, int value)
{
	u8_t dir;

	if(offset >= gpio->ngpio)
		return;

	writeb(gpio->regbase + (1 << (offset + 2)), !!value << offset);
	dir = readb(gpio->regbase + REALVIEW_GPIO_OFFSET_DIR);
	dir |= 1 << offset;
	writeb(gpio->regbase  + REALVIEW_GPIO_OFFSET_DIR, dir);

	/*
	 * gpio value is set again, because pl061 doesn't allow to set value of
	 * a gpio pin before configuring it in OUT mode.
	 */
	writeb(gpio->regbase + (1 << (offset + 2)), !!value << offset);
}

static void gpio_set(struct gpio_t * gpio, int offset, int value)
{
	writeb(gpio->regbase + (1 << (offset + 2)), !!value << offset);
}

static int gpio_get(struct gpio_t * gpio, int offset)
{
	return !!readb(gpio->regbase + (1 << (offset + 2)));
}

static struct gpio_t realview_gpios[] = {
	{
		.name		= "GPIO0",
		.base		= 0,
		.ngpio		= 8,
		.pull		= gpio_pull,
		.input		= gpio_input,
		.output		= gpio_output,
		.set		= gpio_set,
		.get		= gpio_get,
		.regbase	= REALVIEW_GPIO0_BASE,
	}, {
		.name		= "GPIO1",
		.base		= 8,
		.ngpio		= 8,
		.pull		= gpio_pull,
		.input		= gpio_input,
		.output		= gpio_output,
		.set		= gpio_set,
		.get		= gpio_get,
		.regbase	= REALVIEW_GPIO1_BASE,
	}, {
		.name		= "GPIO2",
		.base		= 16,
		.ngpio		= 8,
		.pull		= gpio_pull,
		.input		= gpio_input,
		.output		= gpio_output,
		.set		= gpio_set,
		.get		= gpio_get,
		.regbase	= REALVIEW_GPIO2_BASE,
	},
};

static __init void realview_gpio_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(realview_gpios); i++)
	{
		if(register_gpio(&realview_gpios[i]))
			LOG("Register gpio '%s'", realview_gpios[i].name);
		else
			LOG("Failed to register gpio '%s'", realview_gpios[i].name);
	}
}

static __exit void realview_gpio_exit(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(realview_gpios); i++)
	{
		if(register_gpio(&realview_gpios[i]))
			LOG("Unregister gpio '%s'", realview_gpios[i].name);
		else
			LOG("Failed to unregister gpio '%s'", realview_gpios[i].name);
	}
}

postcore_initcall(realview_gpio_init);
postcore_exitcall(realview_gpio_exit);
