/*
 * driver/regulator-rk808.c
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
#include <regulator/regulator.h>

enum {
	RK808_VB_MON			= 0x21,
	RK808_THERMAL			= 0x22,
	RK808_DCDC_EN			= 0x23,
	RK808_LDO_EN			= 0x24,
	RK808_SLEEP_SET_OFF1	= 0x25,
	RK808_SLEEP_SET_OFF2	= 0x26,
	RK808_DCDC_UV_STS		= 0x27,
	RK808_DCDC_UV_ACT		= 0x28,
	RK808_LDO_UV_STS		= 0x29,
	RK808_LDO_UV_ACT		= 0x2a,
	RK808_DCDC_PG			= 0x2b,
	RK808_LDO_PG			= 0x2c,
	RK808_VOUT_MON_TDB		= 0x2d,
	RK808_BUCK1_CONFIG		= 0x2e,
	RK808_BUCK1_ON			= 0x2f,
	RK808_BUCK1_SLP			= 0x30,
	RK808_BUCK1_DVS			= 0x31,
	RK808_BUCK2_CONFIG		= 0x32,
	RK808_BUCK2_ON			= 0x33,
	RK808_BUCK2_SLP			= 0x34,
	RK808_BUCK2_DVS			= 0x35,
	RK808_BUCK3_CONFIG		= 0x36,
	RK808_BUCK4_CONFIG		= 0x37,
	RK808_BUCK4_ON			= 0x38,
	RK808_BUCK4_SLP_VSEL	= 0x39,
	RK808_BOOST_CONFIG		= 0x3a,
	RK808_LDO1_ON_VSEL		= 0x3b,
	RK808_LDO1_SLP_VSEL		= 0x3c,
	RK808_LDO2_ON_VSEL		= 0x3d,
	RK808_LDO2_SLP_VSEL 	= 0x3e,
	RK808_LDO3_ON_VSEL 		= 0x3f,
	RK808_LDO3_SLP_VSEL 	= 0x40,
	RK808_LDO4_ON_VSEL 		= 0x41,
	RK808_LDO4_SLP_VSEL		= 0x42,
	RK808_LDO5_ON_VSEL		= 0x43,
	RK808_LDO5_SLP_VSEL		= 0x44,
	RK808_LDO6_ON_VSEL		= 0x45,
	RK808_LDO6_SLP_VSEL		= 0x46,
	RK808_LDO7_ON_VSEL		= 0x47,
	RK808_LDO7_SLP_VSEL		= 0x48,
	RK808_LDO8_ON_VSEL		= 0x49,
	RK808_LDO8_SLP_VSEL		= 0x4a,
	RK808_DEVCTRL			= 0x4b,
	RK808_INT_STS1			= 0x4c,
	RK808_INT_STS_MSK1		= 0x4d,
	RK808_INT_STS2			= 0x4e,
	RK808_INT_STS_MSK2		= 0x4f,
	RK808_IO_POL			= 0x50,
};

struct regulator_rk808_pdata_t {
	struct i2c_device_t * dev;
	int channel;
};

static bool_t rk808_read(struct i2c_device_t * dev, u8_t reg, u8_t * val)
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

static bool_t rk808_write(struct i2c_device_t * dev, u8_t reg, u8_t val)
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

static void regulator_rk808_set_parent(struct regulator_t * supply, const char * pname)
{
}

static const char * regulator_rk808_get_parent(struct regulator_t * supply)
{
	return NULL;
}

static void regulator_rk808_set_enable(struct regulator_t * supply, bool_t enable)
{
	struct regulator_rk808_pdata_t * pdat = (struct regulator_rk808_pdata_t *)supply->priv;
	u8_t val = 0;

	switch(pdat->channel)
	{
	case 0 ... 3:
		rk808_read(pdat->dev, RK808_DCDC_EN, &val);
		if(enable)
			val |= (0x1 << pdat->channel);
		else
			val &= ~(0x1 << pdat->channel);
		rk808_write(pdat->dev, RK808_DCDC_EN, val);
		break;

	case 4 ... 11:
		rk808_read(pdat->dev, RK808_LDO_EN, &val);
		if(enable)
			val |= (0x1 << (pdat->channel - 4));
		else
			val &= ~(0x1 << (pdat->channel - 4));
		rk808_write(pdat->dev, RK808_LDO_EN, val);
		break;

	case 12 ... 13:
		rk808_read(pdat->dev, RK808_DCDC_EN, &val);
		if(enable)
			val |= (0x1 << (pdat->channel - 12 + 5));
		else
			val &= ~(0x1 << (pdat->channel - 12 + 5));
		rk808_write(pdat->dev, RK808_DCDC_EN, val);
		break;

	default:
		break;
	}
}

static bool_t regulator_rk808_get_enable(struct regulator_t * supply)
{
	struct regulator_rk808_pdata_t * pdat = (struct regulator_rk808_pdata_t *)supply->priv;
	u8_t val = 0;

	switch(pdat->channel)
	{
	case 0 ... 3:
		rk808_read(pdat->dev, RK808_DCDC_EN, &val);
		val = (val >> pdat->channel) & 0x1;
		break;

	case 4 ... 11:
		rk808_read(pdat->dev, RK808_LDO_EN, &val);
		val = (val >> (pdat->channel - 4)) & 0x1;
		break;

	case 12 ... 13:
		rk808_read(pdat->dev, RK808_DCDC_EN, &val);
		val = (val >> (pdat->channel - 12 + 5)) & 0x1;
		break;

	default:
		break;
	}

	return (val != 0) ? TRUE : FALSE;
}

static u8_t rk808_vol_to_reg(int vol, int step, int min, int max)
{
	int reg;

	if(vol < min)
		vol = min;
	else if(vol > max)
		vol = max;

	reg = (vol - min + step - 1) / step;
	return (u8_t)(reg & 0xff);
}

static int rk808_reg_to_vol(u8_t reg, int step, int min, int max)
{
	int vol = (int)reg * step + min;

	if(vol < min)
		vol = min;
	else if(vol > max)
		vol = max;
	return vol;
}

static void regulator_rk808_set_voltage(struct regulator_t * supply, int voltage)
{
	struct regulator_rk808_pdata_t * pdat = (struct regulator_rk808_pdata_t *)supply->priv;
	u8_t val;

	switch(pdat->channel)
	{
	/* DCDC1 - 0.7V ~ 1.5V, 0.0125V/step, 5A */
	case 0:
		val = rk808_vol_to_reg(voltage, 12500, 700000, 1500000);
		rk808_write(pdat->dev, RK808_BUCK1_ON, val);
		break;

	/* DCDC2 - 0.7V ~ 1.5V, 0.0125V/step, 5A */
	case 1:
		val = rk808_vol_to_reg(voltage, 12500, 700000, 1500000);
		rk808_write(pdat->dev, RK808_BUCK2_ON, val);
		break;

	/* DCDC3 - 1.0V ~ 1.8V, 1.5A */
	case 2:
		break;

	/* DCDC4 - 1.8V ~ 3.3V, 0.1V/step, 1.5A */
	case 3:
		val = rk808_vol_to_reg(voltage, 100000, 1800000, 3300000);
		rk808_write(pdat->dev, RK808_BUCK4_ON, val);
		break;

	/* LDO1 - 1.8V ~ 3.4V, 0.1V/step, 0.15A */
	case 4:
		val = rk808_vol_to_reg(voltage, 100000, 1800000, 3400000);
		rk808_write(pdat->dev, RK808_LDO1_ON_VSEL, val);
		break;

	/* LDO2 - 1.8V ~ 3.4V, 0.1V/step, 0.15A */
	case 5:
		val = rk808_vol_to_reg(voltage, 100000, 1800000, 3400000);
		rk808_write(pdat->dev, RK808_LDO2_ON_VSEL, val);
		break;

	/* LDO3 - 0.8V ~ 2.5V, 0.1V/step, 0.1A */
	case 6:
		val = rk808_vol_to_reg(voltage, 100000, 800000, 2500000);
		rk808_write(pdat->dev, RK808_LDO3_ON_VSEL, val);
		break;

	/* LDO4 - 1.8V ~ 3.4V, 0.1V/step, 0.15A */
	case 7:
		val = rk808_vol_to_reg(voltage, 100000, 1800000, 3400000);
		rk808_write(pdat->dev, RK808_LDO4_ON_VSEL, val);
		break;

	/* LDO5 - 1.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 8:
		val = rk808_vol_to_reg(voltage, 100000, 1800000, 3400000);
		rk808_write(pdat->dev, RK808_LDO5_ON_VSEL, val);
		break;

	/* LDO6 - 0.8V ~ 2.5V, 0.1V/step, 0.15A */
	case 9:
		val = rk808_vol_to_reg(voltage, 100000, 800000, 2500000);
		rk808_write(pdat->dev, RK808_LDO6_ON_VSEL, val);
		break;

	/* LDO7 - 0.8V ~ 2.5V, 0.1V/step, 0.3A */
	case 10:
		val = rk808_vol_to_reg(voltage, 100000, 800000, 2500000);
		rk808_write(pdat->dev, RK808_LDO7_ON_VSEL, val);
		break;

	/* LDO8 - 1.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 11:
		val = rk808_vol_to_reg(voltage, 100000, 1800000, 3400000);
		rk808_write(pdat->dev, RK808_LDO8_ON_VSEL, val);
		break;

	/* SWOUT1 - 0.2R */
	case 12:
		break;

	/* SWOUT2 - 0.2R */
	case 13:
		break;

	default:
		break;
	}
}

