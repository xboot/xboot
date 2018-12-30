/*
 * driver/regulator-syr82x.c
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
#include <regulator/regulator.h>

enum {
	SYR82X_VSEL0	= 0x00,
	SYR82X_VSEL1	= 0x01,
	SYR82X_CTRL		= 0x02,
	SYR82X_ID1		= 0x03,
	SYR82X_ID2		= 0x04,
	SYR82X_PGOOD	= 0x05,
};

struct regulator_syr82x_pdata_t {
	struct i2c_device_t * dev;
	char * parent;
};

static bool_t syr82x_read(struct i2c_device_t * dev, u8_t reg, u8_t * val)
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

static bool_t syr82x_write(struct i2c_device_t * dev, u8_t reg, u8_t val)
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

static void regulator_syr82x_set_parent(struct regulator_t * supply, const char * pname)
{
}

static const char * regulator_syr82x_get_parent(struct regulator_t * supply)
{
	struct regulator_syr82x_pdata_t * pdat = (struct regulator_syr82x_pdata_t *)supply->priv;
	return pdat->parent;
}

static void regulator_syr82x_set_enable(struct regulator_t * supply, bool_t enable)
{
	struct regulator_syr82x_pdata_t * pdat = (struct regulator_syr82x_pdata_t *)supply->priv;
	u8_t val = 0;

	syr82x_read(pdat->dev, SYR82X_VSEL0, &val);
	if(enable)
		val |= (0x1 << 7);
	else
		val &= ~(0x1 << 7);
	syr82x_write(pdat->dev, SYR82X_VSEL0, val);

	syr82x_read(pdat->dev, SYR82X_VSEL1, &val);
	if(enable)
		val |= (0x1 << 7);
	else
		val &= ~(0x1 << 7);
	syr82x_write(pdat->dev, SYR82X_VSEL1, val);
}

static bool_t regulator_syr82x_get_enable(struct regulator_t * supply)
{
	struct regulator_syr82x_pdata_t * pdat = (struct regulator_syr82x_pdata_t *)supply->priv;
	u8_t val = 0;

	syr82x_read(pdat->dev, SYR82X_VSEL0, &val);
	return (val & 0x80) ? TRUE : FALSE;
}

static u8_t syr82x_vol_to_reg(int vol, int step, int min, int max)
{
	int reg;

	if(vol < min)
		vol = min;
	else if(vol > max)
		vol = max;

	reg = (vol - min + step - 1) / step;
	return (u8_t)(reg & 0xff);
}

static int syr82x_reg_to_vol(u8_t reg, int step, int min, int max)
{
	int vol = (int)reg * step + min;

	if(vol < min)
		vol = min;
	else if(vol > max)
		vol = max;
	return vol;
}

static void regulator_syr82x_set_voltage(struct regulator_t * supply, int voltage)
{
	struct regulator_syr82x_pdata_t * pdat = (struct regulator_syr82x_pdata_t *)supply->priv;
	u8_t val = 0;

	/*
	 * DCDC - 0.7125V ~ 1.5000V, 0.0125V/step, 6A
	 */
	syr82x_read(pdat->dev, SYR82X_VSEL0, &val);
	val &= ~(0x3f << 0);
	val |= ((syr82x_vol_to_reg(voltage, 12500, 712500, 1500000) & 0x3f) << 0);
	syr82x_write(pdat->dev, SYR82X_VSEL0, val);

	syr82x_read(pdat->dev, SYR82X_VSEL1, &val);
	val &= ~(0x3f << 0);
	val |= ((syr82x_vol_to_reg(voltage, 12500, 712500, 1500000) & 0x3f) << 0);
	syr82x_write(pdat->dev, SYR82X_VSEL1, val);
}

