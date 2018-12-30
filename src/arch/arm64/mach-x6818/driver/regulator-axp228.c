/*
 * driver/regulator-axp228.c
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
	AXP228_POWER_STATUS			= 0x00,
	AXP228_CHARGE_STATUS		= 0x01,
	AXP228_CHIP_ID				= 0x03,
	AXP228_BUFFER1				= 0x04,
	AXP228_BUFFER2				= 0x05,
	AXP228_BUFFER3				= 0x06,
	AXP228_BUFFER4				= 0x07,
	AXP228_BUFFER5				= 0x08,
	AXP228_BUFFER6				= 0x09,
	AXP228_BUFFER7				= 0x0A,
	AXP228_BUFFER8				= 0x0B,
	AXP228_BUFFER9				= 0x0C,
	AXP228_BUFFERA				= 0x0D,
	AXP228_BUFFERB				= 0x0E,
	AXP228_BUFFERC				= 0x0F,

	AXP228_IPS_SET				= 0x30,
	AXP228_VOFF_SET				= 0x31,
	AXP228_OFF_CTL				= 0x32,
	AXP228_CHARGE1				= 0x33,
	AXP228_CHARGE2				= 0x34,
	AXP228_CHARGE3				= 0x35,
	AXP228_POK_SET				= 0x36,
	AXP228_INTEN1				= 0x40,
	AXP228_INTEN2				= 0x41,
	AXP228_INTEN3				= 0x42,
	AXP228_INTEN4				= 0x43,
	AXP228_INTEN5				= 0x44,
	AXP228_INTSTS1				= 0x48,
	AXP228_INTSTS2				= 0x49,
	AXP228_INTSTS3				= 0x4A,
	AXP228_INTSTS4				= 0x4B,
	AXP228_INTSTS5				= 0x4C,

	AXP228_LDO_DC_EN1			= 0X10,
	AXP228_LDO_DC_EN2			= 0X12,
	AXP228_LDO_DC_EN3			= 0X13,
	AXP228_DLDO1OUT_VOL			= 0x15,
	AXP228_DLDO2OUT_VOL			= 0x16,
	AXP228_DLDO3OUT_VOL			= 0x17,
	AXP228_DLDO4OUT_VOL			= 0x18,
	AXP228_ELDO1OUT_VOL			= 0x19,
	AXP228_ELDO2OUT_VOL			= 0x1A,
	AXP228_ELDO3OUT_VOL			= 0x1B,
	AXP228_DC5LDOOUT_VOL		= 0x1C,
	AXP228_DC1OUT_VOL			= 0x21,
	AXP228_DC2OUT_VOL			= 0x22,
	AXP228_DC3OUT_VOL			= 0x23,
	AXP228_DC4OUT_VOL			= 0x24,
	AXP228_DC5OUT_VOL			= 0x25,
	AXP228_GPIO0LDOOUT_VOL		= 0x91,
	AXP228_GPIO1LDOOUT_VOL		= 0x93,
	AXP228_ALDO1OUT_VOL			= 0x28,
	AXP228_ALDO2OUT_VOL			= 0x29,
	AXP228_ALDO3OUT_VOL			= 0x2A,

	AXP228_DCDC_MODESET			= 0x80,
	AXP228_DCDC_FREQSET			= 0x37,
	AXP228_ADC_EN				= 0x82,
	AXP228_PWREN_CTL1			= 0x8C,
	AXP228_PWREN_CTL2			= 0x8D,
	AXP228_HOTOVER_CTL			= 0x8F,

	AXP228_GPIO0_CTL			= 0x90,
	AXP228_GPIO1_CTL			= 0x92,
	AXP228_GPIO01_SIGNAL		= 0x94,
	AXP228_BAT_CHGCOULOMB3		= 0xB0,
	AXP228_BAT_CHGCOULOMB2		= 0xB1,
	AXP228_BAT_CHGCOULOMB1		= 0xB2,
	AXP228_BAT_CHGCOULOMB0		= 0xB3,
	AXP228_BAT_DISCHGCOULOMB3	= 0xB4,
	AXP228_BAT_DISCHGCOULOMB2	= 0xB5,
	AXP228_BAT_DISCHGCOULOMB1	= 0xB6,
	AXP228_BAT_DISCHGCOULOMB0	= 0xB7,
	AXP228_COULOMB_CTL			= 0xB8,

	AXP228_INTERTEMPH_RES		= 0x56,
	AXP228_INTERTEMPL_RES		= 0x57,
	AXP228_BATTEMPH_RES			= 0x58,
	AXP228_BATTEMPL_RES			= 0x59,
	AXP228_VBATH_RES			= 0x78,
	AXP228_VBATL_RES			= 0x79,
	AXP228_CHGCURRH_RES			= 0x7A,
	AXP228_CHGCURRL_RES			= 0x7B,
	AXP228_DISCHGCURRH_RES		= 0x7C,
	AXP228_DISCHGCURRL_RES		= 0x7D,

	AXP228_CAP					= 0xB9,
	AXP228_BATCAP0				= 0xE0,
	AXP228_BATCAP1				= 0xE1,
	AXP228_RDC0					= 0xBA,
	AXP228_RDC1					= 0xBB,
	AXP228_WARNING_LEVEL		= 0xE6,
	AXP228_ADJUST_PARA			= 0xE8,
};

struct regulator_axp228_pdata_t {
	struct i2c_device_t * dev;
	int channel;
};

static bool_t axp228_read(struct i2c_device_t * dev, u8_t reg, u8_t * val)
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

static bool_t axp228_write(struct i2c_device_t * dev, u8_t reg, u8_t val)
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

static void regulator_axp228_set_parent(struct regulator_t * supply, const char * pname)
{
}

static const char * regulator_axp228_get_parent(struct regulator_t * supply)
{
	return NULL;
}

static void regulator_axp228_set_enable(struct regulator_t * supply, bool_t enable)
{
	struct regulator_axp228_pdata_t * pdat = (struct regulator_axp228_pdata_t *)supply->priv;
	u8_t val = 0;

	switch(pdat->channel)
	{
	case 0 ... 7:
		axp228_read(pdat->dev, AXP228_LDO_DC_EN1, &val);
		if(enable)
			val |= (0x1 << pdat->channel);
		else
			val &= ~(0x1 << pdat->channel);
		axp228_write(pdat->dev, AXP228_LDO_DC_EN1, val);
		break;

	case 8 ... 14:
		axp228_read(pdat->dev, AXP228_LDO_DC_EN2, &val);
		if(enable)
			val |= (0x1 << (pdat->channel - 8));
		else
			val &= ~(0x1 << (pdat->channel - 8));
		axp228_write(pdat->dev, AXP228_LDO_DC_EN2, val);
		break;

	case 15:
		axp228_read(pdat->dev, AXP228_LDO_DC_EN3, &val);
		if(enable)
			val |= (0x1 << 7);
		else
			val &= ~(0x1 << 7);
		axp228_write(pdat->dev, AXP228_LDO_DC_EN3, val);
		break;

	default:
		break;
	}
}

static bool_t regulator_axp228_get_enable(struct regulator_t * supply)
{
	struct regulator_axp228_pdata_t * pdat = (struct regulator_axp228_pdata_t *)supply->priv;
	u8_t val = 0;

	switch(pdat->channel)
	{
	case 0 ... 7:
		axp228_read(pdat->dev, AXP228_LDO_DC_EN1, &val);
		val = (val >> pdat->channel) & 0x1;
		break;

	case 8 ... 14:
		axp228_read(pdat->dev, AXP228_LDO_DC_EN2, &val);
		val = (val >> (pdat->channel - 8)) & 0x1;
		break;

	case 15:
		axp228_read(pdat->dev, AXP228_LDO_DC_EN3, &val);
		val = (val >> 7) & 0x1;
		break;

	default:
		break;
	}

	return (val != 0) ? TRUE : FALSE;
}

static u8_t axp228_get_reg_with_vol(int vol, int step, int min, int max)
{
	int reg;

	if(vol < min)
		vol = min;
	else if(vol > max)
		vol = max;

	reg = (vol - min + step - 1) / step;
	return (u8_t)(reg & 0xff);
}

static int axp228_get_vol_with_reg(u8_t reg, int step, int min, int max)
{
	int vol = (int)reg * step + min;

	if(vol < min)
		vol = min;
	else if(vol > max)
		vol = max;
	return vol;
}

static void regulator_axp228_set_voltage(struct regulator_t * supply, int voltage)
{
	struct regulator_axp228_pdata_t * pdat = (struct regulator_axp228_pdata_t *)supply->priv;
	u8_t val;

	voltage = voltage / 1000;
	switch(pdat->channel)
	{
	/* DC5LDO - 0.7V ~ 1.4V, 100mV/step, 200mA */
	case 0:
		val = axp228_get_reg_with_vol(voltage, 100, 700, 1400);
		axp228_write(pdat->dev, AXP228_DC5LDOOUT_VOL, val);
		break;

	/* DCDC1 - 1.6V ~ 3.4V, 100mV/step, 1.4A */
	case 1:
		val = axp228_get_reg_with_vol(voltage, 100, 1600, 3400);
		axp228_write(pdat->dev, AXP228_DC1OUT_VOL, val);
		break;

	/* DCDC2 - 0.6V ~ 1.54V, 20mV/step, 2.5A */
	case 2:
		val = axp228_get_reg_with_vol(voltage, 20, 600, 1540);
		axp228_write(pdat->dev, AXP228_DC2OUT_VOL, val);
		break;

	/* DCDC3 - 0.6V ~ 1.86V, 20mV/step, 2.5A */
	case 3:
		val = axp228_get_reg_with_vol(voltage, 20, 600, 1860);
		axp228_write(pdat->dev, AXP228_DC3OUT_VOL, val);
		break;

	/* DCDC4 - 0.6V ~ 1.54V, 20mV/step, 0.6A */
	case 4:
		val = axp228_get_reg_with_vol(voltage, 20, 600, 1540);
		axp228_write(pdat->dev, AXP228_DC4OUT_VOL, val);
		break;

	/* DCDC5 - 1.0V ~ 2.55V, 50mV/step, 2A */
	case 5:
		val = axp228_get_reg_with_vol(voltage, 50, 1000, 2550);
		axp228_write(pdat->dev, AXP228_DC5OUT_VOL, val);
		break;

	/* ALDO1 - 0.7V ~ 3.3V, 100mV/step, 300mA */
	case 6:
		val = axp228_get_reg_with_vol(voltage, 100, 700, 3300);
		axp228_write(pdat->dev, AXP228_ALDO1OUT_VOL, val);
		break;

	/* ALDO2 - 0.7V ~ 3.3V, 100mV/step, 300mA */
	case 7:
		val = axp228_get_reg_with_vol(voltage, 100, 700, 3300);
		axp228_write(pdat->dev, AXP228_ALDO2OUT_VOL, val);
		break;

	/* ELDO1 - 0.7V ~ 3.3V, 100mV/step, 400mA */
	case 8:
		val = axp228_get_reg_with_vol(voltage, 100, 700, 3300);
		axp228_write(pdat->dev, AXP228_ELDO1OUT_VOL, val);
		break;

	/* ELDO2 - 0.7V ~ 3.3V, 100mV/step, 200mA */
	case 9:
		val = axp228_get_reg_with_vol(voltage, 100, 700, 3300);
		axp228_write(pdat->dev, AXP228_ELDO2OUT_VOL, val);
		break;

	/* ELDO3 - 0.7V ~ 3.3V, 100mV/step, 200mA */
	case 10:
		val = axp228_get_reg_with_vol(voltage, 100, 700, 3300);
		axp228_write(pdat->dev, AXP228_ELDO3OUT_VOL, val);
		break;

	/* DLDO1 - 0.7V ~ 3.3V, 100mV/step, 400mA */
	case 11:
		val = axp228_get_reg_with_vol(voltage, 100, 700, 3300);
		axp228_write(pdat->dev, AXP228_DLDO1OUT_VOL, val);
		break;

	/* DLDO2 - 0.7V ~ 3.3V, 100mV/step, 200mA */
	case 12:
		val = axp228_get_reg_with_vol(voltage, 100, 700, 3300);
		axp228_write(pdat->dev, AXP228_DLDO2OUT_VOL, val);
		break;

	/* DLDO3 - 0.7V ~ 3.3V, 100mV/step, 200mA */
	case 13:
		val = axp228_get_reg_with_vol(voltage, 100, 700, 3300);
		axp228_write(pdat->dev, AXP228_DLDO3OUT_VOL, val);
		break;

	/* DLDO4 - 0.7V ~ 3.3V, 100mV/step, 100mA */
	case 14:
		val = axp228_get_reg_with_vol(voltage, 100, 700, 3300);
		axp228_write(pdat->dev, AXP228_DLDO4OUT_VOL, val);
		break;

	/* ALDO3 - 0.7V ~ 3.3V, 100mV/step, 200mA */
	case 15:
		val = axp228_get_reg_with_vol(voltage, 100, 700, 3300);
		axp228_write(pdat->dev, AXP228_ALDO3OUT_VOL, val);
		break;

	default:
		break;
	}
}

