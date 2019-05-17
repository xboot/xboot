/*
 * driver/compass-hmc5883l.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
#include <compass/compass.h>

enum {
	REG_CFGA 	= 0x00,
	REG_CFGB	= 0x01,
	REG_MODE	= 0x02,
	REG_DATAXH 	= 0x03,
	REG_DATAXL 	= 0x04,
	REG_DATAZH 	= 0x05,
	REG_DATAZL	= 0x06,
	REG_DATAYH 	= 0x07,
	REG_DATAYL 	= 0x08,
	REG_STATUS	= 0x09,
	REG_IDA		= 0x0a,
	REG_IDB		= 0x0b,
	REG_IDC		= 0x0c,
};

struct compass_hmc5883l_pdata_t {
	struct i2c_device_t * dev;
};

static bool_t hmc5883l_read(struct i2c_device_t * dev, u8_t reg, u8_t * val)
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

static bool_t hmc5883l_write(struct i2c_device_t * dev, u8_t reg, u8_t val)
{
	struct i2c_msg_t msg;
	u8_t buf[2];

	buf[0] = reg;
	buf[1] = val;
	msg.addr = dev->addr;
	msg.flags = 0;
	msg.len = 2;
	msg.buf = &buf[0];

    if(i2c_transfer(dev->i2c, &msg, 1) != 1)
    	return FALSE;
    return TRUE;
}

static bool_t compass_hmc5883l_get(struct compass_t * c, int * x, int * y, int * z)
{
	struct compass_hmc5883l_pdata_t * pdat = (struct compass_hmc5883l_pdata_t *)c->priv;
	u8_t s, l = 0, h = 0;
	s16_t tx, ty, tz;

	if(hmc5883l_read(pdat->dev, REG_STATUS, &s) && (s & (1 << 0)))
	{
		hmc5883l_read(pdat->dev, REG_DATAXH, &h);
		hmc5883l_read(pdat->dev, REG_DATAXL, &l);
		tx = (h << 8) | (l << 0);

		hmc5883l_read(pdat->dev, REG_DATAZH, &h);
		hmc5883l_read(pdat->dev, REG_DATAZL, &l);
		tz = (h << 8) | (l << 0);

		hmc5883l_read(pdat->dev, REG_DATAYH, &h);
		hmc5883l_read(pdat->dev, REG_DATAYL, &l);
		ty = (h << 8) | (l << 0);

		*x = (s64_t)tx * 1000000 / 1090;
		*y = (s64_t)ty * 1000000 / 1090;
		*z = (s64_t)tz * 1000000 / 1090;
		return TRUE;
	}
	return FALSE;
}

static struct device_t * compass_hmc5883l_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct compass_hmc5883l_pdata_t * pdat;
	struct compass_t * c;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	u8_t ida, idb, idc;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x1e), 0);
	if(!i2cdev)
		return NULL;

	if(hmc5883l_read(i2cdev, REG_IDA, &ida)
		&& hmc5883l_read(i2cdev, REG_IDB, &idb)
		&& hmc5883l_read(i2cdev, REG_IDC, &idc)
		&& (ida == 0x48)
		&& (idb == 0x34)
		&& (idc == 0x33))
	{
		hmc5883l_write(i2cdev, REG_CFGA, 0x70);
		hmc5883l_write(i2cdev, REG_CFGB, 0x20);
		hmc5883l_write(i2cdev, REG_MODE, 0x00);
	}
	else
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct compass_hmc5883l_pdata_t));
	if(!pdat)
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	c = malloc(sizeof(struct compass_t));
	if(!c)
	{
		i2c_device_free(i2cdev);
		free(pdat);
		return NULL;
	}

	pdat->dev = i2cdev;

	c->name = alloc_device_name(dt_read_name(n), -1);
	c->ox = 0;
	c->oy = 0;
	c->oz = 0;
	c->get = compass_hmc5883l_get;
	c->priv = pdat;

	if(!register_compass(&dev, c))
	{
		i2c_device_free(pdat->dev);

		free_device_name(c->name);
		free(c->priv);
		free(c);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void compass_hmc5883l_remove(struct device_t * dev)
{
	struct compass_t * c = (struct compass_t *)dev->priv;
	struct compass_hmc5883l_pdata_t * pdat = (struct compass_hmc5883l_pdata_t *)c->priv;

	if(c && unregister_compass(c))
	{
		i2c_device_free(pdat->dev);

		free_device_name(c->name);
		free(c->priv);
		free(c);
	}
}

static void compass_hmc5883l_suspend(struct device_t * dev)
{
}

static void compass_hmc5883l_resume(struct device_t * dev)
{
}

static struct driver_t compass_hmc5883l = {
	.name		= "compass-hmc5883l",
	.probe		= compass_hmc5883l_probe,
	.remove		= compass_hmc5883l_remove,
	.suspend	= compass_hmc5883l_suspend,
	.resume		= compass_hmc5883l_resume,
};

static __init void compass_hmc5883l_driver_init(void)
{
	register_driver(&compass_hmc5883l);
}

static __exit void compass_hmc5883l_driver_exit(void)
{
	unregister_driver(&compass_hmc5883l);
}

driver_initcall(compass_hmc5883l_driver_init);
driver_exitcall(compass_hmc5883l_driver_exit);
