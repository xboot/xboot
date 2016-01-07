/*
 * bcm2836-gpio.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
#include <bcm2836/reg-gpio.h>
#include <bcm2836-gpio.h>

struct bcm2836_gpiochip_data_t
{
	physical_addr_t regbase;
};

static void bcm2836_gpiochip_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
	struct bcm2836_gpiochip_data_t * dat = (struct bcm2836_gpiochip_data_t *)chip->priv;
	int bank = offset / 10;
	int field = (offset - 10 * bank) * 3;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(cfg & 0x7)
	{
	case 0x0: cfg = 0x4; break;	/* Alternate Function 0 */
	case 0x1: cfg = 0x5; break;	/* Alternate Function 1 */
	case 0x2: cfg = 0x6; break;	/* Alternate Function 2 */
	case 0x3: cfg = 0x7; break;	/* Alternate Function 3 */
	case 0x4: cfg = 0x3; break;	/* Alternate Function 4 */
	case 0x5: cfg = 0x2; break;	/* Alternate Function 5 */
	case 0x6: cfg = 0x0; break;	/* GPIO Input */
	case 0x7: cfg = 0x1; break;	/* GPIO Output */
	default: break;
	}

	val = read32(phys_to_virt(dat->regbase + GPIO_FSEL(bank)));
	val &= ~(0x7 << field);
	val |= cfg << field;
	write32(phys_to_virt(dat->regbase + GPIO_FSEL(bank)), val);
}

static int bcm2836_gpiochip_get_cfg(struct gpiochip_t * chip, int offset)
{
	struct bcm2836_gpiochip_data_t * dat = (struct bcm2836_gpiochip_data_t *)chip->priv;
	int bank = offset / 10;
	int field = (offset - 10 * bank) * 3;
	int cfg;
	u32_t val;

	val = read32(phys_to_virt(dat->regbase + GPIO_FSEL(bank)));
	switch((val >> field) & 0x7)
	{
	case 0x0: cfg = 0x6; break;	/* GPIO Input */
	case 0x1: cfg = 0x7; break;	/* GPIO Output */
	case 0x2: cfg = 0x5; break;	/* Alternate Function 5 */
	case 0x3: cfg = 0x4; break;	/* Alternate Function 4 */
	case 0x4: cfg = 0x0; break;	/* Alternate Function 0 */
	case 0x5: cfg = 0x1; break;	/* Alternate Function 1 */
	case 0x6: cfg = 0x2; break;	/* Alternate Function 2 */
	case 0x7: cfg = 0x3; break;	/* Alternate Function 3 */
	default: break;
	}
	return cfg;
}

static void bcm2836_gpiochip_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct bcm2836_gpiochip_data_t * dat = (struct bcm2836_gpiochip_data_t *)chip->priv;
	int bank = offset / 32;
	int field = (offset - 32 * bank);

	if(offset >= chip->ngpio)
		return;

	switch(pull)
	{
	case GPIO_PULL_UP:
		write32(phys_to_virt(dat->regbase + GPIO_UD(0)), 2);
		break;

	case GPIO_PULL_DOWN:
		write32(phys_to_virt(dat->regbase + GPIO_UD(0)), 1);
		break;

	case GPIO_PULL_NONE:
		write32(phys_to_virt(dat->regbase + GPIO_UD(0)), 0);
		break;

	default:
		return;
	}

	udelay(5);
	write32(phys_to_virt(dat->regbase + GPIO_UDCLK(bank)), 1 << field);
	udelay(5);
	write32(phys_to_virt(dat->regbase + GPIO_UD(0)), 0);
	write32(phys_to_virt(dat->regbase + GPIO_UDCLK(bank)), 0 << field);
}

static enum gpio_pull_t bcm2836_gpiochip_get_pull(struct gpiochip_t * chip, int offset)
{
	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;
	return GPIO_PULL_NONE;
}

static void bcm2836_gpiochip_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
}

static enum gpio_drv_t bcm2836_gpiochip_get_drv(struct gpiochip_t * chip, int offset)
{
	return GPIO_DRV_LOW;
}

static void bcm2836_gpiochip_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t bcm2836_gpiochip_get_rate(struct gpiochip_t * chip, int offset)
{
	return GPIO_RATE_SLOW;
}

static void bcm2836_gpiochip_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	if(offset >= chip->ngpio)
		return;

	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		bcm2836_gpiochip_set_cfg(chip, offset, 0x6);
		break;

	case GPIO_DIRECTION_OUTPUT:
		bcm2836_gpiochip_set_cfg(chip, offset, 0x7);
		break;

	default:
		break;
	}
}

