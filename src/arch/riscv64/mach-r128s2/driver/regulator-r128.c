/*
 * driver/regulator-r128.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
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
#include <r128/reg-gprcm.h>
#include <regulator/regulator.h>

struct regulator_r128_pdata_t {
	virtual_addr_t virt;
	int channel;
};

static void regulator_r128_set_parent(struct regulator_t * supply, const char * pname)
{
}

static const char * regulator_r128_get_parent(struct regulator_t * supply)
{
	return NULL;
}

static void regulator_r128_set_enable(struct regulator_t * supply, bool_t enable)
{
	struct regulator_r128_pdata_t * pdat = (struct regulator_r128_pdata_t *)supply->priv;
	u32_t val;

	switch(pdat->channel)
	{
	case 0 ... 1:
		break;

	case 2:
		val = read32(pdat->virt + EXT_LDO_CTRL);
		if(enable)
			val |= (0x3 << 0);
		else
			val &= ~(0x3 << 0);
		write32(pdat->virt + EXT_LDO_CTRL, val);
		break;

	case 3:
		val = read32(pdat->virt + AON_LDO_CTRL);
		if(enable)
			val |= (0x1 << 0);
		else
			val &= ~(0x1 << 0);
		write32(pdat->virt + AON_LDO_CTRL, val);
		break;

	case 4:
		val = read32(pdat->virt + APP_LDO_CTRL);
		if(enable)
			val |= (0x1 << 0);
		else
			val &= ~(0x1 << 0);
		write32(pdat->virt + APP_LDO_CTRL, val);
		break;

	case 5:
		val = read32(pdat->virt + DSP_LDO_CTRL);
		if(enable)
			val |= (0x1 << 0);
		else
			val &= ~(0x1 << 0);
		write32(pdat->virt + DSP_LDO_CTRL, val);
		break;

	default:
		break;
	}
}

static bool_t regulator_r128_get_enable(struct regulator_t * supply)
{
	struct regulator_r128_pdata_t * pdat = (struct regulator_r128_pdata_t *)supply->priv;

	switch(pdat->channel)
	{
	case 0 ... 1:
		return TRUE;

	case 2:
		return (read32(pdat->virt + EXT_LDO_CTRL) & (0x1 << 1)) ? TRUE : FALSE;

	case 3:
		return (read32(pdat->virt + AON_LDO_CTRL) & (0x1 << 0)) ? TRUE : FALSE;

	case 4:
		return (read32(pdat->virt + APP_LDO_CTRL) & (0x1 << 0)) ? TRUE : FALSE;

	case 5:
		return (read32(pdat->virt + DSP_LDO_CTRL) & (0x1 << 0)) ? TRUE : FALSE;

	default:
		break;
	}
	return FALSE;
}

static u8_t r128_vol_to_reg(int vol, int step, int min, int max)
{
	int reg;

	if(vol < min)
		vol = min;
	else if(vol > max)
		vol = max;

	reg = (vol - min + step - 1) / step;
	return (u8_t)(reg & 0xff);
}

static int r128_reg_to_vol(u8_t reg, int step, int min, int max)
{
	int vol = (int)reg * step + min;

	if(vol < min)
		vol = min;
	else if(vol > max)
		vol = max;
	return vol;
}

static const int rtc_ldo_table[] = {
	1075000, 975000, 875000, 775000, 675000, 575000, 1175000, 1275000,
};

static const int ext_ldo_table[] = {
	3300000, 3100000, 2800000, 2500000,
};

static u8_t r128_table_vol_to_reg(const int * tab, int tablen, int vol)
{
	int minv = INT_MAX;
	int best = 0;

	for(int i = 0; i < tablen; i++)
	{
		int v = abs(vol - tab[i]);
		if(v <= minv)
		{
			minv = v;
			best = i;
		}
	}
	return best;
}

static int r128_table_reg_to_vol(const int * tab, int tablen, u8_t reg)
{
	if(reg >= tablen)
		reg = tablen - 1;
	return tab[reg];
}

static void regulator_r128_set_voltage(struct regulator_t * supply, int voltage)
{
	struct regulator_r128_pdata_t * pdat = (struct regulator_r128_pdata_t *)supply->priv;
	u32_t val;
	u8_t v;

	switch(pdat->channel)
	{
	/* DCDC - 1.0V ~ 2.5V, 0.025V/step, 0.9A */
	case 0:
		v = r128_vol_to_reg(voltage, 25000,  1000000, 2500000);
		val = read32(pdat->virt + DCDC_CTRL0);
		val &= ~(0x3f << 12);
		val |= (v & 0x3f) << 12;
		write32(pdat->virt + DCDC_CTRL0, val);
		break;

	/* RTCLDO - 0.6V ~ 1.3V, 0.1V/step, 0.01A */
	case 1:
		v = r128_table_vol_to_reg(rtc_ldo_table, ARRAY_SIZE(rtc_ldo_table), voltage);
		val = read32(pdat->virt + RTC_LDO_CTRL);
		val &= ~(0x7 << 0);
		val |= (v & 0x7) << 0;
		write32(pdat->virt + RTC_LDO_CTRL, val);
		break;

	/* EXTLDO - 2.5V ~ 3.3V, 0.3V/step, 0.8A */
	case 2:
		v = r128_table_vol_to_reg(ext_ldo_table, ARRAY_SIZE(ext_ldo_table), voltage);
		val = read32(pdat->virt + EXT_LDO_CTRL);
		val &= ~(0x3 << 4);
		val |= (v & 0x3) << 4;
		write32(pdat->virt + EXT_LDO_CTRL, val);
		break;

	/* AONLDO - 0.6V ~ 1.375V, 0.025V/step, 0.25A */
	case 3:
		v = r128_vol_to_reg(voltage, 25000,  600000, 1350000);
		val = read32(pdat->virt + AON_LDO_CTRL);
		val &= ~(0x1f << 9);
		val |= (v & 0x1f) << 9;
		write32(pdat->virt + AON_LDO_CTRL, val);
		break;

	/* APPLDO - 0.6V ~ 1.375V, 0.025V/step, 0.25A */
	case 4:
		v = r128_vol_to_reg(voltage, 25000,  600000, 1350000);
		val = read32(pdat->virt + APP_LDO_CTRL);
		val &= ~(0x1f << 9);
		val |= (v & 0x1f) << 9;
		write32(pdat->virt + APP_LDO_CTRL, val);
		break;

	/* DSPLDO - 0.6V ~ 1.375V, 0.025V/step, 0.25A */
	case 5:
		v = r128_vol_to_reg(voltage, 25000,  600000, 1350000);
		val = read32(pdat->virt + DSP_LDO_CTRL);
		val &= ~(0x1f << 4);
		val |= (v & 0x1f) << 4;
		write32(pdat->virt + DSP_LDO_CTRL, val);
		break;

	default:
		break;
	}
}

