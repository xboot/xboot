/*
 * driver/regulator-rc5t620.c
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
	RC5T620_LSIVER		= 0x00,
	RC5T620_OTPVER		= 0x01,
	RC5T620_IODAC		= 0x02,
	RC5T620_VINDAC		= 0x03,
	RC5T620_OUT32KEN	= 0x05,

	RC5T620_DC1CTL1		= 0x2c,
	RC5T620_DC1CTL2		= 0x2d,
	RC5T620_DC2CTL1		= 0x2e,
	RC5T620_DC2CTL2		= 0x2f,
	RC5T620_DC3CTL1		= 0x30,
	RC5T620_DC3CTL2		= 0x31,
	RC5T620_DC4CTL1		= 0x32,
	RC5T620_DC4CTL2		= 0x33,
	RC5T620_DC5CTL1		= 0x34,
	RC5T620_DC5CTL2		= 0x35,
	RC5T620_DC1DAC		= 0x36,
	RC5T620_DC2DAC		= 0x37,
	RC5T620_DC3DAC		= 0x38,
	RC5T620_DC4DAC		= 0x39,
	RC5T620_DC5DAC		= 0x3a,
	RC5T620_DC1DAC_SLP	= 0x3b,
	RC5T620_DC2DAC_SLP	= 0x3c,
	RC5T620_DC3DAC_SLP	= 0x3d,
	RC5T620_DC4DAC_SLP	= 0x3e,
	RC5T620_DC5DAC_SLP	= 0x3f,
	RC5T620_DCIREN		= 0x40,
	RC5T620_DCIRQ		= 0x41,
	RC5T620_DCIRMON		= 0x42,

	RC5T620_LDOEN1		= 0x44,
	RC5T620_LDOEN2		= 0x45,
	RC5T620_LDODIS1		= 0x46,
	RC5T620_LDODIS2		= 0x47,
	RC5T620_LDOECO		= 0x48,
	RC5T620_LDOECO_SLP	= 0x4a,
	RC5T620_LDO1DAC		= 0x4c,
	RC5T620_LDO2DAC		= 0x4d,
	RC5T620_LDO3DAC		= 0x4e,
	RC5T620_LDO4DAC		= 0x4f,
	RC5T620_LDO5DAC		= 0x50,
	RC5T620_LDO6DAC		= 0x51,
	RC5T620_LDO7DAC		= 0x52,
	RC5T620_LDO8DAC		= 0x53,
	RC5T620_LDO9DAC		= 0x54,
	RC5T620_LDO10DAC	= 0x55,
	RC5T620_LDORTC1DAC	= 0x56,
	RC5T620_LDORTC2DAC	= 0x57,
	RC5T620_LDO1DAC_SLP	= 0x58,
	RC5T620_LDO2DAC_SLP	= 0x59,
	RC5T620_LDO3DAC_SLP	= 0x5a,
	RC5T620_LDO4DAC_SLP	= 0x5b,
	RC5T620_LDO5DAC_SLP	= 0x5c,
	RC5T620_LDO6DAC_SLP	= 0x5d,
	RC5T620_LDO7DAC_SLP	= 0x5e,
	RC5T620_LDO8DAC_SLP	= 0x5f,
	RC5T620_LDO9DAC_SLP	= 0x60,
	RC5T620_LDO10DAC_SLP= 0x61,
};

struct regulator_rc5t620_pdata_t {
	struct i2c_device_t * dev;
	int channel;
};

static bool_t rc5t620_read(struct i2c_device_t * dev, u8_t reg, u8_t * val)
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

static bool_t rc5t620_write(struct i2c_device_t * dev, u8_t reg, u8_t val)
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

static void regulator_rc5t620_set_parent(struct regulator_t * supply, const char * pname)
{
}

static const char * regulator_rc5t620_get_parent(struct regulator_t * supply)
{
	return NULL;
}

static void regulator_rc5t620_set_enable(struct regulator_t * supply, bool_t enable)
{
	struct regulator_rc5t620_pdata_t * pdat = (struct regulator_rc5t620_pdata_t *)supply->priv;
	u8_t val = 0;

	switch(pdat->channel)
	{
	case 0:
		rc5t620_read(pdat->dev, RC5T620_DC1CTL1, &val);
		if(enable)
			val |= (0x1 << 0);
		else
			val &= ~(0x1 << 0);
		rc5t620_write(pdat->dev, RC5T620_DC1CTL1, val);
		break;

	case 1:
		rc5t620_read(pdat->dev, RC5T620_DC2CTL1, &val);
		if(enable)
			val |= (0x1 << 0);
		else
			val &= ~(0x1 << 0);
		rc5t620_write(pdat->dev, RC5T620_DC2CTL1, val);
		break;

	case 2:
		rc5t620_read(pdat->dev, RC5T620_DC3CTL1, &val);
		if(enable)
			val |= (0x1 << 0);
		else
			val &= ~(0x1 << 0);
		rc5t620_write(pdat->dev, RC5T620_DC3CTL1, val);
		break;

	case 3:
		rc5t620_read(pdat->dev, RC5T620_DC4CTL1, &val);
		if(enable)
			val |= (0x1 << 0);
		else
			val &= ~(0x1 << 0);
		rc5t620_write(pdat->dev, RC5T620_DC4CTL1, val);
		break;

	case 4:
		rc5t620_read(pdat->dev, RC5T620_DC5CTL1, &val);
		if(enable)
			val |= (0x1 << 0);
		else
			val &= ~(0x1 << 0);
		rc5t620_write(pdat->dev, RC5T620_DC5CTL1, val);
		break;

	case 5 ... 12:
		rc5t620_read(pdat->dev, RC5T620_LDOEN1, &val);
		if(enable)
			val |= (0x1 << (pdat->channel - 5));
		else
			val &= ~(0x1 << (pdat->channel - 5));
		rc5t620_write(pdat->dev, RC5T620_LDOEN1, val);
		break;

	case 13 ... 14:
		rc5t620_read(pdat->dev, RC5T620_LDOEN2, &val);
		if(enable)
			val |= (0x1 << (pdat->channel - 13));
		else
			val &= ~(0x1 << (pdat->channel - 13));
		rc5t620_write(pdat->dev, RC5T620_LDOEN2, val);
		break;

	case 15 ... 16:
		rc5t620_read(pdat->dev, RC5T620_LDOEN2, &val);
		if(enable)
			val |= (0x1 << (pdat->channel - 15 + 4));
		else
			val &= ~(0x1 << (pdat->channel - 15 + 4));
		rc5t620_write(pdat->dev, RC5T620_LDOEN2, val);
		break;

	default:
		break;
	}
}

static bool_t regulator_rc5t620_get_enable(struct regulator_t * supply)
{
	struct regulator_rc5t620_pdata_t * pdat = (struct regulator_rc5t620_pdata_t *)supply->priv;
	u8_t val = 0;

	switch(pdat->channel)
	{
	case 0:
		rc5t620_read(pdat->dev, RC5T620_DC1CTL1, &val);
		val &= 0x1;
		break;

	case 1:
		rc5t620_read(pdat->dev, RC5T620_DC2CTL1, &val);
		val &= 0x1;
		break;

	case 2:
		rc5t620_read(pdat->dev, RC5T620_DC3CTL1, &val);
		val &= 0x1;
		break;

	case 3:
		rc5t620_read(pdat->dev, RC5T620_DC4CTL1, &val);
		val &= 0x1;
		break;

	case 4:
		rc5t620_read(pdat->dev, RC5T620_DC5CTL1, &val);
		val &= 0x1;
		break;

	case 5 ... 12:
		rc5t620_read(pdat->dev, RC5T620_LDOEN1, &val);
		val = (val >> (pdat->channel - 5)) & 0x1;
		break;

	case 13 ... 14:
		rc5t620_read(pdat->dev, RC5T620_LDOEN2, &val);
		val = (val >> (pdat->channel - 13)) & 0x1;
		break;

	case 15 ... 16:
		rc5t620_read(pdat->dev, RC5T620_LDOEN2, &val);
		val = (val >> (pdat->channel - 15 + 4)) & 0x1;
		break;

	default:
		break;
	}

	return (val != 0) ? TRUE : FALSE;
}

static u8_t rc5t620_vol_to_reg(int vol, int step, int min, int max)
{
	int reg;

	if(vol < min)
		vol = min;
	else if(vol > max)
		vol = max;

	reg = (vol - min + step - 1) / step;
	return (u8_t)(reg & 0xff);
}

static int rc5t620_reg_to_vol(u8_t reg, int step, int min, int max)
{
	int vol = (int)reg * step + min;

	if(vol < min)
		vol = min;
	else if(vol > max)
		vol = max;
	return vol;
}

static void regulator_rc5t620_set_voltage(struct regulator_t * supply, int voltage)
{
	struct regulator_rc5t620_pdata_t * pdat = (struct regulator_rc5t620_pdata_t *)supply->priv;
	u8_t val;

	switch(pdat->channel)
	{
	/* DC1 - 0.6V ~ 3.5V, 0.0125V/step, 3A */
	case 0:
		val = rc5t620_vol_to_reg(voltage, 12500, 600000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_DC1DAC, val);
		break;

	/* DC2 - 0.6V ~ 3.5V, 0.0125V/step, 3A */
	case 1:
		val = rc5t620_vol_to_reg(voltage, 12500, 600000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_DC2DAC, val);
		break;

	/* DC3 - 0.6V ~ 3.5V, 0.0125V/step, 3A */
	case 2:
		val = rc5t620_vol_to_reg(voltage, 12500, 600000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_DC3DAC, val);
		break;

	/* DC4 - 0.6V ~ 3.5V, 0.0125V/step, 2A */
	case 3:
		val = rc5t620_vol_to_reg(voltage, 12500, 600000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_DC4DAC, val);
		break;

	/* DC5 - 0.6V ~ 3.5V, 0.0125V/step, 2A */
	case 4:
		val = rc5t620_vol_to_reg(voltage, 12500, 600000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_DC5DAC, val);
		break;

	/* LDO1 - 0.9V ~ 3.5V, 0.025V/step, 0.3A */
	case 5:
		val = rc5t620_vol_to_reg(voltage, 25000, 900000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_LDO1DAC, val);
		break;

	/* LDO2 - 0.9V ~ 3.5V, 0.025V/step, 0.3A */
	case 6:
		val = rc5t620_vol_to_reg(voltage, 25000, 900000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_LDO2DAC, val);
		break;

	/* LDO3 - 0.9V ~ 3.5V, 0.025V/step, 0.3A */
	case 7:
		val = rc5t620_vol_to_reg(voltage, 25000, 900000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_LDO3DAC, val);
		break;

	/* LDO4 - 0.9V ~ 3.5V, 0.025V/step, 0.3A */
	case 8:
		val = rc5t620_vol_to_reg(voltage, 25000, 900000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_LDO4DAC, val);
		break;

	/* LDO5 - 0.6V ~ 3.5V, 0.025V/step, 0.3A */
	case 9:
		val = rc5t620_vol_to_reg(voltage, 25000, 600000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_LDO5DAC, val);
		break;

	/* LDO6 - 0.6V ~ 3.5V, 0.025V/step, 0.3A */
	case 10:
		val = rc5t620_vol_to_reg(voltage, 25000, 600000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_LDO6DAC, val);
		break;

	/* LDO7 - 0.9V ~ 3.5V, 0.025V/step, 0.2A */
	case 11:
		val = rc5t620_vol_to_reg(voltage, 25000, 900000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_LDO7DAC, val);
		break;

	/* LDO8 - 0.9V ~ 3.5V, 0.025V/step, 0.2A */
	case 12:
		val = rc5t620_vol_to_reg(voltage, 25000, 900000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_LDO8DAC, val);
		break;

	/* LDO9 - 0.9V ~ 3.5V, 0.025V/step, 0.2A */
	case 13:
		val = rc5t620_vol_to_reg(voltage, 25000, 900000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_LDO9DAC, val);
		break;

	/* LDO10 - 0.9V ~ 3.5V, 0.025V/step, 0.2A */
	case 14:
		val = rc5t620_vol_to_reg(voltage, 25000, 900000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_LDO10DAC, val);
		break;

	/* LDORTC1 - 1.7V ~ 3.5V, 0.025V/step, 0.01A */
	case 15:
		val = rc5t620_vol_to_reg(voltage, 25000, 1700000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_LDORTC1DAC, val);
		break;

	/* LDORTC2 - 0.9V ~ 3.5V, 0.025V/step, 0.01A */
	case 16:
		val = rc5t620_vol_to_reg(voltage, 25000, 900000, 3500000);
		rc5t620_write(pdat->dev, RC5T620_LDORTC2DAC, val);
		break;

	default:
		break;
	}
}