static int regulator_rk808_get_voltage(struct regulator_t * supply)
{
	struct regulator_rk808_pdata_t * pdat = (struct regulator_rk808_pdata_t *)supply->priv;
	int voltage;
	u8_t val = 0;

	switch(pdat->channel)
	{
	/* DCDC1 - 0.7V ~ 1.5V, 0.0125V/step, 5A */
	case 0:
		rk808_read(pdat->dev, RK808_BUCK1_ON, &val);
		voltage = rk808_reg_to_vol(val, 12500, 700000, 1500000);
		break;

	/* DCDC2 - 0.7V ~ 1.5V, 0.0125V/step, 5A */
	case 1:
		rk808_read(pdat->dev, RK808_BUCK2_ON, &val);
		voltage = rk808_reg_to_vol(val, 12500, 700000, 1500000);
		break;

	/* DCDC3 - 1.0V ~ 1.8V, 1.5A */
	case 2:
		voltage = 1250000;
		break;

	/* DCDC4 - 1.8V ~ 3.3V, 0.1V/step, 1.5A */
	case 3:
		rk808_read(pdat->dev, RK808_BUCK4_ON, &val);
		voltage = rk808_reg_to_vol(val, 100000, 1800000, 3300000);
		break;

	/* LDO1 - 1.8V ~ 3.4V, 0.1V/step, 0.15A */
	case 4:
		rk808_read(pdat->dev, RK808_LDO1_ON_VSEL, &val);
		voltage = rk808_reg_to_vol(val, 100000, 1800000, 3400000);
		break;

	/* LDO2 - 1.8V ~ 3.4V, 0.1V/step, 0.15A */
	case 5:
		rk808_read(pdat->dev, RK808_LDO2_ON_VSEL, &val);
		voltage = rk808_reg_to_vol(val, 100000, 1800000, 3400000);
		break;

	/* LDO3 - 0.8V ~ 2.5V, 0.1V/step, 0.1A */
	case 6:
		rk808_read(pdat->dev, RK808_LDO3_ON_VSEL, &val);
		voltage = rk808_reg_to_vol(val, 100000, 800000, 2500000);
		break;

	/* LDO4 - 1.8V ~ 3.4V, 0.1V/step, 0.15A */
	case 7:
		rk808_read(pdat->dev, RK808_LDO4_ON_VSEL, &val);
		voltage = rk808_reg_to_vol(val, 100000, 1800000, 3400000);
		break;

	/* LDO5 - 1.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 8:
		rk808_read(pdat->dev, RK808_LDO5_ON_VSEL, &val);
		voltage = rk808_reg_to_vol(val, 100000, 1800000, 3400000);
		break;

	/* LDO6 - 0.8V ~ 2.5V, 0.1V/step, 0.15A */
	case 9:
		rk808_read(pdat->dev, RK808_LDO6_ON_VSEL, &val);
		voltage = rk808_reg_to_vol(val, 100000, 800000, 2500000);
		break;

	/* LDO7 - 0.8V ~ 2.5V, 0.1V/step, 0.3A */
	case 10:
		rk808_read(pdat->dev, RK808_LDO7_ON_VSEL, &val);
		voltage = rk808_reg_to_vol(val, 100000, 800000, 2500000);
		break;

	/* LDO8 - 1.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 11:
		rk808_read(pdat->dev, RK808_LDO8_ON_VSEL, &val);
		voltage = rk808_reg_to_vol(val, 100000, 1800000, 3400000);
		break;

	/* SWOUT1 - 0.2R */
	case 12:
		voltage = 3300000;
		break;

	/* SWOUT2 - 0.2R */
	case 13:
		voltage = 3000000;
		break;

	default:
		voltage = 0;
		break;
	}

	return voltage;
}

