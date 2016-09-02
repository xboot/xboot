/*
 * driver/axp228-pmic.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <axp228-pmic.h>

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

struct axp228_pmic_pdata_t {
	struct i2c_device_t * client;
};

static bool_t axp228_pmic_read(struct i2c_device_t * client, u8_t reg, u8_t * val)
{
	struct i2c_msg_t msgs[2];
    u8_t buf;

    msgs[0].addr = client->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &reg;

    msgs[1].addr = client->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 1;
    msgs[1].buf = &buf;

    if(i2c_transfer(client->i2c, msgs, 2) != 2)
    	return FALSE;

    if(val)
    	*val = buf;
    return TRUE;
}

static bool_t axp228_pmic_write(struct i2c_device_t * client, u8_t reg, u8_t val)
{
	struct i2c_msg_t msg;
	u8_t buf[2];

	buf[0] = reg;
	buf[1] = val;
    msg.addr = client->addr;
    msg.flags = 0;
    msg.len = 2;
    msg.buf = &buf[0];

    if(i2c_transfer(client->i2c, &msg, 1) != 1)
    	return FALSE;
    return TRUE;
}

static u8_t axp228_pmic_get_vol_step(int vol, int step, int min, int max)
{
	u32_t v = 0;

	if(vol < min)
	{
		vol = min;
	}
	else if(vol > max)
	{
		vol = max;
	}

	v = (vol - min + step - 1) / step;
	return (u8_t)(v & 0xff);
}

static void axp228_pmic_init(struct battery_t * bat)
{
}

static void axp228_pmic_exit(struct battery_t * bat)
{

}

static bool_t axp228_pmic_update(struct battery_t * bat, struct battery_info_t * info)
{
	struct axp228_pmic_pdata_t * pdat = (struct axp228_pmic_pdata_t *)bat->priv;
	u8_t val, hi, lo;
	u16_t tmp;

	if(axp228_pmic_read(pdat->client, AXP228_POWER_STATUS, &val))
	{
		if(((val >> 6) & 0x3) == 0x3)
			info->supply = POWER_SUPPLAY_AC;
		else if(((val >> 4) & 0x3) == 0x3)
			info->supply = POWER_SUPPLAY_USB;
		else
			info->supply = POWER_SUPPLAY_BATTERY;
	}

	if(axp228_pmic_read(pdat->client, AXP228_CHARGE_STATUS, &val))
	{
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
	}

	info->count = 0;
	info->capacity = 3600;

	if(axp228_pmic_read(pdat->client, AXP228_VBATH_RES, &hi) &&
		axp228_pmic_read(pdat->client, AXP228_VBATL_RES, &lo))
	{
		tmp = (hi << 8) | lo;
		info->voltage = ((((tmp >> 8) << 4) | (tmp & 0x000F))) * 1100 / 1000;
	}

	if(axp228_pmic_read(pdat->client, AXP228_DISCHGCURRH_RES, &hi) &&
		axp228_pmic_read(pdat->client, AXP228_DISCHGCURRL_RES, &lo))
	{
		tmp = (hi << 5) | (lo & 0x1f);
		info->current = tmp;
	}

	if(axp228_pmic_read(pdat->client, AXP228_BATTEMPH_RES, &hi) &&
		axp228_pmic_read(pdat->client, AXP228_BATTEMPL_RES, &lo))
	{
		tmp = (hi << 4) | (lo & 0xf);
		info->current = tmp * 1063 / 10000 - 2667 / 10;
	}

	if(axp228_pmic_read(pdat->client, AXP228_CAP, &val))
	{
		if(val & (0x1 << 7))
			info->level = val & 0x7f;
		else
			info->level = 0;
	}

	return TRUE;
}

static void axp228_pmic_suspend(struct battery_t * bat)
{
}

static void axp228_pmic_resume(struct battery_t * bat)
{
}

static bool_t register_axp228_pmic(struct resource_t * res)
{
	struct axp228_pmic_data_t * rdat = (struct axp228_pmic_data_t *)res->data;
	struct axp228_pmic_pdata_t * pdat;
	struct battery_t * bat;
	struct i2c_device_t * client;
	char name[64];
	u8_t val = 0;

	client = i2c_device_alloc(rdat->i2cbus, rdat->addr, 0);
	if(!client)
		return FALSE;

	if(!axp228_pmic_read(client, AXP228_CHIP_ID, &val) || (val != 0x06))
	{
		i2c_device_free(client);
		return FALSE;
	}

	/* DCDC1 - 1.6V ~ 3.4V, 100mV/step, 1.4A */
	if(rdat->dcdc1 > 0)
	{
		val = axp228_pmic_get_vol_step(rdat->dcdc1, 100, 1600, 3400);
		axp228_pmic_write(client, AXP228_DC1OUT_VOL, val);

		axp228_pmic_read(client, AXP228_LDO_DC_EN1, &val);
		val |= 0x1 << 1;
		axp228_pmic_write(client, AXP228_LDO_DC_EN1, val);
	}
	else
	{
		axp228_pmic_read(client, AXP228_LDO_DC_EN1, &val);
		val &= ~(0x1 << 1);
		axp228_pmic_write(client, AXP228_LDO_DC_EN1, val);
	}

	/* DCDC2 - 0.6V ~ 1.54V, 20mV/step, 2.5A */
	if(rdat->dcdc2 > 0)
	{
		val = axp228_pmic_get_vol_step(rdat->dcdc2, 20, 600, 1540);
		axp228_pmic_write(client, AXP228_DC2OUT_VOL, val);

		axp228_pmic_read(client, AXP228_LDO_DC_EN1, &val);
		val |= 0x1 << 2;
		axp228_pmic_write(client, AXP228_LDO_DC_EN1, val);
	}
	else
	{
		axp228_pmic_read(client, AXP228_LDO_DC_EN1, &val);
		val &= ~(0x1 << 2);
		axp228_pmic_write(client, AXP228_LDO_DC_EN1, val);
	}

	/* DCDC3 - 0.6V ~ 1.86V, 20mV/step, 2.5A */
	if(rdat->dcdc3 > 0)
	{
		val = axp228_pmic_get_vol_step(rdat->dcdc3, 20, 600, 1860);
		axp228_pmic_write(client, AXP228_DC3OUT_VOL, val);

		axp228_pmic_read(client, AXP228_LDO_DC_EN1, &val);
		val |= 0x1 << 3;
		axp228_pmic_write(client, AXP228_LDO_DC_EN1, val);
	}
	else
	{
		axp228_pmic_read(client, AXP228_LDO_DC_EN1, &val);
		val &= ~(0x1 << 3);
		axp228_pmic_write(client, AXP228_LDO_DC_EN1, val);
	}

	/* DCDC4 - 0.6V ~ 1.54V, 20mV/step, 0.6A */
	if(rdat->dcdc4 > 0)
	{
		val = axp228_pmic_get_vol_step(rdat->dcdc4, 20, 600, 1540);
		axp228_pmic_write(client, AXP228_DC4OUT_VOL, val);

		axp228_pmic_read(client, AXP228_LDO_DC_EN1, &val);
		val |= 0x1 << 4;
		axp228_pmic_write(client, AXP228_LDO_DC_EN1, val);
	}
	else
	{
		axp228_pmic_read(client, AXP228_LDO_DC_EN1, &val);
		val &= ~(0x1 << 4);
		axp228_pmic_write(client, AXP228_LDO_DC_EN1, val);
	}

	/* DCDC5 - 1.0V ~ 2.55V, 50mV/step, 2A */
	if(rdat->dcdc5 > 0)
	{
		val = axp228_pmic_get_vol_step(rdat->dcdc5, 50, 1000, 2550);
		axp228_pmic_write(client, AXP228_DC5OUT_VOL, val);

		axp228_pmic_read(client, AXP228_LDO_DC_EN1, &val);
		val |= 0x1 << 5;
		axp228_pmic_write(client, AXP228_LDO_DC_EN1, val);
	}
	else
	{
		axp228_pmic_read(client, AXP228_LDO_DC_EN1, &val);
		val &= ~(0x1 << 5);
		axp228_pmic_write(client, AXP228_LDO_DC_EN1, val);
	}

	/* DC5LDO - 0.7V ~ 1.4V, 100mV/step, 200mA */
	if(rdat->dc5ldo > 0)
	{
		val = axp228_pmic_get_vol_step(rdat->dc5ldo, 100, 700, 1400);
		axp228_pmic_write(client, AXP228_DC5LDOOUT_VOL, val);

		axp228_pmic_read(client, AXP228_LDO_DC_EN1, &val);
		val |= 0x1 << 0;
		axp228_pmic_write(client, AXP228_LDO_DC_EN1, val);
	}
	else
	{
		axp228_pmic_read(client, AXP228_LDO_DC_EN1, &val);
		val &= ~(0x1 << 0);
		axp228_pmic_write(client, AXP228_LDO_DC_EN1, val);
	}

	/* ALDO1 - 0.7V ~ 3.3V, 100mV/step, 300mA */
	if(rdat->aldo1 > 0)
	{
		val = axp228_pmic_get_vol_step(rdat->aldo1, 100, 700, 3300);
		axp228_pmic_write(client, AXP228_ALDO1OUT_VOL, val);

		axp228_pmic_read(client, AXP228_LDO_DC_EN1, &val);
		val |= 0x1 << 6;
		axp228_pmic_write(client, AXP228_LDO_DC_EN1, val);
	}
	else
	{
		axp228_pmic_read(client, AXP228_LDO_DC_EN1, &val);
		val &= ~(0x1 << 6);
		axp228_pmic_write(client, AXP228_LDO_DC_EN1, val);
	}

	/* ALDO2 - 0.7V ~ 3.3V, 100mV/step, 300mA */
	if(rdat->aldo2 > 0)
	{
		val = axp228_pmic_get_vol_step(rdat->aldo2, 100, 700, 3300);
		axp228_pmic_write(client, AXP228_ALDO2OUT_VOL, val);

		axp228_pmic_read(client, AXP228_LDO_DC_EN1, &val);
		val |= 0x1 << 7;
		axp228_pmic_write(client, AXP228_LDO_DC_EN1, val);
	}
	else
	{
		axp228_pmic_read(client, AXP228_LDO_DC_EN1, &val);
		val &= ~(0x1 << 7);
		axp228_pmic_write(client, AXP228_LDO_DC_EN1, val);
	}

	/* ALDO3 - 0.7V ~ 3.3V, 100mV/step, 200mA */
	if(rdat->aldo3 > 0)
	{
		val = axp228_pmic_get_vol_step(rdat->aldo3, 100, 700, 3300);
		axp228_pmic_write(client, AXP228_ALDO3OUT_VOL, val);

		axp228_pmic_read(client, AXP228_LDO_DC_EN3, &val);
		val |= 0x1 << 7;
		axp228_pmic_write(client, AXP228_LDO_DC_EN3, val);
	}
	else
	{
		axp228_pmic_read(client, AXP228_LDO_DC_EN3, &val);
		val &= ~(0x1 << 7);
		axp228_pmic_write(client, AXP228_LDO_DC_EN3, val);
	}

	/* DLDO1 - 0.7V ~ 3.3V, 100mV/step, 400mA */
	if(rdat->dldo1 > 0)
	{
		val = axp228_pmic_get_vol_step(rdat->dldo1, 100, 700, 3300);
		axp228_pmic_write(client, AXP228_DLDO1OUT_VOL, val);

		axp228_pmic_read(client, AXP228_LDO_DC_EN2, &val);
		val |= 0x1 << 3;
		axp228_pmic_write(client, AXP228_LDO_DC_EN2, val);
	}
	else
	{
		axp228_pmic_read(client, AXP228_LDO_DC_EN2, &val);
		val &= ~(0x1 << 3);
		axp228_pmic_write(client, AXP228_LDO_DC_EN2, val);
	}

	/* DLDO2 - 0.7V ~ 3.3V, 100mV/step, 200mA */
	if(rdat->dldo2 > 0)
	{
		val = axp228_pmic_get_vol_step(rdat->dldo2, 100, 700, 3300);
		axp228_pmic_write(client, AXP228_DLDO2OUT_VOL, val);

		axp228_pmic_read(client, AXP228_LDO_DC_EN2, &val);
		val |= 0x1 << 4;
		axp228_pmic_write(client, AXP228_LDO_DC_EN2, val);
	}
	else
	{
		axp228_pmic_read(client, AXP228_LDO_DC_EN2, &val);
		val &= ~(0x1 << 4);
		axp228_pmic_write(client, AXP228_LDO_DC_EN2, val);
	}

	/* DLDO3 - 0.7V ~ 3.3V, 100mV/step, 200mA */
	if(rdat->dldo3 > 0)
	{
		val = axp228_pmic_get_vol_step(rdat->dldo3, 100, 700, 3300);
		axp228_pmic_write(client, AXP228_DLDO3OUT_VOL, val);

		axp228_pmic_read(client, AXP228_LDO_DC_EN2, &val);
		val |= 0x1 << 5;
		axp228_pmic_write(client, AXP228_LDO_DC_EN2, val);
	}
	else
	{
		axp228_pmic_read(client, AXP228_LDO_DC_EN2, &val);
		val &= ~(0x1 << 5);
		axp228_pmic_write(client, AXP228_LDO_DC_EN2, val);
	}

	/* DLDO4 - 0.7V ~ 3.3V, 100mV/step, 100mA */
	if(rdat->dldo4 > 0)
	{
		val = axp228_pmic_get_vol_step(rdat->dldo4, 100, 700, 3300);
		axp228_pmic_write(client, AXP228_DLDO4OUT_VOL, val);

		axp228_pmic_read(client, AXP228_LDO_DC_EN2, &val);
		val |= 0x1 << 6;
		axp228_pmic_write(client, AXP228_LDO_DC_EN2, val);
	}
	else
	{
		axp228_pmic_read(client, AXP228_LDO_DC_EN2, &val);
		val &= ~(0x1 << 6);
		axp228_pmic_write(client, AXP228_LDO_DC_EN2, val);
	}

	/* ELDO1 - 0.7V ~ 3.3V, 100mV/step, 400mA */
	if(rdat->eldo1 > 0)
	{
		val = axp228_pmic_get_vol_step(rdat->eldo1, 100, 700, 3300);
		axp228_pmic_write(client, AXP228_ELDO1OUT_VOL, val);

		axp228_pmic_read(client, AXP228_LDO_DC_EN2, &val);
		val |= 0x1 << 0;
		axp228_pmic_write(client, AXP228_LDO_DC_EN2, val);
	}
	else
	{
		axp228_pmic_read(client, AXP228_LDO_DC_EN2, &val);
		val &= ~(0x1 << 0);
		axp228_pmic_write(client, AXP228_LDO_DC_EN2, val);
	}

	/* ELDO2 - 0.7V ~ 3.3V, 100mV/step, 200mA */
	if(rdat->eldo2 > 0)
	{
		val = axp228_pmic_get_vol_step(rdat->eldo2, 100, 700, 3300);
		axp228_pmic_write(client, AXP228_ELDO2OUT_VOL, val);

		axp228_pmic_read(client, AXP228_LDO_DC_EN2, &val);
		val |= 0x1 << 1;
		axp228_pmic_write(client, AXP228_LDO_DC_EN2, val);
	}
	else
	{
		axp228_pmic_read(client, AXP228_LDO_DC_EN2, &val);
		val &= ~(0x1 << 1);
		axp228_pmic_write(client, AXP228_LDO_DC_EN2, val);
	}

	/* ELDO3 - 0.7V ~ 3.3V, 100mV/step, 200mA */
	if(rdat->eldo3 > 0)
	{
		val = axp228_pmic_get_vol_step(rdat->eldo3, 100, 700, 3300);
		axp228_pmic_write(client, AXP228_ELDO3OUT_VOL, val);

		axp228_pmic_read(client, AXP228_LDO_DC_EN2, &val);
		val |= 0x1 << 2;
		axp228_pmic_write(client, AXP228_LDO_DC_EN2, val);
	}
	else
	{
		axp228_pmic_read(client, AXP228_LDO_DC_EN2, &val);
		val &= ~(0x1 << 2);
		axp228_pmic_write(client, AXP228_LDO_DC_EN2, val);
	}

	pdat = malloc(sizeof(struct axp228_pmic_pdata_t));
	if(!pdat)
	{
		i2c_device_free(client);
		return FALSE;
	}

	bat = malloc(sizeof(struct battery_t));
	if(!bat)
	{
		i2c_device_free(client);
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	memset(pdat, 0xff, sizeof(struct axp228_pmic_pdata_t));
	pdat->client = client;

	bat->name = strdup(name);
	bat->init = axp228_pmic_init;
	bat->exit = axp228_pmic_exit;
	bat->update = axp228_pmic_update;
	bat->suspend = axp228_pmic_suspend;
	bat->resume = axp228_pmic_resume;
	bat->priv = pdat;

	if(register_battery(bat))
		return TRUE;

	i2c_device_free(client);
	free(bat->priv);
	free(bat->name);
	free(bat);
	return FALSE;
}

static bool_t unregister_axp228_pmic(struct resource_t * res)
{
	struct battery_t * bat;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	bat = search_battery(name);
	if(!bat)
		return FALSE;

	if(!unregister_battery(bat))
		return FALSE;

	i2c_device_free(((struct axp228_pmic_pdata_t *)(bat->priv))->client);
	free(bat->priv);
	free(bat->name);
	free(bat);
	return TRUE;
}

static __init void axp228_pmic_device_init(void)
{
	resource_for_each("axp228-pmic", register_axp228_pmic);
}

static __exit void axp228_pmic_device_exit(void)
{
	resource_for_each("axp228-pmic", unregister_axp228_pmic);
}

device_initcall(axp228_pmic_device_init);
device_exitcall(axp228_pmic_device_exit);
