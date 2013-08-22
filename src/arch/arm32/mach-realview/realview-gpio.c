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
	physical_addr_t regbase;
};

void realview_gpio_cfg_pin(struct gpio_t * gpio, int offset, int cfg)
{
}

static void realview_gpio_set_pull(struct gpio_t * gpio, int offset, enum gpio_pull_t pull)
{
}

void realview_gpio_set_drv(struct gpio_t * gpio, int offset, enum gpio_drv_t drv)
{
}

void realview_gpio_set_rate(struct gpio_t * gpio, int offset, enum gpio_rate_t rate)
{
}

static void realview_gpio_direction_output(struct gpio_t * gpio, int offset, int value)
{
	struct realview_gpio_data_t * dat = (struct realview_gpio_data_t *)gpio->priv;
	u8_t dir;

	if(offset >= gpio->ngpio)
		return;

	writeb(dat->regbase + (1 << (offset + 2)), !!value << offset);
	dir = readb(dat->regbase + REALVIEW_GPIO_OFFSET_DIR);
	dir |= 1 << offset;
	writeb(dat->regbase  + REALVIEW_GPIO_OFFSET_DIR, dir);

	/*
	 * gpio value is set again, because pl061 doesn't allow to set value of
	 * a gpio pin before configuring it in OUT mode.
	 */
	writeb(dat->regbase + (1 << (offset + 2)), !!value << offset);
}

static void realview_gpio_direction_input(struct gpio_t * gpio, int offset)
{
	struct realview_gpio_data_t * dat = (struct realview_gpio_data_t *)gpio->priv;
	u8_t dir;

	if(offset >= gpio->ngpio)
		return;

	dir = readb(dat->regbase + REALVIEW_GPIO_OFFSET_DIR);
	dir &= ~(1 << offset);
	writeb(dat->regbase + REALVIEW_GPIO_OFFSET_DIR, dir);
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

static struct realview_gpio_data_t realview_gpio_datas[] = {
	{
		.regbase	= REALVIEW_GPIO0_BASE,
	}, {
		.regbase	= REALVIEW_GPIO1_BASE,
	}, {
		.regbase	= REALVIEW_GPIO2_BASE,
	},
};

static struct gpio_t realview_gpios[] = {
	{
		.name				= "GPIO0",
		.base				= 0,
		.ngpio				= 8,
		.cfg_pin			= realview_gpio_cfg_pin,
		.set_pull			= realview_gpio_set_pull,
		.set_drv			= realview_gpio_set_drv,
		.set_rate			= realview_gpio_set_rate,
		.direction_output	= realview_gpio_direction_output,
		.direction_input	= realview_gpio_direction_input,
		.set_value			= realview_gpio_set_value,
		.get_value			= realview_gpio_get_value,
		.priv				= &realview_gpio_datas[0],
	}, {
		.name				= "GPIO1",
		.base				= 8,
		.ngpio				= 8,
		.cfg_pin			= realview_gpio_cfg_pin,
		.set_pull			= realview_gpio_set_pull,
		.set_drv			= realview_gpio_set_drv,
		.set_rate			= realview_gpio_set_rate,
		.direction_output	= realview_gpio_direction_output,
		.direction_input	= realview_gpio_direction_input,
		.set_value			= realview_gpio_set_value,
		.get_value			= realview_gpio_get_value,
		.priv				= &realview_gpio_datas[1],
	}, {
		.name				= "GPIO2",
		.base				= 16,
		.ngpio				= 8,
		.cfg_pin			= realview_gpio_cfg_pin,
		.set_pull			= realview_gpio_set_pull,
		.set_drv			= realview_gpio_set_drv,
		.set_rate			= realview_gpio_set_rate,
		.direction_output	= realview_gpio_direction_output,
		.direction_input	= realview_gpio_direction_input,
		.set_value			= realview_gpio_set_value,
		.get_value			= realview_gpio_get_value,
		.priv				= &realview_gpio_datas[2],
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