static int regulator_axp228_get_voltage(struct regulator_t * supply)
{
	struct regulator_axp228_pdata_t * pdat = (struct regulator_axp228_pdata_t *)supply->priv;
	int voltage;
	u8_t val = 0;

	switch(pdat->channel)
	{
	/* DC5LDO - 0.7V ~ 1.4V, 100mV/step, 200mA */
	case 0:
		axp228_read(pdat->dev, AXP228_DC5LDOOUT_VOL, &val);
		voltage = axp228_get_vol_with_reg(val, 100, 700, 1400);
		break;

	/* DCDC1 - 1.6V ~ 3.4V, 100mV/step, 1.4A */
	case 1:
		axp228_read(pdat->dev, AXP228_DC1OUT_VOL, &val);
		voltage = axp228_get_vol_with_reg(val, 100, 1600, 3400);
		break;

	/* DCDC2 - 0.6V ~ 1.54V, 20mV/step, 2.5A */
	case 2:
		axp228_read(pdat->dev, AXP228_DC2OUT_VOL, &val);
		voltage = axp228_get_vol_with_reg(val, 20, 600, 1540);
		break;

	/* DCDC3 - 0.6V ~ 1.86V, 20mV/step, 2.5A */
	case 3:
		axp228_read(pdat->dev, AXP228_DC3OUT_VOL, &val);
		voltage = axp228_get_vol_with_reg(val, 20, 600, 1860);
		break;

	/* DCDC4 - 0.6V ~ 1.54V, 20mV/step, 0.6A */
	case 4:
		axp228_read(pdat->dev, AXP228_DC4OUT_VOL, &val);
		voltage = axp228_get_vol_with_reg(val, 20, 600, 1540);
		break;

	/* DCDC5 - 1.0V ~ 2.55V, 50mV/step, 2A */
	case 5:
		axp228_read(pdat->dev, AXP228_DC5OUT_VOL, &val);
		voltage = axp228_get_vol_with_reg(val, 50, 1000, 2550);
		break;

	/* ALDO1 - 0.7V ~ 3.3V, 100mV/step, 300mA */
	case 6:
		axp228_read(pdat->dev, AXP228_ALDO1OUT_VOL, &val);
		voltage = axp228_get_vol_with_reg(val, 100, 700, 3300);
		break;

	/* ALDO2 - 0.7V ~ 3.3V, 100mV/step, 300mA */
	case 7:
		axp228_read(pdat->dev, AXP228_ALDO2OUT_VOL, &val);
		voltage = axp228_get_vol_with_reg(val, 100, 700, 3300);
		break;

	/* ELDO1 - 0.7V ~ 3.3V, 100mV/step, 400mA */
	case 8:
		axp228_read(pdat->dev, AXP228_ELDO1OUT_VOL, &val);
		voltage = axp228_get_vol_with_reg(val, 100, 700, 3300);
		break;

	/* ELDO2 - 0.7V ~ 3.3V, 100mV/step, 200mA */
	case 9:
		axp228_read(pdat->dev, AXP228_ELDO2OUT_VOL, &val);
		voltage = axp228_get_vol_with_reg(val, 100, 700, 3300);
		break;

	/* ELDO3 - 0.7V ~ 3.3V, 100mV/step, 200mA */
	case 10:
		axp228_read(pdat->dev, AXP228_ELDO3OUT_VOL, &val);
		voltage = axp228_get_vol_with_reg(val, 100, 700, 3300);
		break;

	/* DLDO1 - 0.7V ~ 3.3V, 100mV/step, 400mA */
	case 11:
		axp228_read(pdat->dev, AXP228_DLDO1OUT_VOL, &val);
		voltage = axp228_get_vol_with_reg(val, 100, 700, 3300);
		break;

	/* DLDO2 - 0.7V ~ 3.3V, 100mV/step, 200mA */
	case 12:
		axp228_read(pdat->dev, AXP228_DLDO2OUT_VOL, &val);
		voltage = axp228_get_vol_with_reg(val, 100, 700, 3300);
		break;

	/* DLDO3 - 0.7V ~ 3.3V, 100mV/step, 200mA */
	case 13:
		axp228_read(pdat->dev, AXP228_DLDO3OUT_VOL, &val);
		voltage = axp228_get_vol_with_reg(val, 100, 700, 3300);
		break;

	/* DLDO4 - 0.7V ~ 3.3V, 100mV/step, 100mA */
	case 14:
		axp228_read(pdat->dev, AXP228_DLDO4OUT_VOL, &val);
		voltage = axp228_get_vol_with_reg(val, 100, 700, 3300);
		break;

	/* ALDO3 - 0.7V ~ 3.3V, 100mV/step, 200mA */
	case 15:
		axp228_read(pdat->dev, AXP228_ALDO3OUT_VOL, &val);
		voltage = axp228_get_vol_with_reg(val, 100, 700, 3300);
		break;

	default:
		voltage = 0;
		break;
	}

	return voltage * 1000;
}

