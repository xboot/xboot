/*
 * driver/regulator-rk818.c
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
	RK818_VB_MON			= 0x21,
	RK818_THERMAL			= 0x22,
	RK818_DCDC_EN			= 0x23,
	RK818_LDO_EN			= 0x24,
	RK818_SLEEP_SET_OFF1	= 0x25,
	RK818_SLEEP_SET_OFF2	= 0x26,
	RK818_DCDC_UV_STS		= 0x27,
	RK818_DCDC_UV_ACT		= 0x28,
	RK818_LDO_UV_STS		= 0x29,
	RK818_LDO_UV_ACT		= 0x2a,
	RK818_DCDC_PG			= 0x2b,
	RK818_LDO_PG			= 0x2c,
	RK818_VOUT_MON_TDB		= 0x2d,
	RK818_BUCK1_CONFIG		= 0x2e,
	RK818_BUCK1_ON			= 0x2f,
	RK818_BUCK1_SLP			= 0x30,
	RK818_BUCK1_DVS			= 0x31,
	RK818_BUCK2_CONFIG		= 0x32,
	RK818_BUCK2_ON			= 0x33,
	RK818_BUCK2_SLP			= 0x34,
	RK818_BUCK2_DVS			= 0x35,
	RK818_BUCK3_CONFIG		= 0x36,
	RK818_BUCK4_CONFIG		= 0x37,
	RK818_BUCK4_ON			= 0x38,
	RK818_BUCK4_SLP_VSEL	= 0x39,
	RK818_BOOST_CONFIG		= 0x3a,
	RK818_LDO1_ON_VSEL		= 0x3b,
	RK818_LDO1_SLP_VSEL		= 0x3c,
	RK818_LDO2_ON_VSEL		= 0x3d,
	RK818_LDO2_SLP_VSEL 	= 0x3e,
	RK818_LDO3_ON_VSEL 		= 0x3f,
	RK818_LDO3_SLP_VSEL 	= 0x40,
	RK818_LDO4_ON_VSEL 		= 0x41,
	RK818_LDO4_SLP_VSEL		= 0x42,
	RK818_LDO5_ON_VSEL		= 0x43,
	RK818_LDO5_SLP_VSEL		= 0x44,
	RK818_LDO6_ON_VSEL		= 0x45,
	RK818_LDO6_SLP_VSEL		= 0x46,
	RK818_LDO7_ON_VSEL		= 0x47,
	RK818_LDO7_SLP_VSEL		= 0x48,
	RK818_LDO8_ON_VSEL		= 0x49,
	RK818_LDO8_SLP_VSEL		= 0x4a,
	RK818_DEVCTRL			= 0x4b,
	RK818_INT_STS1			= 0x4c,
	RK818_INT_STS_MSK1		= 0x4d,
	RK818_INT_STS2			= 0x4e,
	RK818_INT_STS_MSK2		= 0x4f,
	RK818_IO_POL			= 0x50,

	RK818_H5V_EN_REG		= 0x52,
	RK818_SLEEP_SET_OFF		= 0x53,
	RK818_LDO9_ON_VSEL		= 0x54,
	RK818_LDO9_SLP_VSEL		= 0x55,
	RK818_CTRL_REG			= 0x56,
	RK818_DCDC_ILMAX		= 0x90,
};

struct regulator_rk818_pdata_t {
	struct i2c_device_t * dev;
	int channel;
};

static bool_t rk818_read(struct i2c_device_t * dev, u8_t reg, u8_t * val)
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

static bool_t rk818_write(struct i2c_device_t * dev, u8_t reg, u8_t val)
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

static void regulator_rk818_set_parent(struct regulator_t * supply, const char * pname)
{
}

static const char * regulator_rk818_get_parent(struct regulator_t * supply)
{
	return NULL;
}

static void regulator_rk818_set_enable(struct regulator_t * supply, bool_t enable)
{
	struct regulator_rk818_pdata_t * pdat = (struct regulator_rk818_pdata_t *)supply->priv;
	u8_t val = 0;

	switch(pdat->channel)
	{
	case 0 ... 3:
		rk818_read(pdat->dev, RK818_DCDC_EN, &val);
		if(enable)
			val |= (0x1 << pdat->channel);
		else
			val &= ~(0x1 << pdat->channel);
		rk818_write(pdat->dev, RK818_DCDC_EN, val);
		break;

	case 4 ... 11:
		rk818_read(pdat->dev, RK818_LDO_EN, &val);
		if(enable)
			val |= (0x1 << (pdat->channel - 4));
		else
			val &= ~(0x1 << (pdat->channel - 4));
		rk818_write(pdat->dev, RK818_LDO_EN, val);
		break;

	case 12 ... 13:
		rk818_read(pdat->dev, RK818_DCDC_EN, &val);
		if(enable)
			val |= (0x1 << (pdat->channel - 12 + 5));
		else
			val &= ~(0x1 << (pdat->channel - 12 + 5));
		rk818_write(pdat->dev, RK818_DCDC_EN, val);
		break;

	default:
		break;
	}
}

static bool_t regulator_rk818_get_enable(struct regulator_t * supply)
{
	struct regulator_rk818_pdata_t * pdat = (struct regulator_rk818_pdata_t *)supply->priv;
	u8_t val = 0;

	switch(pdat->channel)
	{
	case 0 ... 3:
		rk818_read(pdat->dev, RK818_DCDC_EN, &val);
		val = (val >> pdat->channel) & 0x1;
		break;

	case 4 ... 11:
		rk818_read(pdat->dev, RK818_LDO_EN, &val);
		val = (val >> (pdat->channel - 4)) & 0x1;
		break;

	case 12 ... 13:
		rk818_read(pdat->dev, RK818_DCDC_EN, &val);
		val = (val >> (pdat->channel - 12 + 5)) & 0x1;
		break;

	default:
		break;
	}

	return (val != 0) ? TRUE : FALSE;
}

static u8_t rk818_vol_to_reg(int vol, int step, int min, int max)
{
	int reg;

	if(vol < min)
		vol = min;
	else if(vol > max)
		vol = max;

	reg = (vol - min + step - 1) / step;
	return (u8_t)(reg & 0xff);
}

static int rk818_reg_to_vol(u8_t reg, int step, int min, int max)
{
	int vol = (int)reg * step + min;

	if(vol < min)
		vol = min;
	else if(vol > max)
		vol = max;
	return vol;
}

static void regulator_rk818_set_voltage(struct regulator_t * supply, int voltage)
{
	struct regulator_rk818_pdata_t * pdat = (struct regulator_rk818_pdata_t *)supply->priv;
	u8_t val;

	switch(pdat->channel)
	{
	/* DCDC1 - 0.7125V ~ 1.5V, 0.0125V/step, 4.5A */
	case 0:
		val = rk818_vol_to_reg(voltage, 12500, 712500, 1500000);
		rk818_write(pdat->dev, RK818_BUCK1_ON, val);
		break;

	/* DCDC2 - 0.7125V ~ 1.5V, 0.0125V/step, 4.5A */
	case 1:
		val = rk818_vol_to_reg(voltage, 12500, 712500, 1500000);
		rk818_write(pdat->dev, RK818_BUCK2_ON, val);
		break;

	/* DCDC3 - 1.0V ~ 1.8V, 1.5A */
	case 2:
		break;

	/* DCDC4 - 1.8V ~ 3.3V, 0.1V/step, 2A */
	case 3:
		val = rk818_vol_to_reg(voltage, 100000, 1800000, 3300000);
		rk818_write(pdat->dev, RK818_BUCK4_ON, val);
		break;

	/* LDO1 - 1.8V ~ 3.4V, 0.1V/step, 0.15A */
	case 4:
		val = rk818_vol_to_reg(voltage, 100000, 1800000, 3400000);
		rk818_write(pdat->dev, RK818_LDO1_ON_VSEL, val);
		break;

	/* LDO2 - 1.8V ~ 3.4V, 0.1V/step, 0.15A */
	case 5:
		val = rk818_vol_to_reg(voltage, 100000, 1800000, 3400000);
		rk818_write(pdat->dev, RK818_LDO2_ON_VSEL, val);
		break;

	/* LDO3 - 0.8V ~ 2.5V, 0.1V/step, 0.1A */
	case 6:
		val = rk818_vol_to_reg(voltage, 100000, 800000, 2500000);
		rk818_write(pdat->dev, RK818_LDO3_ON_VSEL, val);
		break;

	/* LDO4 - 1.8V ~ 3.4V, 0.1V/step, 0.1A */
	case 7:
		val = rk818_vol_to_reg(voltage, 100000, 1800000, 3400000);
		rk818_write(pdat->dev, RK818_LDO4_ON_VSEL, val);
		break;

	/* LDO5 - 1.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 8:
		val = rk818_vol_to_reg(voltage, 100000, 1800000, 3400000);
		rk818_write(pdat->dev, RK818_LDO5_ON_VSEL, val);
		break;

	/* LDO6 - 0.8V ~ 2.5V, 0.1V/step, 0.15A */
	case 9:
		val = rk818_vol_to_reg(voltage, 100000, 800000, 2500000);
		rk818_write(pdat->dev, RK818_LDO6_ON_VSEL, val);
		break;

	/* LDO7 - 0.8V ~ 2.5V, 0.1V/step, 0.3A */
	case 10:
		val = rk818_vol_to_reg(voltage, 100000, 800000, 2500000);
		rk818_write(pdat->dev, RK818_LDO7_ON_VSEL, val);
		break;

	/* LDO8 - 1.8V ~ 3.4V, 0.1V/step, 0.4A */
	case 11:
		val = rk818_vol_to_reg(voltage, 100000, 1800000, 3400000);
		rk818_write(pdat->dev, RK818_LDO8_ON_VSEL, val);
		break;

	/* LDO9 - 1.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 12:
		val = rk818_vol_to_reg(voltage, 100000, 1800000, 3400000);
		rk818_write(pdat->dev, RK818_LDO9_ON_VSEL, val);
		break;

	/* SWOUT - 0.2R */
	case 13:
		break;

	default:
		break;
	}
}

