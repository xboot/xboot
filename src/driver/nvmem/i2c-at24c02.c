/*
 * driver/i2c-at24c02.c
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
#include <nvmem/nvmem.h>

struct i2c_at24c02_pdata_t {
	struct i2c_device_t * dev;
	int capacity;
};

static bool_t at24c02_read_byte(struct i2c_device_t * dev, uint8_t addr, uint8_t * val)
{
	struct i2c_msg_t msgs[2];

	msgs[0].addr = dev->addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = &addr;

	msgs[1].addr = dev->addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = 1;
	msgs[1].buf = val;

	if(i2c_transfer(dev->i2c, msgs, 2) != 2)
		return FALSE;
	return TRUE;
}

static bool_t at24c02_write_byte(struct i2c_device_t * dev, uint8_t addr, uint8_t * val)
{
	struct i2c_msg_t msg;
	uint8_t buf[2];

	buf[0] = addr;
	buf[1] = *val;
	msg.addr = dev->addr;
	msg.flags = 0;
	msg.len = 2;
	msg.buf = &buf[0];

	if(i2c_transfer(dev->i2c, &msg, 1) != 1)
		return FALSE;
	return TRUE;
}

static int i2c_at24c02_capacity(struct nvmem_t * m)
{
	struct i2c_at24c02_pdata_t * pdat = (struct i2c_at24c02_pdata_t *)m->priv;
	return pdat->capacity;
}

static int i2c_at24c02_read(struct nvmem_t * m, void * buf, int offset, int count)
{
	struct i2c_at24c02_pdata_t * pdat = (struct i2c_at24c02_pdata_t *)m->priv;
	uint8_t * p = buf;
	int i;

	for(i = 0; i < count; i++)
	{
		if(!at24c02_read_byte(pdat->dev, offset + i, &p[i]))
			break;
	}
	return i;
}

static int i2c_at24c02_write(struct nvmem_t * m, void * buf, int offset, int count)
{
	struct i2c_at24c02_pdata_t * pdat = (struct i2c_at24c02_pdata_t *)m->priv;
	uint8_t * p = buf;
	int i;

	for(i = 0; i < count; i++)
	{
		if(!at24c02_write_byte(pdat->dev, offset + i, &p[i]))
			break;
	}
	return i;
}

static struct device_t * i2c_at24c02_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct i2c_at24c02_pdata_t * pdat;
	struct nvmem_t * m;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	uint8_t val;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x50), 0);
	if(!i2cdev)
		return NULL;

	if(!at24c02_read_byte(i2cdev, 0, &val))
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct i2c_at24c02_pdata_t));
	if(!pdat)
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	m = malloc(sizeof(struct nvmem_t));
	if(!m)
	{
		i2c_device_free(i2cdev);
		free(pdat);
		return NULL;
	}

	pdat->dev = i2cdev;
	pdat->capacity = 256;

	m->name = alloc_device_name(dt_read_name(n), -1);
	m->capacity = i2c_at24c02_capacity;
	m->read = i2c_at24c02_read;
	m->write = i2c_at24c02_write;
	m->priv = pdat;

	if(!register_nvmem(&dev, m))
	{
		i2c_device_free(pdat->dev);

		free_device_name(m->name);
		free(m->priv);
		free(m);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void i2c_at24c02_remove(struct device_t * dev)
{
	struct nvmem_t * m = (struct nvmem_t *)dev->priv;
	struct i2c_at24c02_pdata_t * pdat = (struct i2c_at24c02_pdata_t *)m->priv;

	if(m && unregister_nvmem(m))
	{
		i2c_device_free(pdat->dev);

		free_device_name(m->name);
		free(m->priv);
		free(m);
	}
}

static void i2c_at24c02_suspend(struct device_t * dev)
{
}

static void i2c_at24c02_resume(struct device_t * dev)
{
}

static struct driver_t i2c_at24c02 = {
	.name		= "i2c-at24c02",
	.probe		= i2c_at24c02_probe,
	.remove		= i2c_at24c02_remove,
	.suspend	= i2c_at24c02_suspend,
	.resume		= i2c_at24c02_resume,
};

static __init void i2c_at24c02_driver_init(void)
{
	register_driver(&i2c_at24c02);
}

static __exit void i2c_at24c02_driver_exit(void)
{
	unregister_driver(&i2c_at24c02);
}

driver_initcall(i2c_at24c02_driver_init);
driver_exitcall(i2c_at24c02_driver_exit);
