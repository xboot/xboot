/*
 * exynos4412-gpio.c
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
#include <exynos4412/reg-gpio.h>

struct exynos4412_gpio_data_t
{
	physical_addr_t regbase;
};

void exynos4412_gpio_cfg_pin(struct gpio_t * gpio, int offset, int cfg)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return;

	offset <<= 0x2;
	val = readl(dat->regbase + EXYNOS4412_GPIO_CON);
	val &= ~(0xf << offset);
	val |= cfg << offset;
	writel(dat->regbase + EXYNOS4412_GPIO_CON, val);
}

static void exynos4412_gpio_set_pull(struct gpio_t * gpio, int offset, enum gpio_pull_t pull)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
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
	val = readl(dat->regbase + EXYNOS4412_GPIO_PUD);
	val &= ~(0x3 << offset);
	val |= p << offset;
	writel(dat->regbase + EXYNOS4412_GPIO_PUD, val);
}

void exynos4412_gpio_set_drv(struct gpio_t * gpio, int offset, enum gpio_drv_t drv)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
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
	val = readl(dat->regbase + EXYNOS4412_GPIO_DRV);
	val &= ~(0x3 << offset);
	val |= d << offset;
	writel(dat->regbase + EXYNOS4412_GPIO_DRV, val);
}

void exynos4412_gpio_set_rate(struct gpio_t * gpio, int offset, enum gpio_rate_t rate)
{
}

static void exynos4412_gpio_set_value(struct gpio_t * gpio, int offset, int value)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return;

	val = readl(dat->regbase + EXYNOS4412_GPIO_DAT);
	val &= ~(1 << offset);
	val |= (!!value) << offset;
	writel(dat->regbase + EXYNOS4412_GPIO_DAT, val);
}

static int exynos4412_gpio_get_value(struct gpio_t * gpio, int offset)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return 0;

	val = readl(dat->regbase + EXYNOS4412_GPIO_DAT);
	return !!(val & (1 << offset));
}

static void exynos4412_gpio_direction_output(struct gpio_t * gpio, int offset, int value)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return;

	exynos4412_gpio_set_value(gpio, offset, value);

	offset <<= 0x2;
	val = readl(dat->regbase + EXYNOS4412_GPIO_CON);
	val &= ~(0xf << offset);
	val |= 0x1 << offset;
	writel(dat->regbase + EXYNOS4412_GPIO_CON, val);
}

static void exynos4412_gpio_direction_input(struct gpio_t * gpio, int offset)
{
	struct exynos4412_gpio_data_t * dat = (struct exynos4412_gpio_data_t *)gpio->priv;
	u32_t val;

	if(offset >= gpio->ngpio)
		return;

	offset <<= 0x2;
	val = readl(dat->regbase + EXYNOS4412_GPIO_CON);
	val &= ~(0xf << offset);
	writel(dat->regbase + EXYNOS4412_GPIO_CON, val);
}

static struct exynos4412_gpio_data_t exynos4412_gpio_datas[] = {
	{
		.regbase	= EXYNOS4412_GPA0_BASE,
	}, {
		.regbase	= EXYNOS4412_GPA1_BASE,
	}, {
		.regbase	= EXYNOS4412_GPB_BASE,
	}, {
		.regbase	= EXYNOS4412_GPC0_BASE,
	}, {
		.regbase	= EXYNOS4412_GPC1_BASE,
	}, {
		.regbase	= EXYNOS4412_GPD0_BASE,
	}, {
		.regbase	= EXYNOS4412_GPD1_BASE,
	}, {
		.regbase	= EXYNOS4412_GPF0_BASE,
	}, {
		.regbase	= EXYNOS4412_GPF1_BASE,
	}, {
		.regbase	= EXYNOS4412_GPF2_BASE,
	}, {
		.regbase	= EXYNOS4412_GPF3_BASE,
	}, {
		.regbase	= EXYNOS4412_GPJ0_BASE,
	}, {
		.regbase	= EXYNOS4412_GPJ1_BASE,
	}, {
		.regbase	= EXYNOS4412_GPK0_BASE,
	}, {
		.regbase	= EXYNOS4412_GPK1_BASE,
	}, {
		.regbase	= EXYNOS4412_GPK2_BASE,
	}, {
		.regbase	= EXYNOS4412_GPK3_BASE,
	}, {
		.regbase	= EXYNOS4412_GPL0_BASE,
	}, {
		.regbase	= EXYNOS4412_GPL1_BASE,
	}, {
		.regbase	= EXYNOS4412_GPL2_BASE,
	}, {
		.regbase	= EXYNOS4412_GPY0_BASE,
	}, {
		.regbase	= EXYNOS4412_GPY1_BASE,
	}, {
		.regbase	= EXYNOS4412_GPY2_BASE,
	}, {
		.regbase	= EXYNOS4412_GPY3_BASE,
	}, {
		.regbase	= EXYNOS4412_GPY4_BASE,
	}, {
		.regbase	= EXYNOS4412_GPY5_BASE,
	}, {
		.regbase	= EXYNOS4412_GPY6_BASE,
	}, {
		.regbase	= EXYNOS4412_GPM0_BASE,
	}, {
		.regbase	= EXYNOS4412_GPM1_BASE,
	}, {
		.regbase	= EXYNOS4412_GPM2_BASE,
	}, {
		.regbase	= EXYNOS4412_GPM3_BASE,
	}, {
		.regbase	= EXYNOS4412_GPM4_BASE,
	}, {
		.regbase	= EXYNOS4412_GPX0_BASE,
	}, {
		.regbase	= EXYNOS4412_GPX1_BASE,
	}, {
		.regbase	= EXYNOS4412_GPX2_BASE,
	}, {
		.regbase	= EXYNOS4412_GPX3_BASE,
	}, {
		.regbase	= EXYNOS4412_GPZ_BASE,
	}, {
		.regbase	= EXYNOS4412_GPV0_BASE,
	}, {
		.regbase	= EXYNOS4412_GPV1_BASE,
	}, {
		.regbase	= EXYNOS4412_GPV2_BASE,
	}, {
		.regbase	= EXYNOS4412_GPV3_BASE,
	}, {
		.regbase	= EXYNOS4412_GPV4_BASE,
	}
};

static struct gpio_t exynos4412_gpios[] = {
	{
		.name				= "GPA0",
		.base				= 0,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[0],
	}, {
		.name				= "GPA1",
		.base				= 8,
		.ngpio				= 6,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[1],
	}, {
		.name				= "GPB",
		.base				= 16,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[2],
	}, {
		.name				= "GPC0",
		.base				= 24,
		.ngpio				= 5,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[3],
	}, {
		.name				= "GPC1",
		.base				= 32,
		.ngpio				= 5,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[4],
	}, {
		.name				= "GPD0",
		.base				= 40,
		.ngpio				= 4,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[5],
	}, {
		.name				= "GPD1",
		.base				= 48,
		.ngpio				= 4,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[6],
	}, {
		.name				= "GPF0",
		.base				= 56,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[7],
	}, {
		.name				= "GPF1",
		.base				= 64,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[8],
	}, {
		.name				= "GPF2",
		.base				= 72,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[9],
	}, {
		.name				= "GPF3",
		.base				= 80,
		.ngpio				= 6,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[10],
	}, {
		.name				= "GPJ0",
		.base				= 88,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[11],
	}, {
		.name				= "GPJ1",
		.base				= 96,
		.ngpio				= 5,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[12],
	}, {
		.name				= "GPK0",
		.base				= 104,
		.ngpio				= 7,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[13],
	}, {
		.name				= "GPK1",
		.base				= 112,
		.ngpio				= 7,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[14],
	}, {
		.name				= "GPK2",
		.base				= 120,
		.ngpio				= 7,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[15],
	}, {
		.name				= "GPK3",
		.base				= 128,
		.ngpio				= 7,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[16],
	}, {
		.name				= "GPL0",
		.base				= 136,
		.ngpio				= 7,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[17],
	}, {
		.name				= "GPL1",
		.base				= 144,
		.ngpio				= 2,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[18],
	}, {
		.name				= "GPL2",
		.base				= 152,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[19],
	}, {
		.name				= "GPY0",
		.base				= 160,
		.ngpio				= 6,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[20],
	}, {
		.name				= "GPY1",
		.base				= 168,
		.ngpio				= 4,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[21],
	}, {
		.name				= "GPY2",
		.base				= 176,
		.ngpio				= 6,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[22],
	}, {
		.name				= "GPY3",
		.base				= 184,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[23],
	}, {
		.name				= "GPY4",
		.base				= 192,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[24],
	}, {
		.name				= "GPY5",
		.base				= 200,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[25],
	}, {
		.name				= "GPY6",
		.base				= 208,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[26],
	}, {
		.name				= "GPM0",
		.base				= 216,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[27],
	}, {
		.name				= "GPM1",
		.base				= 224,
		.ngpio				= 7,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[28],
	}, {
		.name				= "GPM2",
		.base				= 232,
		.ngpio				= 5,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[29],
	}, {
		.name				= "GPM3",
		.base				= 240,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[30],
	}, {
		.name				= "GPM4",
		.base				= 248,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[31],
	}, {
		.name				= "GPX0",
		.base				= 256,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[32],
	}, {
		.name				= "GPX1",
		.base				= 264,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[33],
	}, {
		.name				= "GPX2",
		.base				= 272,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[33],
	}, {
		.name				= "GPX3",
		.base				= 280,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[33],
	}, {
		.name				= "GPZ",
		.base				= 288,
		.ngpio				= 7,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[33],
	}, {
		.name				= "GPV0",
		.base				= 296,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[33],
	}, {
		.name				= "GPV1",
		.base				= 304,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[33],
	}, {
		.name				= "GPV2",
		.base				= 312,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[33],
	}, {
		.name				= "GPV3",
		.base				= 320,
		.ngpio				= 8,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[33],
	}, {
		.name				= "GPV4",
		.base				= 328,
		.ngpio				= 2,
		.cfg_pin			= exynos4412_gpio_cfg_pin,
		.set_pull			= exynos4412_gpio_set_pull,
		.set_drv			= exynos4412_gpio_set_drv,
		.set_rate			= exynos4412_gpio_set_rate,
		.direction_output	= exynos4412_gpio_direction_output,
		.direction_input	= exynos4412_gpio_direction_input,
		.set_value			= exynos4412_gpio_set_value,
		.get_value			= exynos4412_gpio_get_value,
		.priv				= &exynos4412_gpio_datas[33],
	}
};

static __init void exynos4412_gpio_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(exynos4412_gpios); i++)
	{
		if(register_gpio(&exynos4412_gpios[i]))
			LOG("Register gpio '%s'", exynos4412_gpios[i].name);
		else
			LOG("Failed to register gpio '%s'", exynos4412_gpios[i].name);
	}
}

static __exit void exynos4412_gpio_exit(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(exynos4412_gpios); i++)
	{
		if(register_gpio(&exynos4412_gpios[i]))
			LOG("Unregister gpio '%s'", exynos4412_gpios[i].name);
		else
			LOG("Failed to unregister gpio '%s'", exynos4412_gpios[i].name);
	}
}

postcore_initcall(exynos4412_gpio_init);
postcore_exitcall(exynos4412_gpio_exit);