static int regulator_rk818_get_voltage(struct regulator_t * supply)
{
	struct regulator_rk818_pdata_t * pdat = (struct regulator_rk818_pdata_t *)supply->priv;
	int voltage;
	u8_t val = 0;

	switch(pdat->channel)
	{
	/* DCDC1 - 0.7125V ~ 1.5V, 0.0125V/step, 5A */
	case 0:
		rk818_read(pdat->dev, RK818_BUCK1_ON, &val);
		voltage = rk818_reg_to_vol(val, 12500, 712500, 1500000);
		break;

	/* DCDC2 - 0.7125V ~ 1.5V, 0.0125V/step, 5A */
	case 1:
		rk818_read(pdat->dev, RK818_BUCK2_ON, &val);
		voltage = rk818_reg_to_vol(val, 12500, 712500, 1500000);
		break;

	/* DCDC3 - 1.0V ~ 1.8V, 1.5A */
	case 2:
		voltage = 1250000;
		break;

	/* DCDC4 - 1.8V ~ 3.3V, 0.1V/step, 1.5A */
	case 3:
		rk818_read(pdat->dev, RK818_BUCK4_ON, &val);
		voltage = rk818_reg_to_vol(val, 100000, 1800000, 3300000);
		break;

	/* LDO1 - 1.8V ~ 3.4V, 0.1V/step, 0.15A */
	case 4:
		rk818_read(pdat->dev, RK818_LDO1_ON_VSEL, &val);
		voltage = rk818_reg_to_vol(val, 100000, 1800000, 3400000);
		break;

	/* LDO2 - 1.8V ~ 3.4V, 0.1V/step, 0.15A */
	case 5:
		rk818_read(pdat->dev, RK818_LDO2_ON_VSEL, &val);
		voltage = rk818_reg_to_vol(val, 100000, 1800000, 3400000);
		break;

	/* LDO3 - 0.8V ~ 2.5V, 0.1V/step, 0.1A */
	case 6:
		rk818_read(pdat->dev, RK818_LDO3_ON_VSEL, &val);
		voltage = rk818_reg_to_vol(val, 100000, 800000, 2500000);
		break;

	/* LDO4 - 1.8V ~ 3.4V, 0.1V/step, 0.1A */
	case 7:
		rk818_read(pdat->dev, RK818_LDO4_ON_VSEL, &val);
		voltage = rk818_reg_to_vol(val, 100000, 1800000, 3400000);
		break;

	/* LDO5 - 1.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 8:
		rk818_read(pdat->dev, RK818_LDO5_ON_VSEL, &val);
		voltage = rk818_reg_to_vol(val, 100000, 1800000, 3400000);
		break;

	/* LDO6 - 0.8V ~ 2.5V, 0.1V/step, 0.15A */
	case 9:
		rk818_read(pdat->dev, RK818_LDO6_ON_VSEL, &val);
		voltage = rk818_reg_to_vol(val, 100000, 800000, 2500000);
		break;

	/* LDO7 - 0.8V ~ 2.5V, 0.1V/step, 0.3A */
	case 10:
		rk818_read(pdat->dev, RK818_LDO7_ON_VSEL, &val);
		voltage = rk818_reg_to_vol(val, 100000, 800000, 2500000);
		break;

	/* LDO8 - 1.8V ~ 3.4V, 0.1V/step, 0.4A */
	case 11:
		rk818_read(pdat->dev, RK818_LDO8_ON_VSEL, &val);
		voltage = rk818_reg_to_vol(val, 100000, 1800000, 3400000);
		break;

	/* LDO9 - 1.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 12:
		rk818_read(pdat->dev, RK818_LDO9_ON_VSEL, &val);
		voltage = rk818_reg_to_vol(val, 100000, 1800000, 3400000);
		break;

	/* SWOUT - 0.2R */
	case 13:
		voltage = 3300000;
		break;

	default:
		voltage = 0;
		break;
	}

	return voltage;
}

