/*
 * driver/regulator-rk816.c
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
#include <regulator/regulator.h>

enum {
	RK816_CHIP_NAME 		= 0x17,
	RK816_CHIP_VER 			= 0x18,
	RK816_OTP_VER 			= 0x19,

	RK818_VB_MON			= 0x21,
	RK818_THERMAL			= 0x22,
	RK816_PWRON_LP_INT_TIME = 0x47,
	RK816_PWRON_DB 			= 0x48,
	RK816_DEV_CTRL 			= 0x4b,
	RK816_ON_SOURCE 		= 0xae,
	RK816_OFF_SOURCE 		= 0xaf,

	RK816_DCDC_EN1 			= 0x23,
	RK816_DCDC_EN2			= 0x24,
	RK816_SLP_DCDC_EN 		= 0x25,
	RK816_SLP_LDO_EN 		= 0x26,
	RK816_LDO_EN1 			= 0x27,
	RK816_LDO_EN2 			= 0x28,
	RK816_SW_BUCK_LDO_CFG	= 0x2a,
	RK816_SW2_CFG 			= 0xa6,
	RK816_BUCK5_ON_VSEL 	= 0x2b,
	RK816_BUCK5_SLP_VSEL 	= 0x2c,
	RK816_BUCK5_CFG 		= 0x2d,
	RK816_BUCK1_CFG 		= 0x2e,
	RK816_BUCK1_ON_VSEL 	= 0x2f,
	RK816_BUCK1_SLP_VSEL 	= 0x30,
	RK816_BUCK2_CFG 		= 0x32,
	RK816_BUCK2_ON_VSEL 	= 0x33,
	RK816_BUCK2_SLP_VSEL 	= 0x34,
	RK816_BUCK3_CFG 		= 0x36,
	RK816_BUCK4_CFG 		= 0x37,
	RK816_BUCK4_ON_VSEL 	= 0x38,
	RK816_BUCK4_SLP_VSEL 	= 0x39,
	RK816_LDO1_ON_VSEL 		= 0x3b,
	RK816_LDO1_SLP_VSEL 	= 0x3c,
	RK816_LDO2_ON_VSEL 		= 0x3d,
	RK816_LDO2_SLP_VSEL 	= 0x3e,
	RK816_LDO3_ON_VSEL 		= 0x3f,
	RK816_LDO3_SLP_VSEL 	= 0x40,
	RK816_LDO4_ON_VSEL 		= 0x41,
	RK816_LDO4_SLP_VSEL 	= 0x42,
	RK816_LDO5_ON_VSEL 		= 0x43,
	RK816_LDO5_SLP_VSEL 	= 0x44,
	RK816_LDO6_ON_VSEL 		= 0x45,
	RK816_LDO6_SLP_VSEL 	= 0x46,

	RK816_INT_STS1			= 0x49,
	RK816_INT_STS1_MSK		= 0x4a,
	RK816_INT_STS2			= 0x4c,
	RK816_INT_STS2_MSK		= 0x4d,
	RK816_INT_STS3			= 0x4e,
	RK816_INT_STS3_MSK		= 0x4f,
	RK816_GPIO_IO_POL		= 0x50,
};

struct regulator_rk816_pdata_t {
	struct i2c_device_t * dev;
	int channel;
};

static bool_t rk816_read(struct i2c_device_t * dev, u8_t reg, u8_t * val)
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

static bool_t rk816_write(struct i2c_device_t * dev, u8_t reg, u8_t val)
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

static void regulator_rk816_set_parent(struct regulator_t * supply, const char * pname)
{
}

static const char * regulator_rk816_get_parent(struct regulator_t * supply)
{
	return NULL;
}

static void regulator_rk816_set_enable(struct regulator_t * supply, bool_t enable)
{
	struct regulator_rk816_pdata_t * pdat = (struct regulator_rk816_pdata_t *)supply->priv;
	u8_t val = 0;

	switch(pdat->channel)
	{
	case 0 ... 3:
		val = (enable ? 0x11 : 0x10) << pdat->channel;
		rk816_write(pdat->dev, RK816_DCDC_EN1, val);
		break;

	case 4 ... 7:
		val = (enable ? 0x11 : 0x10) << (pdat->channel - 4);
		rk816_write(pdat->dev, RK816_LDO_EN1, val);
		break;

	case 8 ... 9:
		val = (enable ? 0x11 : 0x10) << (pdat->channel - 8);
		rk816_write(pdat->dev, RK816_LDO_EN2, val);
		break;

	default:
		break;
	}
}

static bool_t regulator_rk816_get_enable(struct regulator_t * supply)
{
	struct regulator_rk816_pdata_t * pdat = (struct regulator_rk816_pdata_t *)supply->priv;
	u8_t val = 0;

	switch(pdat->channel)
	{
	case 0 ... 3:
		rk816_read(pdat->dev, RK816_DCDC_EN1, &val);
		val = (val >> pdat->channel) & 0x1;
		break;

	case 4 ... 7:
		rk816_read(pdat->dev, RK816_LDO_EN1, &val);
		val = (val >> (pdat->channel - 4)) & 0x1;
		break;

	case 8 ... 9:
		rk816_read(pdat->dev, RK816_LDO_EN2, &val);
		val = (val >> (pdat->channel - 8)) & 0x1;
		break;

	default:
		break;
	}

	return (val != 0) ? TRUE : FALSE;
}

static u8_t rk816_vol_to_reg(int vol, int step, int min, int max)
{
	int reg;

	if(vol < min)
		vol = min;
	else if(vol > max)
		vol = max;

	reg = (vol - min + step - 1) / step;
	return (u8_t)(reg & 0xff);
}

static int rk816_reg_to_vol(u8_t reg, int step, int min, int max)
{
	int vol = (int)reg * step + min;

	if(vol < min)
		vol = min;
	else if(vol > max)
		vol = max;
	return vol;
}

static void regulator_rk816_set_voltage(struct regulator_t * supply, int voltage)
{
	struct regulator_rk816_pdata_t * pdat = (struct regulator_rk816_pdata_t *)supply->priv;
	u8_t val;

	switch(pdat->channel)
	{
	/* DCDC1 - 0.7125V ~ 1.45V, 0.0125V/step, 2A */
	case 0:
		val = rk816_vol_to_reg(voltage, 12500, 712500, 1450000);
		rk816_write(pdat->dev, RK816_BUCK1_ON_VSEL, val);
		break;

	/* DCDC2 - 0.7125V ~ 1.45V, 0.0125V/step, 2A */
	case 1:
		val = rk816_vol_to_reg(voltage, 12500, 712500, 1450000);
		rk816_write(pdat->dev, RK816_BUCK2_ON_VSEL, val);
		break;

	/* DCDC3 - 1.0V ~ 1.8V, 1A */
	case 2:
		break;

	/* DCDC4 - 0.8V ~ 3.5V, 0.1V/step, 1A */
	case 3:
		val = rk816_vol_to_reg(voltage, 100000, 800000, 3500000);
		rk816_write(pdat->dev, RK816_BUCK4_ON_VSEL, val);
		break;

	/* LDO1 - 0.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 4:
		val = rk816_vol_to_reg(voltage, 100000, 800000, 3400000);
		rk816_write(pdat->dev, RK816_LDO1_ON_VSEL, val);
		break;

	/* LDO2 - 0.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 5:
		val = rk816_vol_to_reg(voltage, 100000, 800000, 3400000);
		rk816_write(pdat->dev, RK816_LDO2_ON_VSEL, val);
		break;

	/* LDO3 - 0.8V ~ 3.4V, 0.1V/step, 0.1A */
	case 6:
		val = rk816_vol_to_reg(voltage, 100000, 800000, 3400000);
		rk816_write(pdat->dev, RK816_LDO3_ON_VSEL, val);
		break;

	/* LDO4 - 0.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 7:
		val = rk816_vol_to_reg(voltage, 100000, 800000, 3400000);
		rk816_write(pdat->dev, RK816_LDO4_ON_VSEL, val);
		break;

	/* LDO5 - 0.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 8:
		val = rk816_vol_to_reg(voltage, 100000, 800000, 3400000);
		rk816_write(pdat->dev, RK816_LDO5_ON_VSEL, val);
		break;

	/* LDO6 - 0.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 9:
		val = rk816_vol_to_reg(voltage, 100000, 800000, 3400000);
		rk816_write(pdat->dev, RK816_LDO6_ON_VSEL, val);
		break;

	default:
		break;
	}
}

static int regulator_rk816_get_voltage(struct regulator_t * supply)
{
	struct regulator_rk816_pdata_t * pdat = (struct regulator_rk816_pdata_t *)supply->priv;
	int voltage;
	u8_t val = 0;

	switch(pdat->channel)
	{
	/* DCDC1 - 0.7125V ~ 1.45V, 0.0125V/step, 2A */
	case 0:
		rk816_read(pdat->dev, RK816_BUCK1_ON_VSEL, &val);
		voltage = rk816_reg_to_vol(val, 12500, 712500, 1450000);
		break;

	/* DCDC2 - 0.7125V ~ 1.45V, 0.0125V/step, 2A */
	case 1:
		rk816_read(pdat->dev, RK816_BUCK2_ON_VSEL, &val);
		voltage = rk816_reg_to_vol(val, 12500, 712500, 1450000);
		break;

	/* DCDC3 - 1.0V ~ 1.8V, 1A */
	case 2:
		voltage = 1500000;
		break;

	/* DCDC4 - 0.8V ~ 3.5V, 0.1V/step, 1A */
	case 3:
		rk816_read(pdat->dev, RK816_BUCK4_ON_VSEL, &val);
		voltage = rk816_reg_to_vol(val, 100000, 800000, 3500000);
		break;

	/* LDO1 - 0.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 4:
		rk816_read(pdat->dev, RK816_LDO1_ON_VSEL, &val);
		voltage = rk816_reg_to_vol(val, 100000, 800000, 3400000);
		break;

	/* LDO2 - 0.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 5:
		rk816_read(pdat->dev, RK816_LDO2_ON_VSEL, &val);
		voltage = rk816_reg_to_vol(val, 100000, 800000, 3400000);
		break;

	/* LDO3 - 0.8V ~ 3.4V, 0.1V/step, 0.1A */
	case 6:
		rk816_read(pdat->dev, RK816_LDO3_ON_VSEL, &val);
		voltage = rk816_reg_to_vol(val, 100000, 800000, 3400000);
		break;

	/* LDO4 - 0.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 7:
		rk816_read(pdat->dev, RK816_LDO4_ON_VSEL, &val);
		voltage = rk816_reg_to_vol(val, 100000, 800000, 3400000);
		break;

	/* LDO5 - 0.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 8:
		rk816_read(pdat->dev, RK816_LDO5_ON_VSEL, &val);
		voltage = rk816_reg_to_vol(val, 100000, 800000, 3400000);
		break;

	/* LDO6 - 0.8V ~ 3.4V, 0.1V/step, 0.3A */
	case 9:
		rk816_read(pdat->dev, RK816_LDO6_ON_VSEL, &val);
		voltage = rk816_reg_to_vol(val, 100000, 800000, 3400000);
		break;

	default:
		voltage = 0;
		break;
	}

	return voltage;
}