static enum gpio_direction_t bcm2836_gpiochip_get_dir(struct gpiochip_t * chip, int offset)
{
	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_UNKOWN;

	switch(bcm2836_gpiochip_get_cfg(chip, offset))
	{
	case 0x6:
		return GPIO_DIRECTION_INPUT;
	case 0x7:
		return GPIO_DIRECTION_OUTPUT;
	default:
		break;
	}
	return GPIO_DIRECTION_UNKOWN;
}

static void bcm2836_gpiochip_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct bcm2836_gpiochip_data_t * dat = (struct bcm2836_gpiochip_data_t *)chip->priv;
	int bank = offset / 32;
	int field = (offset - 32 * bank);

	if(offset >= chip->ngpio)
		return;

	if(value)
		write32(phys_to_virt(dat->regbase + GPIO_SET(bank)), 1 << field);
	else
		write32(phys_to_virt(dat->regbase + GPIO_CLR(bank)), 1 << field);
}

static int bcm2836_gpiochip_get_value(struct gpiochip_t * chip, int offset)
{
	struct bcm2836_gpiochip_data_t * dat = (struct bcm2836_gpiochip_data_t *)chip->priv;
	int bank = offset / 32;
	int field = (offset - 32 * bank);
	u32_t lev;

	if(offset >= chip->ngpio)
		return 0;

	lev = read32(phys_to_virt(dat->regbase + GPIO_LEV(bank)));
	return (lev & (1 << field)) ? 1 : 0;
}

static const char * bcm2836_gpiochip_to_irq(struct gpiochip_t * chip, int offset)
{
	static const char * irq_gpio[] = {
		"GPIO0",  "GPIO1",  "GPIO2",  "GPIO3",  "GPIO4",  "GPIO5",  "GPIO6",  "GPIO7",
		"GPIO8",  "GPIO9",  "GPIO10", "GPIO11", "GPIO12", "GPIO13", "GPIO14", "GPIO15",
		"GPIO16", "GPIO17", "GPIO18", "GPIO19", "GPIO20", "GPIO21", "GPIO22", "GPIO23",
		"GPIO24", "GPIO25", "GPIO26", "GPIO27", "GPIO28", "GPIO29", "GPIO30", "GPIO31",
		"GPIO32", "GPIO33", "GPIO34", "GPIO35", "GPIO36", "GPIO37", "GPIO38", "GPIO39",
		"GPIO40", "GPIO41", "GPIO42", "GPIO43", "GPIO44", "GPIO45", "GPIO46", "GPIO47",
		"GPIO48", "GPIO49", "GPIO50", "GPIO51", "GPIO52", "GPIO53",
	};

	if(offset >= chip->ngpio)
		return 0;
	return irq_gpio[offset];
}

static struct bcm2836_gpiochip_data_t gpiochip_data = {
	.regbase	= BCM2836_GPIO_BASE,
};

struct gpiochip_t gpiochip_alv = {
	.name		= "GPIO",
	.base		= BCM2836_GPIO(0),
	.ngpio		= 54,
	.set_cfg	= bcm2836_gpiochip_set_cfg,
	.get_cfg	= bcm2836_gpiochip_get_cfg,
	.set_pull	= bcm2836_gpiochip_set_pull,
	.get_pull	= bcm2836_gpiochip_get_pull,
	.set_drv	= bcm2836_gpiochip_set_drv,
	.get_drv	= bcm2836_gpiochip_get_drv,
	.set_rate	= bcm2836_gpiochip_set_rate,
	.get_rate	= bcm2836_gpiochip_get_rate,
	.set_dir	= bcm2836_gpiochip_set_dir,
	.get_dir	= bcm2836_gpiochip_get_dir,
	.set_value	= bcm2836_gpiochip_set_value,
	.get_value	= bcm2836_gpiochip_get_value,
	.to_irq		= bcm2836_gpiochip_to_irq,
	.priv		= &gpiochip_data,
};

static __init void bcm2836_gpiochip_init(void)
{
	if(register_gpiochip(&gpiochip_alv))
		LOG("Register gpiochip '%s'", gpiochip_alv.name);
	else
		LOG("Failed to register gpiochip '%s'", gpiochip_alv.name);
}

static __exit void bcm2836_gpiochip_exit(void)
{
	if(unregister_gpiochip(&gpiochip_alv))
		LOG("Unregister gpiochip '%s'", gpiochip_alv.name);
	else
		LOG("Failed to unregister gpiochip '%s'", gpiochip_alv.name);
}

core_initcall(bcm2836_gpiochip_init);
core_exitcall(bcm2836_gpiochip_exit);