static int regulator_rc5t620_get_voltage(struct regulator_t * supply)
{
	struct regulator_rc5t620_pdata_t * pdat = (struct regulator_rc5t620_pdata_t *)supply->priv;
	int voltage;
	u8_t val = 0;

	switch(pdat->channel)
	{
	/* DC1 - 0.6V ~ 3.5V, 0.0125V/step, 3A */
	case 0:
		rc5t620_read(pdat->dev, RC5T620_DC1DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 12500, 600000, 3500000);
		break;

	/* DC2 - 0.6V ~ 3.5V, 0.0125V/step, 3A */
	case 1:
		rc5t620_read(pdat->dev, RC5T620_DC2DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 12500, 600000, 3500000);
		break;

	/* DC3 - 0.6V ~ 3.5V, 0.0125V/step, 3A */
	case 2:
		rc5t620_read(pdat->dev, RC5T620_DC3DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 12500, 600000, 3500000);
		break;

	/* DC4 - 0.6V ~ 3.5V, 0.0125V/step, 2A */
	case 3:
		rc5t620_read(pdat->dev, RC5T620_DC4DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 12500, 600000, 3500000);
		break;

	/* DC5 - 0.6V ~ 3.5V, 0.0125V/step, 2A */
	case 4:
		rc5t620_read(pdat->dev, RC5T620_DC5DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 12500, 600000, 3500000);
		break;

	/* LDO1 - 0.9V ~ 3.5V, 0.025V/step, 0.3A */
	case 5:
		rc5t620_read(pdat->dev, RC5T620_LDO1DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 25000, 900000, 3500000);
		break;

	/* LDO2 - 0.9V ~ 3.5V, 0.025V/step, 0.3A */
	case 6:
		rc5t620_read(pdat->dev, RC5T620_LDO2DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 25000, 900000, 3500000);
		break;

	/* LDO3 - 0.9V ~ 3.5V, 0.025V/step, 0.3A */
	case 7:
		rc5t620_read(pdat->dev, RC5T620_LDO3DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 25000, 900000, 3500000);
		break;

	/* LDO4 - 0.9V ~ 3.5V, 0.025V/step, 0.3A */
	case 8:
		rc5t620_read(pdat->dev, RC5T620_LDO4DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 25000, 900000, 3500000);
		break;

	/* LDO5 - 0.6V ~ 3.5V, 0.025V/step, 0.3A */
	case 9:
		rc5t620_read(pdat->dev, RC5T620_LDO5DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 25000, 600000, 3500000);
		break;

	/* LDO6 - 0.6V ~ 3.5V, 0.025V/step, 0.3A */
	case 10:
		rc5t620_read(pdat->dev, RC5T620_LDO6DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 25000, 600000, 3500000);
		break;

	/* LDO7 - 0.9V ~ 3.5V, 0.025V/step, 0.2A */
	case 11:
		rc5t620_read(pdat->dev, RC5T620_LDO7DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 25000, 900000, 3500000);
		break;

	/* LDO8 - 0.9V ~ 3.5V, 0.025V/step, 0.2A */
	case 12:
		rc5t620_read(pdat->dev, RC5T620_LDO8DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 25000, 900000, 3500000);
		break;

	/* LDO9 - 0.9V ~ 3.5V, 0.025V/step, 0.2A */
	case 13:
		rc5t620_read(pdat->dev, RC5T620_LDO9DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 25000, 900000, 3500000);
		break;

	/* LDO10 - 0.9V ~ 3.5V, 0.025V/step, 0.2A */
	case 14:
		rc5t620_read(pdat->dev, RC5T620_LDO10DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 25000, 900000, 3500000);
		break;

	/* LDORTC1 - 1.7V ~ 3.5V, 0.025V/step, 0.01A */
	case 15:
		rc5t620_read(pdat->dev, RC5T620_LDORTC1DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 25000, 1700000, 3500000);
		break;

	/* LDORTC2 - 0.9V ~ 3.5V, 0.025V/step, 0.01A */
	case 16:
		rc5t620_read(pdat->dev, RC5T620_LDORTC2DAC, &val);
		voltage = rc5t620_reg_to_vol(val, 25000, 900000, 3500000);
		break;

	default:
		voltage = 0;
		break;
	}

	return voltage;
}

