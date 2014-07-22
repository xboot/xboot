/*
 * s5pv210-gpio.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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

#include <xboot.h>
#include <s5pv210/reg-gpio.h>

struct s5pv210_gpio_data_t
{
	physical_addr_t regbase;
};

void s5pv210_gpio_cfg_pin(struct gpio_t * gpio, int offset, int cfg)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return;

	offset <<= 0x2;
	val = readl(dat->regbase + S5PV210_GPIO_CON);
	val &= ~(0xf << offset);
	val |= cfg << offset;
	writel(dat->regbase + S5PV210_GPIO_CON, val);
}

static void s5pv210_gpio_set_pull(struct gpio_t * gpio, int offset, enum gpio_pull_t pull)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val, p;

	if(offset >= gpio->ngpio)
		return;

	switch(pull)
	{
	case GPIO_PULL_UP:
		p = 0x2;
		break;
	case GPIO_PULL_DOWN:
		p = 0x1;
		break;
	case GPIO_PULL_NONE:
		p = 0x0;
		break;
	default:
		p = 0x0;
		break;
	}

	offset <<= 0x1;
	val = readl(dat->regbase + S5PV210_GPIO_PUD);
	val &= ~(0x3 << offset);
	val |= p << offset;
	writel(dat->regbase + S5PV210_GPIO_PUD, val);
}

void s5pv210_gpio_set_drv(struct gpio_t * gpio, int offset, enum gpio_drv_t drv)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val, d;

	if(offset >= gpio->ngpio)
		return;

	switch(drv)
	{
	case GPIO_DRV_LOW:
		d = 0x0;
		break;
	case GPIO_DRV_MEDIAN:
		d = 0x2;
		break;
	case GPIO_DRV_HIGH:
		d = 0x3;
		break;
	default:
		d = 0x0;
		break;
	}

	offset <<= 0x1;
	val = readl(dat->regbase + S5PV210_GPIO_DRV);
	val &= ~(0x3 << offset);
	val |= d << offset;
	writel(dat->regbase + S5PV210_GPIO_DRV, val);
}

void s5pv210_gpio_set_rate(struct gpio_t * gpio, int offset, enum gpio_rate_t rate)
{
}

static void s5pv210_gpio_set_value(struct gpio_t * gpio, int offset, int value)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return;

	val = readl(dat->regbase + S5PV210_GPIO_DAT);
	val &= ~(1 << offset);
	val |= (!!value) << offset;
	writel(dat->regbase + S5PV210_GPIO_DAT, val);
}

static int s5pv210_gpio_get_value(struct gpio_t * gpio, int offset)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return 0;

	val = readl(dat->regbase + S5PV210_GPIO_DAT);
	return !!(val & (1 << offset));
}

static void s5pv210_gpio_direction_output(struct gpio_t * gpio, int offset, int value)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return;

	s5pv210_gpio_set_value(gpio, offset, value);

	offset <<= 0x2;
	val = readl(dat->regbase + S5PV210_GPIO_CON);
	val &= ~(0xf << offset);
	val |= 0x1 << offset;
	writel(dat->regbase + S5PV210_GPIO_CON, val);
}

static void s5pv210_gpio_direction_input(struct gpio_t * gpio, int offset)
{
	struct s5pv210_gpio_data_t * dat = (struct s5pv210_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return;

	offset <<= 0x2;
	val = readl(dat->regbase + S5PV210_GPIO_CON);
	val &= ~(0xf << offset);
	writel(dat->regbase + S5PV210_GPIO_CON, val);
}

static struct s5pv210_gpio_data_t s5pv210_gpio_datas[] = {
	{
		.regbase	= S5PV210_GPA0_BASE,
	}, {
		.regbase	= S5PV210_GPA1_BASE,
	}, {
		.regbase	= S5PV210_GPB_BASE,
	}, {
		.regbase	= S5PV210_GPC0_BASE,
	}, {
		.regbase	= S5PV210_GPC1_BASE,
	}, {
		.regbase	= S5PV210_GPD0_BASE,
	}, {
		.regbase	= S5PV210_GPD1_BASE,
	}, {
		.regbase	= S5PV210_GPE0_BASE,
	}, {
		.regbase	= S5PV210_GPE1_BASE,
	}, {
		.regbase	= S5PV210_GPF0_BASE,
	}, {
		.regbase	= S5PV210_GPF1_BASE,
	}, {
		.regbase	= S5PV210_GPF2_BASE,
	}, {
		.regbase	= S5PV210_GPF3_BASE,
	}, {
		.regbase	= S5PV210_GPG0_BASE,
	}, {
		.regbase	= S5PV210_GPG1_BASE,
	}, {
		.regbase	= S5PV210_GPG2_BASE,
	}, {
		.regbase	= S5PV210_GPG3_BASE,
	}, {
		.regbase	= S5PV210_GPH0_BASE,
	}, {
		.regbase	= S5PV210_GPH1_BASE,
	}, {
		.regbase	= S5PV210_GPH2_BASE,
	}, {
		.regbase	= S5PV210_GPH3_BASE,
	}, {
		.regbase	= S5PV210_GPI_BASE,
	}, {
		.regbase	= S5PV210_GPJ0_BASE,
	}, {
		.regbase	= S5PV210_GPJ1_BASE,
	}, {
		.regbase	= S5PV210_GPJ2_BASE,
	}, {
		.regbase	= S5PV210_GPJ3_BASE,
	}, {
		.regbase	= S5PV210_GPJ4_BASE,
	}, {
		.regbase	= S5PV210_GPMP01_BASE,
	}, {
		.regbase	= S5PV210_GPMP02_BASE,
	}, {
		.regbase	= S5PV210_GPMP03_BASE,
	}, {
		.regbase	= S5PV210_GPMP04_BASE,
	}, {
		.regbase	= S5PV210_GPMP05_BASE,
	}, {
		.regbase	= S5PV210_GPMP06_BASE,
	}, {
		.regbase	= S5PV210_GPMP07_BASE,
	}
};

static struct gpio_t s5pv210_gpios[] = {
	{
		.name				= "GPA0",
		.base				= 0,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[0],
	}, {
		.name				= "GPA1",
		.base				= 8,
		.ngpio				= 4,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[1],
	}, {
		.name				= "GPB",
		.base				= 16,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[2],
	}, {
		.name				= "GPC0",
		.base				= 24,
		.ngpio				= 5,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[3],
	}, {
		.name				= "GPC1",
		.base				= 32,
		.ngpio				= 5,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[4],
	}, {
		.name				= "GPD0",
		.base				= 40,
		.ngpio				= 4,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[5],
	}, {
		.name				= "GPD1",
		.base				= 48,
		.ngpio				= 6,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[6],
	}, {
		.name				= "GPE0",
		.base				= 56,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[7],
	}, {
		.name				= "GPE1",
		.base				= 64,
		.ngpio				= 5,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[8],
	}, {
		.name				= "GPF0",
		.base				= 72,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[9],
	}, {
		.name				= "GPF1",
		.base				= 80,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[10],
	}, {
		.name				= "GPF2",
		.base				= 88,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[11],
	}, {
		.name				= "GPF3",
		.base				= 96,
		.ngpio				= 6,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[12],
	}, {
		.name				= "GPG0",
		.base				= 104,
		.ngpio				= 7,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[13],
	}, {
		.name				= "GPG1",
		.base				= 112,
		.ngpio				= 7,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[14],
	}, {
		.name				= "GPG2",
		.base				= 120,
		.ngpio				= 7,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[15],
	}, {
		.name				= "GPG3",
		.base				= 128,
		.ngpio				= 7,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[16],
	}, {
		.name				= "GPH0",
		.base				= 136,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[17],
	}, {
		.name				= "GPH1",
		.base				= 144,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[18],
	}, {
		.name				= "GPH2",
		.base				= 152,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[19],
	}, {
		.name				= "GPH3",
		.base				= 160,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[20],
	}, {
		.name				= "GPI",
		.base				= 168,
		.ngpio				= 7,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[21],
	}, {
		.name				= "GPJ0",
		.base				= 176,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[22],
	}, {
		.name				= "GPJ1",
		.base				= 184,
		.ngpio				= 6,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[23],
	}, {
		.name				= "GPJ2",
		.base				= 192,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[24],
	}, {
		.name				= "GPJ3",
		.base				= 200,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[25],
	}, {
		.name				= "GPJ4",
		.base				= 208,
		.ngpio				= 5,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[26],
	}, {
		.name				= "GPMP01",
		.base				= 216,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[27],
	}, {
		.name				= "GPMP02",
		.base				= 224,
		.ngpio				= 4,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[28],
	}, {
		.name				= "GPMP03",
		.base				= 232,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[29],
	}, {
		.name				= "GPMP04",
		.base				= 240,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[30],
	}, {
		.name				= "GPMP05",
		.base				= 248,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[31],
	}, {
		.name				= "GPMP06",
		.base				= 256,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[32],
	}, {
		.name				= "GPMP07",
		.base				= 264,
		.ngpio				= 8,
		.cfg_pin			= s5pv210_gpio_cfg_pin,
		.set_pull			= s5pv210_gpio_set_pull,
		.set_drv			= s5pv210_gpio_set_drv,
		.set_rate			= s5pv210_gpio_set_rate,
		.direction_output	= s5pv210_gpio_direction_output,
		.direction_input	= s5pv210_gpio_direction_input,
		.set_value			= s5pv210_gpio_set_value,
		.get_value			= s5pv210_gpio_get_value,
		.priv				= &s5pv210_gpio_datas[33],
	}
};

static __init void s5pv210_gpio_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(s5pv210_gpios); i++)
	{
		if(register_gpio(&s5pv210_gpios[i]))
			LOG("Register gpio '%s'", s5pv210_gpios[i].name);
		else
			LOG("Failed to register gpio '%s'", s5pv210_gpios[i].name);
	}
}

static __exit void s5pv210_gpio_exit(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(s5pv210_gpios); i++)
	{
		if(register_gpio(&s5pv210_gpios[i]))
			LOG("Unregister gpio '%s'", s5pv210_gpios[i].name);
		else
			LOG("Failed to unregister gpio '%s'", s5pv210_gpios[i].name);
	}
}

postcore_initcall(s5pv210_gpio_init);
postcore_exitcall(s5pv210_gpio_exit);