static struct device_t * regulator_rk808_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct regulator_rk808_pdata_t * pdat;
	struct regulator_t * supply;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	struct dtnode_t o;
	char * name = dt_read_string(n, "name", NULL);
	int channel = dt_read_int(n, "channel", -1);
	u8_t val;

	if(!name || search_regulator(name))
		return NULL;

	if(channel < 0 || channel > 13)
		return NULL;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x1b), 0);
	if(!i2cdev)
		return NULL;

	if(!rk808_read(i2cdev, RK808_DEVCTRL, &val))
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct regulator_rk808_pdata_t));
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
	pdat->channel = channel;

	supply->name = strdup(name);
	supply->count = 0;
	supply->set_parent = regulator_rk808_set_parent;
	supply->get_parent = regulator_rk808_get_parent;
	supply->set_enable = regulator_rk808_set_enable;
	supply->get_enable = regulator_rk808_get_enable;
	supply->set_voltage = regulator_rk808_set_voltage;
	supply->get_voltage = regulator_rk808_get_voltage;
	supply->priv = pdat;

	if(!register_regulator(&dev, supply))
	{
		i2c_device_free(pdat->dev);

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

static void regulator_rk808_remove(struct device_t * dev)
{
	struct regulator_t * supply = (struct regulator_t *)dev->priv;
	struct regulator_rk808_pdata_t * pdat = (struct regulator_rk808_pdata_t *)supply->priv;

	if(supply && unregister_regulator(supply))
	{
		i2c_device_free(pdat->dev);

		free(supply->name);
		free(supply->priv);
		free(supply);
	}
}

static void regulator_rk808_suspend(struct device_t * dev)
{
}

static void regulator_rk808_resume(struct device_t * dev)
{
}

static struct driver_t regulator_rk808 = {
	.name		= "regulator-rk808",
	.probe		= regulator_rk808_probe,
	.remove		= regulator_rk808_remove,
	.suspend	= regulator_rk808_suspend,
	.resume		= regulator_rk808_resume,
};

static __init void regulator_rk808_driver_init(void)
{
	register_driver(&regulator_rk808);
}

static __exit void regulator_rk808_driver_exit(void)
{
	unregister_driver(&regulator_rk808);
}

driver_initcall(regulator_rk808_driver_init);
driver_exitcall(regulator_rk808_driver_exit);
