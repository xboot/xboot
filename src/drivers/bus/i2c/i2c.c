/*
 * drivers/bus/i2c/i2c.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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
#include <bus/i2c.h>

struct i2c_t * search_bus_i2c(const char * name)
{
	struct bus_t * bus;

	bus = search_bus_with_type(name, BUS_TYPE_I2C);
	if(!bus)
		return NULL;

	return (struct i2c_t *)bus->driver;
}

bool_t register_bus_i2c(struct i2c_t * i2c)
{
	struct bus_t * bus;

	if(!i2c || !i2c->name)
		return FALSE;

	bus = malloc(sizeof(struct bus_t));
	if(!bus)
		return FALSE;

	bus->name = strdup(i2c->name);
	bus->type = BUS_TYPE_I2C;
	bus->driver = i2c;
	bus->kobj = kobj_alloc_directory(bus->name);

	if(!register_bus(bus))
	{
		kobj_remove_self(bus->kobj);
		free(bus->name);
		free(bus);
		return FALSE;
	}

	if(i2c->init)
		(i2c->init)(i2c);

	return TRUE;
}

bool_t unregister_bus_i2c(struct i2c_t * i2c)
{
	struct bus_t * bus;
	struct i2c_t * driver;

	if(!i2c || !i2c->name)
		return FALSE;

	bus = search_bus_with_type(i2c->name, BUS_TYPE_I2C);
	if(!bus)
		return FALSE;

	driver = (struct i2c_t *)(bus->driver);
	if(driver && driver->exit)
		(driver->exit)(i2c);

	if(!unregister_bus(bus))
		return FALSE;

	kobj_remove_self(bus->kobj);
	free(bus->name);
	free(bus);

	return TRUE;
}

struct i2c_client_t * i2c_client_alloc(const char * i2cbus, u32_t addr, u32_t flags)
{
	struct i2c_client_t * client;
	struct i2c_t * i2c;

	i2c = search_bus_i2c(i2cbus);
	if(!i2c)
		return NULL;

	if(flags & I2C_M_TEN)
	{
		/* 10-bit address, all values are valid */
		if(addr > 0x3ff)
			return NULL;
	}
	else
	{
		/*
		 * 7-bit address, reject the general call address
		 *
		 * Reserved addresses per I2C specification:
		 *  0x00       General call address / START byte
		 *  0x01       CBUS address
		 *  0x02       Reserved for different bus format
		 *  0x03       Reserved for future purposes
		 *  0x04-0x07  Hs-mode master code
		 *  0x78-0x7b  10-bit slave addressing
		 *  0x7c-0x7f  Reserved for future purposes
		 */
		if(addr < 0x08 || addr > 0x77)
			return NULL;
	}

	client = malloc(sizeof(struct i2c_client_t));
	if(!client)
		return NULL;

	client->i2c = i2c;
	client->addr = addr;
	client->flags = flags;

	return client;
}

void i2c_client_free(struct i2c_client_t * client)
{
	if(client)
		free(client);
}

int i2c_transfer(struct i2c_t * i2c, struct i2c_msg_t * msgs, int num)
{
	int try, ret = 0;

	if(!i2c || !i2c->xfer)
		return -1;

	for(try = 0; try < 3; try++)
	{
		ret = i2c->xfer(i2c, msgs, num);
		if(ret >= 0)
			break;
	}

	return ret;
}

int i2c_master_send(const struct i2c_client_t * client, const char * buf, int count)
{
	struct i2c_msg_t msg;
	int ret;

	msg.addr = client->addr;
	msg.flags = client->flags & I2C_M_TEN;
	msg.len = count;
	msg.buf = (u8_t *)buf;

	ret = i2c_transfer(client->i2c, &msg, 1);
	return (ret == 1) ? count : ret;
}

int i2c_master_recv(const struct i2c_client_t * client, char * buf, int count)
{
	struct i2c_msg_t msg;
	int ret;

	msg.addr = client->addr;
	msg.flags = client->flags & I2C_M_TEN;
	msg.flags |= I2C_M_RD;
	msg.len = count;
	msg.buf = (u8_t *)buf;

	ret = i2c_transfer(client->i2c, &msg, 1);
	return (ret == 1) ? count : ret;
}
