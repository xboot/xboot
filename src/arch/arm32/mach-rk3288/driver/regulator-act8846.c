/*
 * driver/regulator-act8846.c
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
	ACT8846_DC1_SET_VOL = 0x10,
	ACT8846_DC1_CTL 	= 0x12,

	ACT8846_DC2_SET_VOL = 0x20,
	ACT8846_DC2_SLP_VOL = 0x21,
	ACT8846_DC2_CTL		= 0x22,

	ACT8846_DC3_SET_VOL	= 0x30,
	ACT8846_DC3_SLP_VOL = 0x31,
	ACT8846_DC3_CTL		= 0x32,

	ACT8846_DC4_SET_VOL	= 0x40,
	ACT8846_DC4_SLP_VOL = 0x41,
	ACT8846_DC4_CTL		= 0x42,

	ACT8846_LDO1_VOL	= 0x50,
	ACT8846_LDO1_CTL	= 0x51,
	ACT8846_LDO2_VOL 	= 0x58,
	ACT8846_LDO2_CTL 	= 0x59,
	ACT8846_LDO3_VOL 	= 0x60,
	ACT8846_LDO3_CTL	= 0x61,
	ACT8846_LDO4_VOL 	= 0x68,
	ACT8846_LDO4_CTL 	= 0x69,
	ACT8846_LDO5_VOL 	= 0x70,
	ACT8846_LDO5_CTL 	= 0x71,
	ACT8846_LDO6_VOL 	= 0x80,
	ACT8846_LDO6_CTL 	= 0x81,
	ACT8846_LDO7_VOL 	= 0x90,
	ACT8846_LDO7_CTL 	= 0x91,
	ACT8846_LDO8_VOL 	= 0xa0,
	ACT8846_LDO8_CTL 	= 0xa1,
};

struct regulator_act8846_pdata_t {
	struct i2c_device_t * dev;
	int channel;
};

static const int voltage_map[64] = {
	 600,  625,  650,  675,  700,  725,  750,  775,
	 800,  825,  850,  875,  900,  925,  950,  975,
	1000, 1025, 1050, 1075, 1100, 1125, 1150, 1175,
	1200, 1250, 1300, 1350, 1400, 1450, 1500, 1550,
	1600, 1650, 1700, 1750, 1800, 1850, 1900, 1950,
	2000, 2050, 2100, 2150, 2200, 2250, 2300, 2350,
	2400, 2500, 2600, 2700, 2800, 2900, 3000, 3100,
	3200, 3300, 3400, 3500, 3600, 3700, 3800, 3900,
};

static bool_t act8846_read(struct i2c_device_t * dev, u8_t reg, u8_t * val)
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

static bool_t act8846_write(struct i2c_device_t * dev, u8_t reg, u8_t val)
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

static void regulator_act8846_set_parent(struct regulator_t * supply, const char * pname)
{
}

static const char * regulator_act8846_get_parent(struct regulator_t * supply)
{
	return NULL;
}

static void regulator_act8846_set_enable(struct regulator_t * supply, bool_t enable)
{
	struct regulator_act8846_pdata_t * pdat = (struct regulator_act8846_pdata_t *)supply->priv;
	u8_t val = 0;

	switch(pdat->channel)
	{
	case 0:
		act8846_read(pdat->dev, ACT8846_DC1_CTL, &val);
		if(enable)
			val |= (0x1 << 7);
		else
			val &= ~(0x1 << 7);
		act8846_write(pdat->dev, ACT8846_DC1_CTL, val);
		break;
	case 1:
		act8846_read(pdat->dev, ACT8846_DC2_CTL, &val);
		if(enable)
			val |= (0x1 << 7);
		else
			val &= ~(0x1 << 7);
		act8846_write(pdat->dev, ACT8846_DC2_CTL, val);
		break;
	case 2:
		act8846_read(pdat->dev, ACT8846_DC3_CTL, &val);
		if(enable)
			val |= (0x1 << 7);
		else
			val &= ~(0x1 << 7);
		act8846_write(pdat->dev, ACT8846_DC3_CTL, val);
		break;
	case 3:
		act8846_read(pdat->dev, ACT8846_DC4_CTL, &val);
		if(enable)
			val |= (0x1 << 7);
		else
			val &= ~(0x1 << 7);
		act8846_write(pdat->dev, ACT8846_DC4_CTL, val);
		break;

	case 4:
		act8846_read(pdat->dev, ACT8846_LDO1_CTL, &val);
		if(enable)
			val |= (0x1 << 7);
		else
			val &= ~(0x1 << 7);
		act8846_write(pdat->dev, ACT8846_LDO1_CTL, val);
		break;
	case 5:
		act8846_read(pdat->dev, ACT8846_LDO2_CTL, &val);
		if(enable)
			val |= (0x1 << 7);
		else
			val &= ~(0x1 << 7);
		act8846_write(pdat->dev, ACT8846_LDO2_CTL, val);
		break;
	case 6:
		act8846_read(pdat->dev, ACT8846_LDO3_CTL, &val);
		if(enable)
			val |= (0x1 << 7);
		else
			val &= ~(0x1 << 7);
		act8846_write(pdat->dev, ACT8846_LDO3_CTL, val);
		break;
	case 7:
		act8846_read(pdat->dev, ACT8846_LDO4_CTL, &val);
		if(enable)
			val |= (0x1 << 7);
		else
			val &= ~(0x1 << 7);
		act8846_write(pdat->dev, ACT8846_LDO4_CTL, val);
		break;
	case 8:
		act8846_read(pdat->dev, ACT8846_LDO5_CTL, &val);
		if(enable)
			val |= (0x1 << 7);
		else
			val &= ~(0x1 << 7);
		act8846_write(pdat->dev, ACT8846_LDO5_CTL, val);
		break;
	case 9:
		act8846_read(pdat->dev, ACT8846_LDO6_CTL, &val);
		if(enable)
			val |= (0x1 << 7);
		else
			val &= ~(0x1 << 7);
		act8846_write(pdat->dev, ACT8846_LDO6_CTL, val);
		break;
	case 10:
		act8846_read(pdat->dev, ACT8846_LDO7_CTL, &val);
		if(enable)
			val |= (0x1 << 7);
		else
			val &= ~(0x1 << 7);
		act8846_write(pdat->dev, ACT8846_LDO7_CTL, val);
		break;
	case 11:
		act8846_read(pdat->dev, ACT8846_LDO8_CTL, &val);
		if(enable)
			val |= (0x1 << 7);
		else
			val &= ~(0x1 << 7);
		act8846_write(pdat->dev, ACT8846_LDO8_CTL, val);
		break;
	default:
		break;
	}
}

static bool_t regulator_act8846_get_enable(struct regulator_t * supply)
{
	struct regulator_act8846_pdata_t * pdat = (struct regulator_act8846_pdata_t *)supply->priv;
	u8_t val = 0;

	switch(pdat->channel)
	{
	case 0:
		act8846_read(pdat->dev, ACT8846_DC1_CTL, &val);
		break;
	case 1:
		act8846_read(pdat->dev, ACT8846_DC2_CTL, &val);
		break;
	case 2:
		act8846_read(pdat->dev, ACT8846_DC3_CTL, &val);
		break;
	case 3:
		act8846_read(pdat->dev, ACT8846_DC4_CTL, &val);
		break;
	case 4:
		act8846_read(pdat->dev, ACT8846_LDO1_CTL, &val);
		break;
	case 5:
		act8846_read(pdat->dev, ACT8846_LDO2_CTL, &val);
		break;
	case 6:
		act8846_read(pdat->dev, ACT8846_LDO3_CTL, &val);
		break;
	case 7:
		act8846_read(pdat->dev, ACT8846_LDO4_CTL, &val);
		break;
	case 8:
		act8846_read(pdat->dev, ACT8846_LDO5_CTL, &val);
		break;
	case 9:
		act8846_read(pdat->dev, ACT8846_LDO6_CTL, &val);
		break;
	case 10:
		act8846_read(pdat->dev, ACT8846_LDO7_CTL, &val);
		break;
	case 11:
		act8846_read(pdat->dev, ACT8846_LDO8_CTL, &val);
		break;
	default:
		break;
	}
	return (val & 0x80) ? TRUE : FALSE;
}

static int act8846_vol_to_reg(int voltage)
{
	int i;

	if(voltage > 0)
	{
		for(i = 0; i < ARRAY_SIZE(voltage_map); i++)
		{
			if(voltage <= (voltage_map[i] * 1000))
				return i;
		}
	}
	return -1;
}

static void regulator_act8846_set_voltage(struct regulator_t * supply, int voltage)
{
	struct regulator_act8846_pdata_t * pdat = (struct regulator_act8846_pdata_t *)supply->priv;
	u8_t val = 0, reg;

	reg = act8846_vol_to_reg(voltage);
	if(reg < 0)
		return;

	switch(pdat->channel)
	{
	/* DC1 - 0.6V ~ 3.9V, 1.5A */
	case 0:
		act8846_read(pdat->dev, ACT8846_DC1_SET_VOL, &val);
		val &= ~0x3f;
		val |= reg & 0x3f;
		act8846_write(pdat->dev, ACT8846_DC1_SET_VOL, val);
		break;
	/* DC2 - 0.6V ~ 3.9V, 2.8A */
	case 1:
		act8846_read(pdat->dev, ACT8846_DC2_SET_VOL, &val);
		val &= ~0x3f;
		val |= reg & 0x3f;
		act8846_write(pdat->dev, ACT8846_DC2_SET_VOL, val);
		break;
	/* DC3 - 0.6V ~ 3.9V, 2.8A */
	case 2:
		act8846_read(pdat->dev, ACT8846_DC3_SET_VOL, &val);
		val &= ~0x3f;
		val |= reg & 0x3f;
		act8846_write(pdat->dev, ACT8846_DC3_SET_VOL, val);
		break;
	/* DC4 - 0.6V ~ 3.9V, 1.5A */
	case 3:
		act8846_read(pdat->dev, ACT8846_DC4_SET_VOL, &val);
		val &= ~0x3f;
		val |= reg & 0x3f;
		act8846_write(pdat->dev, ACT8846_DC4_SET_VOL, val);
		break;
	/* LDO1 - 0.6V ~ 3.9V, 150mA */
	case 4:
		act8846_read(pdat->dev, ACT8846_LDO1_VOL, &val);
		val &= ~0x3f;
		val |= reg & 0x3f;
		act8846_write(pdat->dev, ACT8846_LDO1_VOL, val);
		break;
	/* LDO2 - 0.6V ~ 3.9V, 150mA */
	case 5:
		act8846_read(pdat->dev, ACT8846_LDO2_VOL, &val);
		val &= ~0x3f;
		val |= reg & 0x3f;
		act8846_write(pdat->dev, ACT8846_LDO2_VOL, val);
		break;
	/* LDO3 - 0.6V ~ 3.9V, 350mA */
	case 6:
		act8846_read(pdat->dev, ACT8846_LDO3_VOL, &val);
		val &= ~0x3f;
		val |= reg & 0x3f;
		act8846_write(pdat->dev, ACT8846_LDO3_VOL, val);
		break;
	/* LDO4 - 0.6V ~ 3.9V, 350mA */
	case 7:
		act8846_read(pdat->dev, ACT8846_LDO4_VOL, &val);
		val &= ~0x3f;
		val |= reg & 0x3f;
		act8846_write(pdat->dev, ACT8846_LDO4_VOL, val);
		break;
	/* LDO5 - 0.6V ~ 3.9V, 350mA */
	case 8:
		act8846_read(pdat->dev, ACT8846_LDO5_VOL, &val);
		val &= ~0x3f;
		val |= reg & 0x3f;
		act8846_write(pdat->dev, ACT8846_LDO5_VOL, val);
		break;
	/* LDO6 - 0.6V ~ 3.9V, 150mA */
	case 9:
		act8846_read(pdat->dev, ACT8846_LDO6_VOL, &val);
		val &= ~0x3f;
		val |= reg & 0x3f;
		act8846_write(pdat->dev, ACT8846_LDO6_VOL, val);
		break;
	/* LDO7 - 0.6V ~ 3.9V, 350mA */
	case 10:
		act8846_read(pdat->dev, ACT8846_LDO7_VOL, &val);
		val &= ~0x3f;
		val |= reg & 0x3f;
		act8846_write(pdat->dev, ACT8846_LDO7_VOL, val);
		break;
	/* LDO8 - 0.6V ~ 3.9V, 350mA */
	case 11:
		act8846_read(pdat->dev, ACT8846_LDO8_VOL, &val);
		val &= ~0x3f;
		val |= reg & 0x3f;
		act8846_write(pdat->dev, ACT8846_LDO8_VOL, val);
		break;
	default:
		break;
	}
}