static struct device_t * regulator_rk818_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct regulator_rk818_pdata_t * pdat;
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

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x1c), 0);
	if(!i2cdev)
		return NULL;

	if(!rk818_read(i2cdev, RK818_DEVCTRL, &val))
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct regulator_rk818_pdata_t));
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
	supply->set_parent = regulator_rk818_set_parent;
	supply->get_parent = regulator_rk818_get_parent;
	supply->set_enable = regulator_rk818_set_enable;
	supply->get_enable = regulator_rk818_get_enable;
	supply->set_voltage = regulator_rk818_set_voltage;
	supply->get_voltage = regulator_rk818_get_voltage;
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

static void regulator_rk818_remove(struct device_t * dev)
{
	struct regulator_t * supply = (struct regulator_t *)dev->priv;
	struct regulator_rk818_pdata_t * pdat = (struct regulator_rk818_pdata_t *)supply->priv;

	if(supply && unregister_regulator(supply))
	{
		i2c_device_free(pdat->dev);

		free(supply->name);
		free(supply->priv);
		free(supply);
	}
}

static void regulator_rk818_suspend(struct device_t * dev)
{
}

static void regulator_rk818_resume(struct device_t * dev)
{
}

static struct driver_t regulator_rk818 = {
	.name		= "regulator-rk818",
	.probe		= regulator_rk818_probe,
	.remove		= regulator_rk818_remove,
	.suspend	= regulator_rk818_suspend,
	.resume		= regulator_rk818_resume,
};

static __init void regulator_rk818_driver_init(void)
{
	register_driver(&regulator_rk818);
}

static __exit void regulator_rk818_driver_exit(void)
{
	unregister_driver(&regulator_rk818);
}

driver_initcall(regulator_rk818_driver_init);
driver_exitcall(regulator_rk818_driver_exit);
