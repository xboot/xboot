/*
 * drivers/i2c/i2c.c
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

#include <i2c/i2c.h>

struct i2c_t * search_i2c(const char * name)
{
	struct device_t * dev;

	dev = search_device_with_type(name, DEVICE_TYPE_I2C);
	if(!dev)
		return NULL;

	return (struct i2c_t *)dev->priv;
}

bool_t register_i2c(struct device_t ** device, struct i2c_t * i2c)
{
	struct device_t * dev;

	if(!i2c || !i2c->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(i2c->name);
	dev->type = DEVICE_TYPE_I2C;
	dev->priv = i2c;
	dev->kobj = kobj_alloc_directory(dev->name);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_i2c(struct i2c_t * i2c)
{
	struct device_t * dev;

	if(!i2c || !i2c->name)
		return FALSE;

	dev = search_device_with_type(i2c->name, DEVICE_TYPE_I2C);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

struct i2c_client_t * i2c_client_alloc(const char * i2cbus, int addr, int flags)
{
	struct i2c_client_t * client;
	struct i2c_t * i2c;

	i2c = search_i2c(i2cbus);
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

int i2c_master_send(const struct i2c_client_t * client, void * buf, int count)
{
	struct i2c_msg_t msg;
	int ret;

	msg.addr = client->addr;
	msg.flags = client->flags & I2C_M_TEN;
	msg.len = count;
	msg.buf = buf;

	ret = i2c_transfer(client->i2c, &msg, 1);
	return (ret == 1) ? count : ret;
}

int i2c_master_recv(const struct i2c_client_t * client, void * buf, int count)
{
	struct i2c_msg_t msg;
	int ret;

	msg.addr = client->addr;
	msg.flags = client->flags & I2C_M_TEN;
	msg.flags |= I2C_M_RD;
	msg.len = count;
	msg.buf = buf;

	ret = i2c_transfer(client->i2c, &msg, 1);
	return (ret == 1) ? count : ret;
}
