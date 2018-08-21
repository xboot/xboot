/*
 * driver/i2c/i2c.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <i2c/i2c.h>

static bool_t detect(struct i2c_device_t * dev, u8_t reg, u8_t * val)
{
	struct i2c_msg_t msgs[2];
	u8_t buf;

	msgs[0].addr = dev->addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = &reg;

	msgs[1].addr = dev->addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = 1;
	msgs[1].buf = &buf;

	if(i2c_transfer(dev->i2c, msgs, 2) != 2)
		return FALSE;
	if(val)
		*val = buf;
	return TRUE;
}

static ssize_t i2c_read_detect(struct kobj_t * kobj, void * buf, size_t size)
{
	struct i2c_t * i2c = (struct i2c_t *)kobj->priv;
	struct i2c_device_t * i2cdev;
	char * p = buf;
	int len = 0;
	int i, j, a;

	len += sprintf((char *)(p + len), "     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f");
	for(j = 0; j < 8; j++)
	{
		len += sprintf((char *)(p + len), "\r\n%02x:", (j << 4) & 0xff);
		for(i = 0; i < 16; i++)
		{
			a = ((j << 4) | (i << 0)) & 0xff;
			if((a >= 0x08) && (a <= 0x77))
			{
				i2cdev = i2c_device_alloc(i2c->name, a, 0);
				if(i2cdev && detect(i2cdev, 0, 0))
					len += sprintf((char *)(p + len), " %02x", a);
				else
					len += sprintf((char *)(p + len), " --");
				i2c_device_free(i2cdev);
			}
			else
			{
				len += sprintf((char *)(p + len), "   ");
			}
		}
	}
	return len;
}

struct i2c_t * search_i2c(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_I2C);
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
	kobj_add_regular(dev->kobj, "detect", i2c_read_detect, NULL, i2c);

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

	dev = search_device(i2c->name, DEVICE_TYPE_I2C);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

struct i2c_device_t * i2c_device_alloc(const char * i2cbus, int addr, int flags)
{
	struct i2c_device_t * dev;
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

	dev = malloc(sizeof(struct i2c_device_t));
	if(!dev)
		return NULL;

	dev->i2c = i2c;
	dev->addr = addr;
	dev->flags = flags;

	return dev;
}

void i2c_device_free(struct i2c_device_t * dev)
{
	if(dev)
		free(dev);
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

int i2c_master_send(const struct i2c_device_t * dev, void * buf, int count)
{
	struct i2c_msg_t msg;
	int ret;

	msg.addr = dev->addr;
	msg.flags = dev->flags & I2C_M_TEN;
	msg.len = count;
	msg.buf = buf;

	ret = i2c_transfer(dev->i2c, &msg, 1);
	return (ret == 1) ? count : ret;
}

int i2c_master_recv(const struct i2c_device_t * dev, void * buf, int count)
{
	struct i2c_msg_t msg;
	int ret;

	msg.addr = dev->addr;
	msg.flags = dev->flags & I2C_M_TEN;
	msg.flags |= I2C_M_RD;
	msg.len = count;
	msg.buf = buf;

	ret = i2c_transfer(dev->i2c, &msg, 1);
	return (ret == 1) ? count : ret;
}
