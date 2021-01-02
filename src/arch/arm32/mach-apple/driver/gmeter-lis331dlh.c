/*
 * driver/gmeter-lis331dlh.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
#include <gmeter/gmeter.h>

enum {
	REG_WHOAMI			= 0x0f,
	REG_CTRL1			= 0x20,
	REG_CTRL2			= 0x21,
	REG_CTRL3			= 0x22,
	REG_CTRL4			= 0x23,
	REG_CTRL5			= 0x24,
	REG_REFERENCE		= 0x26,
	REG_STATUS			= 0x27,
	REG_OUTX_L			= 0x28,
	REG_OUTX_H			= 0x29,
	REG_OUTY_L			= 0x2a,
	REG_OUTY_H			= 0x2b,
	REG_OUTZ_L			= 0x2c,
	REG_OUTZ_H			= 0x2d,
	REG_INT1_CFG		= 0x30,
	REG_INT1_SOURCE		= 0x31,
	REG_INT1_THS		= 0x32,
	REG_INT1_DURATION	= 0x33,
	REG_INT2_CFG		= 0x34,
	REG_INT2_SOURCE		= 0x35,
	REG_INT2_THS		= 0x36,
	REG_INT2_DURATION	= 0x37,
};

struct gmeter_lis331dlh_pdata_t {
	struct i2c_device_t * dev;
};

static bool_t lis331dlh_read(struct i2c_device_t * dev, u8_t reg, u8_t * val)
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

static bool_t lis331dlh_write(struct i2c_device_t * dev, u8_t reg, u8_t val)
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

static bool_t gmeter_lis331dlh_get(struct gmeter_t * g, int * x, int * y, int * z)
{
	struct gmeter_lis331dlh_pdata_t * pdat = (struct gmeter_lis331dlh_pdata_t *)g->priv;
	u8_t s, l = 0, h = 0;
	s16_t tx, ty, tz;

	if(lis331dlh_read(pdat->dev, REG_STATUS, &s) && (s & (1 << 3)))
	{
		lis331dlh_read(pdat->dev, REG_OUTX_L, &l);
		lis331dlh_read(pdat->dev, REG_OUTX_H, &h);
		tx = (h << 8) | (l << 0);

		lis331dlh_read(pdat->dev, REG_OUTY_L, &l);
		lis331dlh_read(pdat->dev, REG_OUTY_H, &h);
		ty = (h << 8) | (l << 0);

		lis331dlh_read(pdat->dev, REG_OUTZ_L, &l);
		lis331dlh_read(pdat->dev, REG_OUTZ_H, &h);
		tz = (h << 8) | (l << 0);

		*x = (s64_t)tx * 2 * 9806650 / 32768;
		*y = (s64_t)ty * 2 * 9806650 / 32768;
		*z = (s64_t)tz * 2 * 9806650 / 32768;
		return TRUE;
	}
	return FALSE;
}

static struct device_t * gmeter_lis331dlh_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct gmeter_lis331dlh_pdata_t * pdat;
	struct gmeter_t * g;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	u8_t val;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x18), 0);
	if(!i2cdev)
		return NULL;

	if(lis331dlh_read(i2cdev, REG_WHOAMI, &val) && (val == 0x32))
	{
		lis331dlh_write(i2cdev, REG_CTRL2, (1 << 7));
		lis331dlh_write(i2cdev, REG_CTRL1, (1 << 5) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0));
		lis331dlh_write(i2cdev, REG_CTRL2, (0 << 7));
	}
	else
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct gmeter_lis331dlh_pdata_t));
	if(!pdat)
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	g = malloc(sizeof(struct gmeter_t));
	if(!g)
	{
		i2c_device_free(i2cdev);
		free(pdat);
		return NULL;
	}

	pdat->dev = i2cdev;

	g->name = alloc_device_name(dt_read_name(n), -1);
	g->get = gmeter_lis331dlh_get;
	g->priv = pdat;

	if(!(dev = register_gmeter(g, drv)))
	{
		i2c_device_free(pdat->dev);
		free_device_name(g->name);
		free(g->priv);
		free(g);
		return NULL;
	}
	return dev;
}

static void gmeter_lis331dlh_remove(struct device_t * dev)
{
	struct gmeter_t * g = (struct gmeter_t *)dev->priv;
	struct gmeter_lis331dlh_pdata_t * pdat = (struct gmeter_lis331dlh_pdata_t *)g->priv;

	if(g)
	{
		unregister_gmeter(g);
		i2c_device_free(pdat->dev);
		free_device_name(g->name);
		free(g->priv);
		free(g);
	}
}

static void gmeter_lis331dlh_suspend(struct device_t * dev)
{
}

static void gmeter_lis331dlh_resume(struct device_t * dev)
{
}

static struct driver_t gmeter_lis331dlh = {
	.name		= "gmeter-lis331dlh",
	.probe		= gmeter_lis331dlh_probe,
	.remove		= gmeter_lis331dlh_remove,
	.suspend	= gmeter_lis331dlh_suspend,
	.resume		= gmeter_lis331dlh_resume,
};

static __init void gmeter_lis331dlh_driver_init(void)
{
	register_driver(&gmeter_lis331dlh);
}

static __exit void gmeter_lis331dlh_driver_exit(void)
{
	unregister_driver(&gmeter_lis331dlh);
}

driver_initcall(gmeter_lis331dlh_driver_init);
driver_exitcall(gmeter_lis331dlh_driver_exit);
