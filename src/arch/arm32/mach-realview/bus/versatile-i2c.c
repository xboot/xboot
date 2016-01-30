/*
 * bus/versatile-i2c.c
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

#include <versatile-i2c.h>

#define I2C_CONTROL		(0x00)
#define I2C_CONTROLS	(0x00)
#define I2C_CONTROLC	(0x04)
#define SCL				(1 << 0)
#define SDA				(1 << 1)

struct versatile_i2c_private_data_t {
	struct i2c_algo_bit_data_t bdat;
	virtual_addr_t regbase;
};

static void versatile_i2c_setsda(struct i2c_algo_bit_data_t * bdat, int state)
{
	struct versatile_i2c_private_data_t * dat = (struct versatile_i2c_private_data_t *)bdat->priv;
	write32(dat->regbase + (state ? I2C_CONTROLS : I2C_CONTROLC), SDA);
}

static void versatile_i2c_setscl(struct i2c_algo_bit_data_t * bdat, int state)
{
	struct versatile_i2c_private_data_t * dat = (struct versatile_i2c_private_data_t *)bdat->priv;
	write32(dat->regbase + (state ? I2C_CONTROLS : I2C_CONTROLC), SCL);
}

static int versatile_i2c_getsda(struct i2c_algo_bit_data_t * bdat)
{
	struct versatile_i2c_private_data_t * dat = (struct versatile_i2c_private_data_t *)bdat->priv;
	return !!(read32(dat->regbase + I2C_CONTROL) & SDA);
}

static int versatile_i2c_getscl(struct i2c_algo_bit_data_t * bdat)
{
	struct versatile_i2c_private_data_t * dat = (struct versatile_i2c_private_data_t *)bdat->priv;
	return !!(read32(dat->regbase + I2C_CONTROL) & SCL);
}

static void versatile_i2c_init(struct i2c_t * i2c)
{
}

static void versatile_i2c_exit(struct i2c_t * i2c)
{
}

static int versatile_i2c_xfer(struct i2c_t * i2c, struct i2c_msg_t * msgs, int num)
{
	struct versatile_i2c_private_data_t * dat = (struct versatile_i2c_private_data_t *)i2c->priv;
	struct i2c_algo_bit_data_t * bdat = (struct i2c_algo_bit_data_t *)(&dat->bdat);
	return i2c_algo_bit_xfer(bdat, msgs, num);
}

static bool_t versatile_register_i2c_bus(struct resource_t * res)
{
	struct versatile_i2c_data_t * rdat = (struct versatile_i2c_data_t *)res->data;
	struct versatile_i2c_private_data_t * dat;
	struct i2c_t * i2c;
	char name[64];

	dat = malloc(sizeof(struct versatile_i2c_private_data_t));
	if(!dat)
		return FALSE;

	i2c = malloc(sizeof(struct i2c_t));
	if(!i2c)
	{
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	dat->bdat.setsda = versatile_i2c_setsda;
	dat->bdat.setscl = versatile_i2c_setscl;
	dat->bdat.getsda = versatile_i2c_getsda;
	dat->bdat.getscl = versatile_i2c_getscl;
	if(rdat->udelay > 0)
		dat->bdat.udelay = rdat->udelay;
	else
		dat->bdat.udelay = 5;
	dat->regbase = phys_to_virt(rdat->regbase);
	dat->bdat.priv = dat;

	i2c->name = strdup(name);
	i2c->init = versatile_i2c_init;
	i2c->exit = versatile_i2c_exit;
	i2c->xfer = versatile_i2c_xfer,
	i2c->priv = dat;

	if(register_bus_i2c(i2c))
		return TRUE;

	free(i2c->priv);
	free(i2c->name);
	free(i2c);
	return FALSE;
}

static bool_t versatile_unregister_i2c_bus(struct resource_t * res)
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

static __init void versatile_i2c_bus_init(void)
{
	resource_for_each_with_name("versatile-i2c", versatile_register_i2c_bus);
}

static __exit void versatile_i2c_bus_exit(void)
{
	resource_for_each_with_name("versatile-i2c", versatile_unregister_i2c_bus);
}

bus_initcall(versatile_i2c_bus_init);
bus_exitcall(versatile_i2c_bus_exit);