static struct device_t * regulator_rk816_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct regulator_rk816_pdata_t * pdat;
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

	if(!rk816_read(i2cdev, RK816_CHIP_VER, &val))
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct regulator_rk816_pdata_t));
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
	supply->set_parent = regulator_rk816_set_parent;
	supply->get_parent = regulator_rk816_get_parent;
	supply->set_enable = regulator_rk816_set_enable;
	supply->get_enable = regulator_rk816_get_enable;
	supply->set_voltage = regulator_rk816_set_voltage;
	supply->get_voltage = regulator_rk816_get_voltage;
	supply->priv = pdat;

	if(!(dev = register_regulator(supply, drv)))
	{
		i2c_device_free(pdat->dev);
		free(supply->name);
		free(supply->priv);
		free(supply);
		return NULL;
	}
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

static void regulator_rk816_remove(struct device_t * dev)
{
	struct regulator_t * supply = (struct regulator_t *)dev->priv;
	struct regulator_rk816_pdata_t * pdat = (struct regulator_rk816_pdata_t *)supply->priv;

	if(supply)
	{
		unregister_regulator(supply);
		i2c_device_free(pdat->dev);
		free(supply->name);
		free(supply->priv);
		free(supply);
	}
}

static void regulator_rk816_suspend(struct device_t * dev)
{
}

static void regulator_rk816_resume(struct device_t * dev)
{
}

static struct driver_t regulator_rk816 = {
	.name		= "regulator-rk816",
	.probe		= regulator_rk816_probe,
	.remove		= regulator_rk816_remove,
	.suspend	= regulator_rk816_suspend,
	.resume		= regulator_rk816_resume,
};

static __init void regulator_rk816_driver_init(void)
{
	register_driver(&regulator_rk816);
}

static __exit void regulator_rk816_driver_exit(void)
{
	unregister_driver(&regulator_rk816);
}

driver_initcall(regulator_rk816_driver_init);
driver_exitcall(regulator_rk816_driver_exit);