static int regulator_act8846_get_voltage(struct regulator_t * supply)
{
	struct regulator_act8846_pdata_t * pdat = (struct regulator_act8846_pdata_t *)supply->priv;
	u8_t val = 0;

	switch(pdat->channel)
	{
	/* DC1 - 0.6V ~ 3.9V, 1.5A */
	case 0:
		act8846_read(pdat->dev, ACT8846_DC1_SET_VOL, &val);
		break;
	/* DC2 - 0.6V ~ 3.9V, 2.8A */
	case 1:
		act8846_read(pdat->dev, ACT8846_DC2_SET_VOL, &val);
		break;
	/* DC3 - 0.6V ~ 3.9V, 2.8A */
	case 2:
		act8846_read(pdat->dev, ACT8846_DC3_SET_VOL, &val);
		break;
	/* DC4 - 0.6V ~ 3.9V, 1.5A */
	case 3:
		act8846_read(pdat->dev, ACT8846_DC4_SET_VOL, &val);
		break;
	/* LDO1 - 0.6V ~ 3.9V, 150mA */
	case 4:
		act8846_read(pdat->dev, ACT8846_LDO1_VOL, &val);
		break;
	/* LDO2 - 0.6V ~ 3.9V, 150mA */
	case 5:
		act8846_read(pdat->dev, ACT8846_LDO2_VOL, &val);
		break;
	/* LDO3 - 0.6V ~ 3.9V, 350mA */
	case 6:
		act8846_read(pdat->dev, ACT8846_LDO3_VOL, &val);
		break;
	/* LDO4 - 0.6V ~ 3.9V, 350mA */
	case 7:
		act8846_read(pdat->dev, ACT8846_LDO4_VOL, &val);
		break;
	/* LDO5 - 0.6V ~ 3.9V, 350mA */
	case 8:
		act8846_read(pdat->dev, ACT8846_LDO5_VOL, &val);
		break;
	/* LDO6 - 0.6V ~ 3.9V, 150mA */
	case 9:
		act8846_read(pdat->dev, ACT8846_LDO6_VOL, &val);
		break;
	/* LDO7 - 0.6V ~ 3.9V, 350mA */
	case 10:
		act8846_read(pdat->dev, ACT8846_LDO7_VOL, &val);
		break;
	/* LDO8 - 0.6V ~ 3.9V, 350mA */
	case 11:
		act8846_read(pdat->dev, ACT8846_LDO8_VOL, &val);
		break;
	default:
		break;
	}
	return voltage_map[val & 0x3f] * 1000;
}

