/*
 * bus/realview-i2c.c
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
#include <realview-i2c.h>

struct realview_i2c_private_data_t {
	struct i2c_algo_bit_data_t bdat;
	struct realview_i2c_data_t rdat;
};

static void realview_i2c_setsda(struct i2c_algo_bit_data_t * bdat, int state)
{
	struct realview_i2c_data_t * rdat = (struct realview_i2c_data_t *)bdat->priv;
	write32(rdat->regbase + (state ? REALVIEW_I2C_CTRLS : REALVIEW_I2C_CTRLC), REALVIEW_I2C_FLAG_SDA);
}

static void realview_i2c_setscl(struct i2c_algo_bit_data_t * bdat, int state)
{
	struct realview_i2c_data_t * rdat = (struct realview_i2c_data_t *)bdat->priv;
	write32(rdat->regbase + (state ? REALVIEW_I2C_CTRLS : REALVIEW_I2C_CTRLC), REALVIEW_I2C_FLAG_SCL);
}

static int realview_i2c_getsda(struct i2c_algo_bit_data_t * bdat)
{
	struct realview_i2c_data_t * rdat = (struct realview_i2c_data_t *)bdat->priv;
	return !!(read32(rdat->regbase + REALVIEW_I2C_CTRL) & REALVIEW_I2C_FLAG_SDA);
}

static int realview_i2c_getscl(struct i2c_algo_bit_data_t * bdat)
{
	struct realview_i2c_data_t * rdat = (struct realview_i2c_data_t *)bdat->priv;
	return !!(read32(rdat->regbase + REALVIEW_I2C_CTRL) & REALVIEW_I2C_FLAG_SCL);
}

static void realview_i2c_init(struct i2c_t * i2c)
{
}

static void realview_i2c_exit(struct i2c_t * i2c)
{
}

static int realview_i2c_xfer(struct i2c_t * i2c, struct i2c_msg_t * msgs, int num)
{
	struct realview_i2c_private_data_t * dat = (struct realview_i2c_private_data_t *)i2c->priv;
	struct i2c_algo_bit_data_t * bdat = (struct i2c_algo_bit_data_t *)(&dat->bdat);
	return i2c_algo_bit_xfer(bdat, msgs, num);
}

static bool_t realview_register_i2c_bus(struct resource_t * res)
{
	struct realview_i2c_data_t * rdat = (struct realview_i2c_data_t *)res->data;
	struct realview_i2c_private_data_t * dat;
	struct i2c_t * i2c;
	char name[64];

	dat = malloc(sizeof(struct realview_i2c_private_data_t));
	if(!dat)
		return FALSE;

	i2c = malloc(sizeof(struct i2c_t));
	if(!i2c)
	{
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	dat->bdat.setsda = realview_i2c_setsda;
	dat->bdat.setscl = realview_i2c_setscl;
	dat->bdat.getsda = realview_i2c_getsda;
	dat->bdat.getscl = realview_i2c_getscl;
	if(rdat->udelay > 0)
		dat->bdat.udelay = rdat->udelay;
	else
		dat->bdat.udelay = 5;
	memcpy(&(dat->rdat), rdat, sizeof(struct realview_i2c_data_t));
	dat->bdat.priv = &(dat->rdat);

	i2c->name = strdup(name);
	i2c->init = realview_i2c_init;
	i2c->exit = realview_i2c_exit;
	i2c->xfer = realview_i2c_xfer,
	i2c->priv = dat;

	if(register_bus_i2c(i2c))
		return TRUE;

	free(i2c->priv);
	free(i2c->name);
	free(i2c);
	return FALSE;
}

static bool_t realview_unregister_i2c_bus(struct resource_t * res)
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

static __init void realview_i2c_bus_init(void)
{
	resource_for_each_with_name("realview-i2c", realview_register_i2c_bus);
}

static __exit void realview_i2c_bus_exit(void)
{
	resource_for_each_with_name("realview-i2c", realview_unregister_i2c_bus);
}

bus_initcall(realview_i2c_bus_init);
bus_exitcall(realview_i2c_bus_exit);
