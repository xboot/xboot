/*
 * driver/gmeter-axdl345.c
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
#include <gmeter/gmeter.h>

enum {
	REG_DEVID 			= 0x00,
	REG_THRESH_TAP		= 0x1d,
	REG_OFSX			= 0x1e,
	REG_OFSY 			= 0x1f,
	REG_OFSZ 			= 0x20,
	REG_DUR 			= 0x21,
	REG_LATENT 			= 0x22,
	REG_WINDOW 			= 0x23,
	REG_THRESH_ACT 		= 0x24,
	REG_THRESH_INACT 	= 0x25,
	REG_TIME_INACT 		= 0x26,
	REG_ACT_INACT_CTL	= 0x27,
	REG_THRESH_FF		= 0x28,
	REG_TIME_FF 		= 0x29,
	REG_TAP_AXES 		= 0x2a,
	REG_ACT_TAP_STATUS	= 0x2b,
	REG_BW_RATE			= 0x2c,
	REG_POWER_CTL		= 0x2d,
	REG_INT_ENABLE		= 0x2e,
	REG_INT_MAP			= 0x2f,
	REG_INT_SOURCE		= 0x30,
	REG_DATA_FORMAT		= 0x31,
	REG_DATAX0			= 0x32,
	REG_DATAX1			= 0x33,
	REG_DATAY0			= 0x34,
	REG_DATAY1			= 0x35,
	REG_DATAZ0			= 0x36,
	REG_DATAZ1			= 0x37,
	REG_FIFO_CTL		= 0x38,
	REG_FIFO_STATUS		= 0x39,
};

struct gmeter_axdl345_pdata_t {
	struct i2c_device_t * dev;
};

static bool_t axdl345_read(struct i2c_device_t * dev, u8_t reg, u8_t * val)
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

static bool_t axdl345_write(struct i2c_device_t * dev, u8_t reg, u8_t val)
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

static bool_t gmeter_axdl345_get(struct gmeter_t * g, int * x, int * y, int * z)
{
	struct gmeter_axdl345_pdata_t * pdat = (struct gmeter_axdl345_pdata_t *)g->priv;
	u8_t s, l = 0, h = 0;
	s16_t tx, ty, tz;

	if(axdl345_read(pdat->dev, REG_INT_SOURCE, &s) && (s & (1 << 7)))
	{
		axdl345_read(pdat->dev, REG_DATAX0, &l);
		axdl345_read(pdat->dev, REG_DATAX1, &h);
		tx = (h << 8) | (l << 0);

		axdl345_read(pdat->dev, REG_DATAY0, &l);
		axdl345_read(pdat->dev, REG_DATAY1, &h);
		ty = (h << 8) | (l << 0);

		axdl345_read(pdat->dev, REG_DATAZ0, &l);
		axdl345_read(pdat->dev, REG_DATAZ1, &h);
		tz = (h << 8) | (l << 0);

		*x = (s64_t)tx * 39 * 9806650 / 10000;
		*y = (s64_t)ty * 39 * 9806650 / 10000;
		*z = (s64_t)tz * 39 * 9806650 / 10000;
		return TRUE;
	}
	return FALSE;
}

static struct device_t * gmeter_axdl345_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct gmeter_axdl345_pdata_t * pdat;
	struct gmeter_t * g;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	u8_t val;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x53), 0);
	if(!i2cdev)
		return NULL;

	if(axdl345_read(i2cdev, REG_DEVID, &val) && (val == 0xe5))
	{
		axdl345_write(i2cdev, REG_DATA_FORMAT, 0x0b);
		axdl345_write(i2cdev, REG_POWER_CTL, 0x08);
		axdl345_write(i2cdev, REG_INT_ENABLE, 0x80);
	}
	else
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct gmeter_axdl345_pdata_t));
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
	g->get = gmeter_axdl345_get;
	g->priv = pdat;

	if(!register_gmeter(&dev, g))
	{
		i2c_device_free(pdat->dev);

		free_device_name(g->name);
		free(g->priv);
		free(g);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void gmeter_axdl345_remove(struct device_t * dev)
{
	struct gmeter_t * g = (struct gmeter_t *)dev->priv;
	struct gmeter_axdl345_pdata_t * pdat = (struct gmeter_axdl345_pdata_t *)g->priv;

	if(g && unregister_gmeter(g))
	{
		i2c_device_free(pdat->dev);

		free_device_name(g->name);
		free(g->priv);
		free(g);
	}
}

static void gmeter_axdl345_suspend(struct device_t * dev)
{
}

static void gmeter_axdl345_resume(struct device_t * dev)
{
}

static struct driver_t gmeter_axdl345 = {
	.name		= "gmeter-axdl345",
	.probe		= gmeter_axdl345_probe,
	.remove		= gmeter_axdl345_remove,
	.suspend	= gmeter_axdl345_suspend,
	.resume		= gmeter_axdl345_resume,
};

static __init void gmeter_axdl345_driver_init(void)
{
	register_driver(&gmeter_axdl345);
}

static __exit void gmeter_axdl345_driver_exit(void)
{
	unregister_driver(&gmeter_axdl345);
}

driver_initcall(gmeter_axdl345_driver_init);
driver_exitcall(gmeter_axdl345_driver_exit);