static struct device_t * regulator_act8846_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct regulator_act8846_pdata_t * pdat;
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

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x5a), 0);
	if(!i2cdev)
		return NULL;

	if(!act8846_read(i2cdev, ACT8846_DC1_CTL, &val))
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct regulator_act8846_pdata_t));
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
	supply->set_parent = regulator_act8846_set_parent;
	supply->get_parent = regulator_act8846_get_parent;
	supply->set_enable = regulator_act8846_set_enable;
	supply->get_enable = regulator_act8846_get_enable;
	supply->set_voltage = regulator_act8846_set_voltage;
	supply->get_voltage = regulator_act8846_get_voltage;
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

static void regulator_act8846_remove(struct device_t * dev)
{
	struct regulator_t * supply = (struct regulator_t *)dev->priv;
	struct regulator_act8846_pdata_t * pdat = (struct regulator_act8846_pdata_t *)supply->priv;

	if(supply && unregister_regulator(supply))
	{
		i2c_device_free(pdat->dev);

		free(supply->name);
		free(supply->priv);
		free(supply);
	}
}

static void regulator_act8846_suspend(struct device_t * dev)
{
}

static void regulator_act8846_resume(struct device_t * dev)
{
}

static struct driver_t regulator_act8846 = {
	.name		= "regulator-act8846",
	.probe		= regulator_act8846_probe,
	.remove		= regulator_act8846_remove,
	.suspend	= regulator_act8846_suspend,
	.resume		= regulator_act8846_resume,
};

static __init void regulator_act8846_driver_init(void)
{
	register_driver(&regulator_act8846);
}

static __exit void regulator_act8846_driver_exit(void)
{
	unregister_driver(&regulator_act8846);
}

driver_initcall(regulator_act8846_driver_init);
driver_exitcall(regulator_act8846_driver_exit);