static struct device_t * regulator_rc5t620_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct regulator_rc5t620_pdata_t * pdat;
	struct regulator_t * supply;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	struct dtnode_t o;
	char * name = dt_read_string(n, "name", NULL);
	int channel = dt_read_int(n, "channel", -1);
	u8_t val;

	if(!name || search_regulator(name))
		return NULL;

	if(channel < 0 || channel > 16)
		return NULL;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x32), 0);
	if(!i2cdev)
		return NULL;

	if(!rc5t620_read(i2cdev, RC5T620_LSIVER, &val) || (val != 0x03))
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct regulator_rc5t620_pdata_t));
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
	supply->set_parent = regulator_rc5t620_set_parent;
	supply->get_parent = regulator_rc5t620_get_parent;
	supply->set_enable = regulator_rc5t620_set_enable;
	supply->get_enable = regulator_rc5t620_get_enable;
	supply->set_voltage = regulator_rc5t620_set_voltage;
	supply->get_voltage = regulator_rc5t620_get_voltage;
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

static void regulator_rc5t620_remove(struct device_t * dev)
{
	struct regulator_t * supply = (struct regulator_t *)dev->priv;
	struct regulator_rc5t620_pdata_t * pdat = (struct regulator_rc5t620_pdata_t *)supply->priv;

	if(supply)
	{
		unregister_regulator(supply);
		i2c_device_free(pdat->dev);
		free(supply->name);
		free(supply->priv);
		free(supply);
	}
}

static void regulator_rc5t620_suspend(struct device_t * dev)
{
}

static void regulator_rc5t620_resume(struct device_t * dev)
{
}

static struct driver_t regulator_rc5t620 = {
	.name		= "regulator-rc5t620",
	.probe		= regulator_rc5t620_probe,
	.remove		= regulator_rc5t620_remove,
	.suspend	= regulator_rc5t620_suspend,
	.resume		= regulator_rc5t620_resume,
};

static __init void regulator_rc5t620_driver_init(void)
{
	register_driver(&regulator_rc5t620);
}

static __exit void regulator_rc5t620_driver_exit(void)
{
	unregister_driver(&regulator_rc5t620);
}

driver_initcall(regulator_rc5t620_driver_init);
driver_exitcall(regulator_rc5t620_driver_exit);
