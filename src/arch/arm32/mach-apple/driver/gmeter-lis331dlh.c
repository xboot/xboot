/*
 * driver/gmeter-lis331dlh.c
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

static bool_t gmeter_lis331dlh_get(struct gmeter_t * gmeter, int * x, int * y, int * z)
{
	struct gmeter_lis331dlh_pdata_t * pdat = (struct gmeter_lis331dlh_pdata_t *)gmeter->priv;
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
	struct gmeter_t * gmeter;
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

	gmeter = malloc(sizeof(struct gmeter_t));
	if(!gmeter)
	{
		i2c_device_free(i2cdev);
		free(pdat);
		return NULL;
	}

	pdat->dev = i2cdev;

	gmeter->name = alloc_device_name(dt_read_name(n), -1);
	gmeter->get = gmeter_lis331dlh_get,
	gmeter->priv = pdat;

	if(!register_gmeter(&dev, gmeter))
	{
		i2c_device_free(pdat->dev);

		free_device_name(gmeter->name);
		free(gmeter->priv);
		free(gmeter);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void gmeter_lis331dlh_remove(struct device_t * dev)
{
	struct gmeter_t * gmeter = (struct gmeter_t *)dev->priv;
	struct gmeter_lis331dlh_pdata_t * pdat = (struct gmeter_lis331dlh_pdata_t *)gmeter->priv;

	if(gmeter && unregister_gmeter(gmeter))
	{
		i2c_device_free(pdat->dev);

		free_device_name(gmeter->name);
		free(gmeter->priv);
		free(gmeter);
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
