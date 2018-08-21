/*
 * driver/battery-axp228.c
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
#include <battery/battery.h>

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

struct battery_axp228_pdata_t {
	struct i2c_device_t * dev;
	int design_capacity;
	int design_voltage;
	int charge_current_limit;
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

static bool_t battery_axp228_update(struct battery_t * bat, struct battery_info_t * info)
{
	struct battery_axp228_pdata_t * pdat = (struct battery_axp228_pdata_t *)bat->priv;
	u8_t val, hi, lo;
	u16_t tmp;

	if(!axp228_read(pdat->dev, AXP228_POWER_STATUS, &val))
		return FALSE;
	if(((val >> 6) & 0x3) == 0x3)
		info->supply = POWER_SUPPLAY_AC;
	else if(((val >> 4) & 0x3) == 0x3)
		info->supply = POWER_SUPPLAY_USB;
	else
		info->supply = POWER_SUPPLAY_BATTERY;

	if(!axp228_read(pdat->dev, AXP228_CHARGE_STATUS, &val))
		return FALSE;
	if(val & (0x1 << 6))
		info->status = BATTERY_STATUS_CHARGING;
	else
	{
		if(val & (0x1 << 5))
			info->status = BATTERY_STATUS_FULL;
		else
			info->status = BATTERY_STATUS_DISCHARGING;
	}

	if(val & (0x1 << 7))
		info->health = BATTERY_HEALTH_OVERHEAT;
	else
		info->health = BATTERY_HEALTH_GOOD;

	info->design_capacity = pdat->design_capacity;
	info->design_voltage = pdat->design_voltage;

	if(!axp228_read(pdat->dev, AXP228_VBATH_RES, &hi) ||
		!axp228_read(pdat->dev, AXP228_VBATL_RES, &lo))
		return FALSE;
	tmp = (hi << 8) | lo;
	info->voltage = ((((tmp >> 8) << 4) | (tmp & 0x000F))) * 1100 / 1000;

	if(!axp228_read(pdat->dev, AXP228_DISCHGCURRH_RES, &hi) ||
		!axp228_read(pdat->dev, AXP228_DISCHGCURRL_RES, &lo))
		return FALSE;
	tmp = (hi << 5) | (lo & 0x1f);
	info->current = tmp;

	if(!axp228_read(pdat->dev, AXP228_BATTEMPH_RES, &hi) ||
		!axp228_read(pdat->dev, AXP228_BATTEMPL_RES, &lo))
		return FALSE;
	tmp = (hi << 4) | (lo & 0xf);
	info->temperature = (tmp * 1063 / 10000 - 2667 / 10) * 100;

	info->cycle = 0;

	if(!axp228_read(pdat->dev, AXP228_CAP, &val))
		return FALSE;
	if(val & (0x1 << 7))
		info->level = val & 0x7f;
	else
		info->level = 0;

	return TRUE;
}

static struct device_t * battery_axp228_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct battery_axp228_pdata_t * pdat;
	struct battery_t * bat;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	int charge_current_limit = dt_read_int(n, "charge-current-limit", 1500);
	u8_t val;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), 0x34, 0);
	if(!i2cdev)
		return NULL;

	if(axp228_read(i2cdev, AXP228_CHIP_ID, &val) && (val == 0x06))
	{
		axp228_read(i2cdev, AXP228_CHARGE3, &val);
		val &= ~(0xf << 0);
		val |= ((((charge_current_limit - 300) / 150) & 0xf) << 0);
		axp228_write(i2cdev, AXP228_CHARGE3, val);
	}
	else
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct battery_axp228_pdata_t));
	if(!pdat)
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	bat = malloc(sizeof(struct battery_t));
	if(!bat)
	{
		i2c_device_free(i2cdev);
		free(pdat);
		return NULL;
	}

	pdat->dev = i2cdev;
	pdat->design_capacity = dt_read_int(n, "design-capacity", 5000);
	pdat->design_voltage = dt_read_int(n, "design_voltage", 4200);
	pdat->charge_current_limit = charge_current_limit;

	bat->name = alloc_device_name(dt_read_name(n), -1);
	bat->update = battery_axp228_update;
	bat->priv = pdat;

	if(!register_battery(&dev, bat))
	{
		i2c_device_free(pdat->dev);

		free_device_name(bat->name);
		free(bat->priv);
		free(bat);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void battery_axp228_remove(struct device_t * dev)
{
	struct battery_t * bat = (struct battery_t *)dev->priv;
	struct battery_axp228_pdata_t * pdat = (struct battery_axp228_pdata_t *)bat->priv;

	if(bat && unregister_battery(bat))
	{
		i2c_device_free(pdat->dev);

		free_device_name(bat->name);
		free(bat->priv);
		free(bat);
	}
}

static void battery_axp228_suspend(struct device_t * dev)
{
}

static void battery_axp228_resume(struct device_t * dev)
{
}

static struct driver_t battery_axp228 = {
	.name		= "battery-axp228",
	.probe		= battery_axp228_probe,
	.remove		= battery_axp228_remove,
	.suspend	= battery_axp228_suspend,
	.resume		= battery_axp228_resume,
};

static __init void battery_axp228_driver_init(void)
{
	register_driver(&battery_axp228);
}

static __exit void battery_axp228_driver_exit(void)
{
	unregister_driver(&battery_axp228);
}

driver_initcall(battery_axp228_driver_init);
driver_exitcall(battery_axp228_driver_exit);
