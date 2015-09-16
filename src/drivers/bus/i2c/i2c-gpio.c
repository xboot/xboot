/*
 * drivers/bus/i2c/i2c-gpio.c
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
#include <bus/i2c-gpio.h>

struct i2c_gpio_private_data_t {
	struct i2c_algo_bit_data_t bdat;
	struct i2c_gpio_data_t rdat;
};

static void i2c_gpio_setsda_dir(struct i2c_algo_bit_data_t * bdat, int state)
{
	struct i2c_gpio_data_t * rdat = (struct i2c_gpio_data_t *)bdat->priv;

	if(state)
		gpio_direction_input(rdat->sda_pin);
	else
		gpio_direction_output(rdat->sda_pin, 0);
}

static void i2c_gpio_setsda_val(struct i2c_algo_bit_data_t * bdat, int state)
{
	struct i2c_gpio_data_t * rdat = (struct i2c_gpio_data_t *)bdat->priv;

	gpio_set_value(rdat->sda_pin, state);
}

static void i2c_gpio_setscl_dir(struct i2c_algo_bit_data_t * bdat, int state)
{
	struct i2c_gpio_data_t * rdat = (struct i2c_gpio_data_t *)bdat->priv;

	if(state)
		gpio_direction_input(rdat->scl_pin);
	else
		gpio_direction_output(rdat->scl_pin, 0);
}

static void i2c_gpio_setscl_val(struct i2c_algo_bit_data_t * bdat, int state)
{
	struct i2c_gpio_data_t * rdat = (struct i2c_gpio_data_t *)bdat->priv;

	gpio_set_value(rdat->scl_pin, state);
}

static int i2c_gpio_getsda(struct i2c_algo_bit_data_t * bdat)
{
	struct i2c_gpio_data_t * rdat = (struct i2c_gpio_data_t *)bdat->priv;

	return gpio_get_value(rdat->sda_pin);
}

static int i2c_gpio_getscl(struct i2c_algo_bit_data_t * bdat)
{
	struct i2c_gpio_data_t * rdat = (struct i2c_gpio_data_t *)bdat->priv;

	return gpio_get_value(rdat->scl_pin);
}

static void i2c_gpio_init(struct i2c_t * i2c)
{
}

static void i2c_gpio_exit(struct i2c_t * i2c)
{
}

static int i2c_gpio_xfer(struct i2c_t * i2c, struct i2c_msg_t * msgs, int num)
{
	struct i2c_gpio_private_data_t * dat = (struct i2c_gpio_private_data_t *)i2c->priv;
	struct i2c_algo_bit_data_t * bdat = &(dat->bdat);

	return i2c_algo_bit_xfer(bdat, msgs, num);
}

static bool_t i2c_gpio_register_bus(struct resource_t * res)
{
	struct i2c_gpio_data_t * rdat = (struct i2c_gpio_data_t *)res->data;
	struct i2c_gpio_private_data_t * dat;
	struct i2c_t * i2c;
	char name[64];

	dat = malloc(sizeof(struct i2c_gpio_private_data_t));
	if(!dat)
		return FALSE;

	i2c = malloc(sizeof(struct i2c_t));
	if(!i2c)
	{
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	if(rdat->sda_is_open_drain)
	{
		gpio_direction_output(rdat->sda_pin, 1);
		dat->bdat.setsda = i2c_gpio_setsda_val;
	}
	else
	{
		gpio_direction_input(rdat->sda_pin);
		dat->bdat.setsda = i2c_gpio_setsda_dir;
	}

	if(rdat->scl_is_open_drain || rdat->scl_is_output_only)
	{
		gpio_direction_output(rdat->scl_pin, 1);
		dat->bdat.setscl = i2c_gpio_setscl_val;
	}
	else
	{
		gpio_direction_input(rdat->scl_pin);
		dat->bdat.setscl = i2c_gpio_setscl_dir;
	}

	dat->bdat.getsda = i2c_gpio_getsda;
	if(rdat->scl_is_output_only)
		dat->bdat.getscl = 0;
	else
		dat->bdat.getscl = i2c_gpio_getscl;

	if(rdat->udelay > 0)
		dat->bdat.udelay = rdat->udelay;
	else if(rdat->scl_is_output_only)
		dat->bdat.udelay = 50;
	else
		dat->bdat.udelay = 5;

	memcpy(&(dat->rdat), rdat, sizeof(struct i2c_gpio_data_t));
	dat->bdat.priv = &(dat->rdat);

	i2c->name = strdup(name);
	i2c->init = i2c_gpio_init;
	i2c->exit = i2c_gpio_exit;
	i2c->xfer = i2c_gpio_xfer,
	i2c->priv = dat;

	if(register_bus_i2c(i2c))
		return TRUE;

	free(i2c->priv);
	free(i2c->name);
	free(i2c);
	return FALSE;
}

static bool_t i2c_gpio_unregister_bus(struct resource_t * res)
{
	struct i2c_t * i2c;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	i2c = search_bus_i2c(name);
	if(!i2c)
		return FALSE;

	if(!unregister_bus_i2c(i2c))
		return FALSE;

	free(i2c->priv);
	free(i2c->name);
	free(i2c);
	return TRUE;
}

static __init void i2c_gpio_bus_init(void)
{
	resource_for_each_with_name("i2c-gpio", i2c_gpio_register_bus);
}

static __exit void i2c_gpio_bus_exit(void)
{
	resource_for_each_with_name("i2c-gpio", i2c_gpio_unregister_bus);
}

bus_initcall(i2c_gpio_bus_init);
bus_exitcall(i2c_gpio_bus_exit);