static int regulator_syr82x_get_voltage(struct regulator_t * supply)
{
	struct regulator_syr82x_pdata_t * pdat = (struct regulator_syr82x_pdata_t *)supply->priv;
	u8_t val = 0;

	/*
	 * DCDC - 0.7125V ~ 1.5000V, 0.0125V/step, 6A
	 */
	syr82x_read(pdat->dev, SYR82X_VSEL0, &val);
	return syr82x_reg_to_vol(val & 0x3f, 12500, 712500, 1500000);
}

static struct device_t * regulator_syr82x_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct regulator_syr82x_pdata_t * pdat;
	struct regulator_t * supply;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	struct dtnode_t o;
	char * parent = dt_read_string(n, "parent", NULL);
	char * name = dt_read_string(n, "name", NULL);
	u8_t val;

	if(!name || search_regulator(name))
		return NULL;

	if(parent && !search_regulator(parent))
		parent = NULL;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x40), 0);
	if(!i2cdev)
		return NULL;

	if(!syr82x_read(i2cdev, SYR82X_ID1, &val) || (((val >> 5) & 0x7) != 0x4))
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct regulator_syr82x_pdata_t));
	if(!pdat)
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	supply = malloc(sizeof(struct regulator_t));
	if(!supply)
	{
		i2c_device_free(i2cdev);
		free(pdat);
		return NULL;
	}

	pdat->dev = i2cdev;
	pdat->parent = strdup(parent);

	supply->name = strdup(name);
	supply->count = 0;
	supply->set_parent = regulator_syr82x_set_parent;
	supply->get_parent = regulator_syr82x_get_parent;
	supply->set_enable = regulator_syr82x_set_enable;
	supply->get_enable = regulator_syr82x_get_enable;
	supply->set_voltage = regulator_syr82x_set_voltage;
	supply->get_voltage = regulator_syr82x_get_voltage;
	supply->priv = pdat;

	syr82x_read(pdat->dev, SYR82X_CTRL, &val);
	val &= ~(0x7 << 4);
	val |= (0x4 << 4);
	syr82x_write(pdat->dev, SYR82X_CTRL, val);

	if(!register_regulator(&dev, supply))
	{
		i2c_device_free(pdat->dev);
		if(pdat->parent)
			free(pdat->parent);

		free(supply->name);
		free(supply->priv);
		free(supply);
		return NULL;
	}
	dev->driver = drv;

	if(dt_read_object(n, "default", &o))
	{
		char * s = supply->name;
		char * p;
		int v;
		int e;

		if((p = dt_read_string(&o, "parent", NULL)) && search_regulator(p))
			regulator_set_parent(s, p);
		if((v = dt_read_int(&o, "voltage", -1)) >= 0)
			regulator_set_voltage(s, v);
		if((e = dt_read_bool(&o, "enable", -1)) != -1)
		{
			if(e > 0)
				regulator_enable(s);
			else
				regulator_disable(s);
		}
	}
	return dev;
}

static void regulator_syr82x_remove(struct device_t * dev)
{
	struct regulator_t * supply = (struct regulator_t *)dev->priv;
	struct regulator_syr82x_pdata_t * pdat = (struct regulator_syr82x_pdata_t *)supply->priv;

	if(supply && unregister_regulator(supply))
	{
		i2c_device_free(pdat->dev);
		if(pdat->parent)
			free(pdat->parent);

		free(supply->name);
		free(supply->priv);
		free(supply);
	}
}

static void regulator_syr82x_suspend(struct device_t * dev)
{
}

static void regulator_syr82x_resume(struct device_t * dev)
{
}

static struct driver_t regulator_syr82x = {
	.name		= "regulator-syr82x",
	.probe		= regulator_syr82x_probe,
	.remove		= regulator_syr82x_remove,
	.suspend	= regulator_syr82x_suspend,
	.resume		= regulator_syr82x_resume,
};

static __init void regulator_syr82x_driver_init(void)
{
	register_driver(&regulator_syr82x);
}

static __exit void regulator_syr82x_driver_exit(void)
{
	unregister_driver(&regulator_syr82x);
}

driver_initcall(regulator_syr82x_driver_init);
driver_exitcall(regulator_syr82x_driver_exit);