static struct device_t * regulator_axp228_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct regulator_axp228_pdata_t * pdat;
	struct regulator_t * supply;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	struct dtnode_t o;
	char * name = dt_read_string(n, "name", NULL);
	int channel = dt_read_int(n, "channel", -1);
	u8_t val;

	if(!name || search_regulator(name))
		return NULL;

	if(channel < 0 || channel > 15)
		return NULL;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), 0x34, 0);
	if(!i2cdev)
		return NULL;

	if(!axp228_read(i2cdev, AXP228_CHIP_ID, &val) || (val != 0x06))
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct regulator_axp228_pdata_t));
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
	supply->set_parent = regulator_axp228_set_parent;
	supply->get_parent = regulator_axp228_get_parent;
	supply->set_enable = regulator_axp228_set_enable;
	supply->get_enable = regulator_axp228_get_enable;
	supply->set_voltage = regulator_axp228_set_voltage;
	supply->get_voltage = regulator_axp228_get_voltage;
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

static void regulator_axp228_remove(struct device_t * dev)
{
	struct regulator_t * supply = (struct regulator_t *)dev->priv;
	struct regulator_axp228_pdata_t * pdat = (struct regulator_axp228_pdata_t *)supply->priv;

	if(supply && unregister_regulator(supply))
	{
		i2c_device_free(pdat->dev);

		free(supply->name);
		free(supply->priv);
		free(supply);
	}
}

static void regulator_axp228_suspend(struct device_t * dev)
{
}

static void regulator_axp228_resume(struct device_t * dev)
{
}

static struct driver_t regulator_axp228 = {
	.name		= "regulator-axp228",
	.probe		= regulator_axp228_probe,
	.remove		= regulator_axp228_remove,
	.suspend	= regulator_axp228_suspend,
	.resume		= regulator_axp228_resume,
};

static __init void regulator_axp228_driver_init(void)
{
	register_driver(&regulator_axp228);
}

static __exit void regulator_axp228_driver_exit(void)
{
	unregister_driver(&regulator_axp228);
}

driver_initcall(regulator_axp228_driver_init);
driver_exitcall(regulator_axp228_driver_exit);
