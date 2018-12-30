/*
 * driver/gmeter-mma8452.c
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
	REG_STATUS			= 0x00,
	REG_OUT_X_MSB		= 0x01,
	REG_OUT_X_LSB		= 0x02,
	REG_OUT_Y_MSB		= 0x03,
	REG_OUT_Y_LSB		= 0x04,
	REG_OUT_Z_MSB		= 0x05,
	REG_OUT_Z_LSB		= 0x06,
	REG_SYSMOD			= 0x0b,
	REG_INTSRC			= 0x0c,
	REG_WHOAMI			= 0x0d,
	REG_XYZ_DATA_CFG	= 0x0e,
	REG_HPFILTER_CUTOFF	= 0x0f,
	REG_PL_STATUS 		= 0x10,
	REG_PL_CFG			= 0x11,
	REG_PL_COUNT		= 0x12,
	REG_PL_BF_ZCOMP		= 0x13,
	REG_PL_THS_REG		= 0x14,
	REG_FF_MT_CFG		= 0x15,
	REG_FF_MT_SRC		= 0x16,
	REG_FF_MT_THS		= 0x17,
	REG_FF_MT_CNT		= 0x18,
	REG_TRANSIENT_CFG	= 0x1d,
	REG_TRANSIENT_SRC	= 0x1e,
	REG_TRANSIENT_THS	= 0x1f,
	REG_TRANSIENT_CNT	= 0x20,
	REG_PULSE_CFG		= 0x21,
	REG_PULSE_SRC		= 0x22,
	REG_PULSE_THSX		= 0x23,
	REG_PULSE_THSY		= 0x24,
	REG_PULSE_THSZ		= 0x25,
	REG_PULSE_TMLT		= 0x26,
	REG_PULSE_LTCY		= 0x27,
	REG_PULSE_WIND		= 0x28,
	REG_ASLP_COUNT		= 0x29,
	REG_CTRL1			= 0x2a,
	REG_CTRL2			= 0x2b,
	REG_CTRL3			= 0x2c,
	REG_CTRL4			= 0x2e,
	REG_CTRL5			= 0x2e,
	REG_OFF_X			= 0x2f,
	REG_OFF_Y			= 0x30,
	REG_OFF_Z			= 0x31,
};

struct gmeter_mma8452_pdata_t {
	struct i2c_device_t * dev;
};

static bool_t mma8452_read(struct i2c_device_t * dev, u8_t reg, u8_t * val)
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

static bool_t mma8452_write(struct i2c_device_t * dev, u8_t reg, u8_t val)
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

static bool_t gmeter_mma8452_get(struct gmeter_t * g, int * x, int * y, int * z)
{
	struct gmeter_mma8452_pdata_t * pdat = (struct gmeter_mma8452_pdata_t *)g->priv;
	u8_t s, l = 0, h = 0;
	s16_t tx, ty, tz;

	if(mma8452_read(pdat->dev, REG_STATUS, &s) && (s & (1 << 3)))
	{
		mma8452_read(pdat->dev, REG_OUT_X_MSB, &h);
		mma8452_read(pdat->dev, REG_OUT_X_LSB, &l);
		tx = (h << 8) | (l << 0);

		mma8452_read(pdat->dev, REG_OUT_Y_MSB, &h);
		mma8452_read(pdat->dev, REG_OUT_Y_LSB, &l);
		ty = (h << 8) | (l << 0);

		mma8452_read(pdat->dev, REG_OUT_Z_MSB, &h);
		mma8452_read(pdat->dev, REG_OUT_Z_LSB, &l);
		tz = (h << 8) | (l << 0);

		*x = (s64_t)tx * 2 * 9806650 / 32768;
		*y = (s64_t)ty * 2 * 9806650 / 32768;
		*z = (s64_t)tz * 2 * 9806650 / 32768;
		return TRUE;
	}
	return FALSE;
}

static struct device_t * gmeter_mma8452_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct gmeter_mma8452_pdata_t * pdat;
	struct gmeter_t * g;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	u8_t val;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x1d), 0);
	if(!i2cdev)
		return NULL;

	if(mma8452_read(i2cdev, REG_WHOAMI, &val) && (val == 0x2a))
	{
		/* Standby */
		mma8452_read(i2cdev, REG_CTRL1, &val);
		val &= ~(1 << 0);
		mma8452_write(i2cdev, REG_CTRL1, val);

		/* Full scale range, 2G */
		mma8452_read(i2cdev, REG_XYZ_DATA_CFG, &val);
		val &= ~(3 << 0);
		mma8452_write(i2cdev, REG_XYZ_DATA_CFG, val);

		/* Data rate, 100HZ */
		mma8452_read(i2cdev, REG_CTRL1, &val);
		val &= ~(7 << 3);
		val |= 3 << 3;
		mma8452_write(i2cdev, REG_CTRL1, val);

		/* Active */
		mma8452_read(i2cdev, REG_CTRL1, &val);
		val |= 1 << 0;
		mma8452_write(i2cdev, REG_CTRL1, val);
	}
	else
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct gmeter_mma8452_pdata_t));
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
	g->get = gmeter_mma8452_get;
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

static void gmeter_mma8452_remove(struct device_t * dev)
{
	struct gmeter_t * g = (struct gmeter_t *)dev->priv;
	struct gmeter_mma8452_pdata_t * pdat = (struct gmeter_mma8452_pdata_t *)g->priv;

	if(g && unregister_gmeter(g))
	{
		i2c_device_free(pdat->dev);

		free_device_name(g->name);
		free(g->priv);
		free(g);
	}
}

static void gmeter_mma8452_suspend(struct device_t * dev)
{
}

static void gmeter_mma8452_resume(struct device_t * dev)
{
}

static struct driver_t gmeter_mma8452 = {
	.name		= "gmeter-mma8452",
	.probe		= gmeter_mma8452_probe,
	.remove		= gmeter_mma8452_remove,
	.suspend	= gmeter_mma8452_suspend,
	.resume		= gmeter_mma8452_resume,
};

static __init void gmeter_mma8452_driver_init(void)
{
	register_driver(&gmeter_mma8452);
}

static __exit void gmeter_mma8452_driver_exit(void)
{
	unregister_driver(&gmeter_mma8452);
}

driver_initcall(gmeter_mma8452_driver_init);
driver_exitcall(gmeter_mma8452_driver_exit);