static int regulator_r128_get_voltage(struct regulator_t * supply)
{
	struct regulator_r128_pdata_t * pdat = (struct regulator_r128_pdata_t *)supply->priv;
	int voltage;
	u8_t v;

	switch(pdat->channel)
	{
	/* DCDC - 1.0V ~ 2.5V, 0.025V/step, 0.9A */
	case 0:
		v = (read32(pdat->virt + DCDC_CTRL0) >> 12) & 0x3f;
		voltage = r128_reg_to_vol(v, 25000, 1000000, 2500000);
		break;

	/* RTCLDO - 0.6V ~ 1.3V, 0.1V/step, 0.01A */
	case 1:
		v = (read32(pdat->virt + RTC_LDO_CTRL) >> 0) & 0x7;
		voltage = r128_table_reg_to_vol(rtc_ldo_table, ARRAY_SIZE(rtc_ldo_table), v);
		break;

	/* EXTLDO - 2.5V ~ 3.3V, 0.3V/step, 0.8A */
	case 2:
		v = (read32(pdat->virt + EXT_LDO_CTRL) >> 4) & 0x3;
		voltage = r128_table_reg_to_vol(ext_ldo_table, ARRAY_SIZE(ext_ldo_table), v);
		break;

	/* AONLDO - 0.6V ~ 1.375V, 0.025V/step, 0.25A */
	case 3:
		v = (read32(pdat->virt + AON_LDO_CTRL) >> 9) & 0x1f;
		voltage = r128_reg_to_vol(v, 25000, 600000, 1350000);
		break;

	/* APPLDO - 0.6V ~ 1.375V, 0.025V/step, 0.25A */
	case 4:
		v = (read32(pdat->virt + APP_LDO_CTRL) >> 9) & 0x1f;
		voltage = r128_reg_to_vol(v, 25000, 600000, 1350000);
		break;

	/* DSPLDO - 0.6V ~ 1.375V, 0.025V/step, 0.25A */
	case 5:
		v = (read32(pdat->virt + DSP_LDO_CTRL) >> 4) & 0x1f;
		voltage = r128_reg_to_vol(v, 25000, 600000, 1350000);
		break;

	default:
		voltage = 0;
		break;
	}
	return voltage;
}

static struct device_t * regulator_r128_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct regulator_r128_pdata_t * pdat;
	struct regulator_t * supply;
	struct device_t * dev;
	struct dtnode_t o;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * name = dt_read_string(n, "name", NULL);
	int channel = dt_read_int(n, "channel", -1);

	if(!name || search_regulator(name))
		return NULL;

	if(channel < 0 || channel > 5)
		return NULL;

	pdat = malloc(sizeof(struct regulator_r128_pdata_t));
	if(!pdat)
		return NULL;

	supply = malloc(sizeof(struct regulator_t));
	if(!supply)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->channel = channel;

	supply->name = strdup(name);
	supply->count = 0;
	supply->set_parent = regulator_r128_set_parent;
	supply->get_parent = regulator_r128_get_parent;
	supply->set_enable = regulator_r128_set_enable;
	supply->get_enable = regulator_r128_get_enable;
	supply->set_voltage = regulator_r128_set_voltage;
	supply->get_voltage = regulator_r128_get_voltage;
	supply->priv = pdat;

	if(!(dev = register_regulator(supply, drv)))
	{
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

static void regulator_r128_remove(struct device_t * dev)
{
	struct regulator_t * supply = (struct regulator_t *)dev->priv;

	if(supply)
	{
		unregister_regulator(supply);
		free(supply->name);
		free(supply->priv);
		free(supply);
	}
}

static void regulator_r128_suspend(struct device_t * dev)
{
}

static void regulator_r128_resume(struct device_t * dev)
{
}

static struct driver_t regulator_r128 = {
	.name		= "regulator-r128",
	.probe		= regulator_r128_probe,
	.remove		= regulator_r128_remove,
	.suspend	= regulator_r128_suspend,
	.resume		= regulator_r128_resume,
};

static __init void regulator_r128_driver_init(void)
{
	register_driver(&regulator_r128);
}

static __exit void regulator_r128_driver_exit(void)
{
	unregister_driver(&regulator_r128);
}

driver_initcall(regulator_r128_driver_init);
driver_exitcall(regulator_r128_driver_exit);
