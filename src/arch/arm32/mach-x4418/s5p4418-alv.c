/*
 * s5p4418-alv.c
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

#include <xboot.h>
#include <s5p4418/reg-alv.h>
#include <s5p4418-gpio.h>

struct s5p4418_gpiochip_alv_data_t
{
	physical_addr_t regbase;
};

static inline void s5p4418_gpiochip_alv_write_enable(struct gpiochip_t * chip)
{
	struct s5p4418_gpiochip_alv_data_t * dat = (struct s5p4418_gpiochip_alv_data_t *)chip->priv;
	write32(phys_to_virt(dat->regbase + GPIOALV_PWRGATEREG), 0x1);
}

static inline void s5p4418_gpiochip_alv_write_disable(struct gpiochip_t * chip)
{
	struct s5p4418_gpiochip_alv_data_t * dat = (struct s5p4418_gpiochip_alv_data_t *)chip->priv;
	write32(phys_to_virt(dat->regbase + GPIOALV_PWRGATEREG), 0x0);
}

static void s5p4418_gpiochip_alv_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
}

static int s5p4418_gpiochip_alv_get_cfg(struct gpiochip_t * chip, int offset)
{
	return 0;
}

static void s5p4418_gpiochip_alv_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct s5p4418_gpiochip_alv_data_t * dat = (struct s5p4418_gpiochip_alv_data_t *)chip->priv;
	u32_t val = 0;

	if(offset >= chip->ngpio)
		return;

	s5p4418_gpiochip_alv_write_enable(chip);
	switch(pull)
	{
	case GPIO_PULL_UP:
		val |= 0x1 << offset;
		write32(phys_to_virt(dat->regbase + GPIOALV_PADPULLUPSETREG), val);
		break;

	case GPIO_PULL_DOWN:
		val |= 0x1 << offset;
		write32(phys_to_virt(dat->regbase + GPIOALV_PADPULLUPRSTREG), val);
		break;

	case GPIO_PULL_NONE:
		break;

	default:
		break;
	}
	s5p4418_gpiochip_alv_write_disable(chip);
}

static enum gpio_pull_t s5p4418_gpiochip_alv_get_pull(struct gpiochip_t * chip, int offset)
{
	struct s5p4418_gpiochip_alv_data_t * dat = (struct s5p4418_gpiochip_alv_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;

	val = read32(phys_to_virt(dat->regbase + GPIOALV_PADPULLUPREADREG));
	if(!((val >> offset) & 0x1))
		return GPIO_PULL_DOWN;
	else
		return GPIO_PULL_UP;

	return GPIO_PULL_NONE;
}

static void s5p4418_gpiochip_alv_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
}

static enum gpio_drv_t s5p4418_gpiochip_alv_get_drv(struct gpiochip_t * chip, int offset)
{
	return GPIO_DRV_LOW;
}

static void s5p4418_gpiochip_alv_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t s5p4418_gpiochip_alv_get_rate(struct gpiochip_t * chip, int offset)
{
	return GPIO_RATE_SLOW;
}

static void s5p4418_gpiochip_alv_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	struct s5p4418_gpiochip_alv_data_t * dat = (struct s5p4418_gpiochip_alv_data_t *)chip->priv;
	u32_t val = 0;

	if(offset >= chip->ngpio)
		return;

	s5p4418_gpiochip_alv_write_enable(chip);
	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		val |= 0x1 << offset;
		write32(phys_to_virt(dat->regbase + GPIOALV_PADOUTENBRSTREG), val);
		break;

	case GPIO_DIRECTION_OUTPUT:
		val |= 0x1 << offset;
		write32(phys_to_virt(dat->regbase + GPIOALV_PADOUTENBSETREG), val);
		break;

	default:
		break;
	}
	s5p4418_gpiochip_alv_write_disable(chip);
}

static enum gpio_direction_t s5p4418_gpiochip_alv_get_dir(struct gpiochip_t * chip, int offset)
{
	struct s5p4418_gpiochip_alv_data_t * dat = (struct s5p4418_gpiochip_alv_data_t *)chip->priv;
	u32_t val, d;

	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_UNKOWN;

	val = read32(phys_to_virt(dat->regbase + GPIOALV_PADOUTENBREADREG));
	d = (val >> offset) & 0x1;
	switch(d)
	{
	case 0x0:
		return GPIO_DIRECTION_INPUT;
	case 0x1:
		return GPIO_DIRECTION_OUTPUT;
	default:
		break;
	}
	return GPIO_DIRECTION_UNKOWN;
}

static void s5p4418_gpiochip_alv_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct s5p4418_gpiochip_alv_data_t * dat = (struct s5p4418_gpiochip_alv_data_t *)chip->priv;
	u32_t val = 0;

	if(offset >= chip->ngpio)
		return;

	s5p4418_gpiochip_alv_write_enable(chip);
	if(value)
	{
		val |= 0x1 << offset;
		write32(phys_to_virt(dat->regbase + GPIOALV_PADOUTSETREG), val);
	}
	else
	{
		val |= 0x1 << offset;
		write32(phys_to_virt(dat->regbase + GPIOALV_PADOUTRSTREG), val);
	}
	s5p4418_gpiochip_alv_write_disable(chip);
}

static int s5p4418_gpiochip_alv_get_value(struct gpiochip_t * chip, int offset)
{
	struct s5p4418_gpiochip_alv_data_t * dat = (struct s5p4418_gpiochip_alv_data_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	val = read32(phys_to_virt(dat->regbase + GPIOALV_GPIOINPUTVALUE));
	return !!(val & (1 << offset));
}

static const char * s5p4418_gpiochip_alv_to_irq(struct gpiochip_t * chip, int offset)
{
	static const char * irq_gpio_alv[] = {
		"GPIOALV0", "GPIOALV1", "GPIOALV2", "GPIOALV3", "GPIOALV4", "GPIOALV5",
	};

	if(offset >= chip->ngpio)
		return 0;
	return irq_gpio_alv[offset];
}

static struct s5p4418_gpiochip_alv_data_t gpiochip_alv_data = {
	.regbase	= S5P4418_GPIOALV_BASE,
};

struct gpiochip_t gpiochip_alv = {
	.name		= "GPIOALV",
	.base		= S5P4418_GPIOALV(0),
	.ngpio		= 6,
	.set_cfg	= s5p4418_gpiochip_alv_set_cfg,
	.get_cfg	= s5p4418_gpiochip_alv_get_cfg,
	.set_pull	= s5p4418_gpiochip_alv_set_pull,
	.get_pull	= s5p4418_gpiochip_alv_get_pull,
	.set_drv	= s5p4418_gpiochip_alv_set_drv,
	.get_drv	= s5p4418_gpiochip_alv_get_drv,
	.set_rate	= s5p4418_gpiochip_alv_set_rate,
	.get_rate	= s5p4418_gpiochip_alv_get_rate,
	.set_dir	= s5p4418_gpiochip_alv_set_dir,
	.get_dir	= s5p4418_gpiochip_alv_get_dir,
	.set_value	= s5p4418_gpiochip_alv_set_value,
	.get_value	= s5p4418_gpiochip_alv_get_value,
	.to_irq		= s5p4418_gpiochip_alv_to_irq,
	.priv		= &gpiochip_alv_data,
};

static __init void s5p4418_gpiochip_alv_init(void)
{
	if(register_gpiochip(&gpiochip_alv))
		LOG("Register gpiochip '%s'", gpiochip_alv.name);
	else
		LOG("Failed to register gpiochip '%s'", gpiochip_alv.name);
}

static __exit void s5p4418_gpiochip_alv_exit(void)
{
	if(unregister_gpiochip(&gpiochip_alv))
		LOG("Unregister gpiochip '%s'", gpiochip_alv.name);
	else
		LOG("Failed to unregister gpiochip '%s'", gpiochip_alv.name);
}

core_initcall(s5p4418_gpiochip_alv_init);
core_exitcall(s5p4418_gpiochip_alv_exit);
