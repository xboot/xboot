/*
 * bcm2836-gpio.c
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
#include <bcm2836-mbox.h>
#include <bcm2836-gpio.h>

struct gpiochip_data_t
{
	const char * name;
	int base;
	int ngpio;
};

struct gpiochip_pdata_t
{
	const char * name;
	int base;
	int ngpio;
	uint32_t virtbuf;
	uint32_t * status;
};

static struct gpiochip_data_t datas[] = {
	{
		.name	= "GPIOVIRT",
		.base	= BCM2836_GPIOVIRT(0),
		.ngpio	= 2,
	},
};

static void gpiochip_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
}

static int gpiochip_get_cfg(struct gpiochip_t * chip, int offset)
{
	return 0;
}

static void gpiochip_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
}

static enum gpio_pull_t gpiochip_get_pull(struct gpiochip_t * chip, int offset)
{
	return GPIO_PULL_NONE;
}

static void gpiochip_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
}

static enum gpio_drv_t gpiochip_get_drv(struct gpiochip_t * chip, int offset)
{
	return GPIO_DRV_LOW;
}

static void gpiochip_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t gpiochip_get_rate(struct gpiochip_t * chip, int offset)
{
	return GPIO_RATE_SLOW;
}

static void gpiochip_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
}

static enum gpio_direction_t gpiochip_get_dir(struct gpiochip_t * chip, int offset)
{
	return GPIO_DIRECTION_OUTPUT;
}

static void gpiochip_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	uint16_t enables, disables;
	int16_t diff, lit;

	enables = pdat->status[offset] >> 16;
	disables = pdat->status[offset] >> 0;
	diff = (int16_t)(enables - disables);
	lit = (diff > 0) ? 1 : 0;

	if((value && lit) || (!value && !lit))
		return;
	if(value)
		enables++;
	else
		disables++;

	pdat->status[offset] = (enables << 16) | (disables << 0);
	mb(); write32(pdat->virtbuf + offset * 4, pdat->status[offset]);
}

static int gpiochip_get_value(struct gpiochip_t * chip, int offset)
{
	struct gpiochip_pdata_t * pdat = (struct gpiochip_pdata_t *)chip->priv;
	return (read32(pdat->virtbuf + offset * 4) >> offset) & 0x1;
}

static int gpiochip_to_irq(struct gpiochip_t * chip, int offset)
{
	return -1;
}

static __init void bcm2836_gpiochip_init(void)
{
	struct gpiochip_pdata_t * pdat;
	struct gpiochip_t * chip;
	int i;

	for(i = 0; i < ARRAY_SIZE(datas); i++)
	{
		pdat = malloc(sizeof(struct gpiochip_pdata_t));
		if(!pdat)
			continue;

		chip = malloc(sizeof(struct gpiochip_t));
		if(!chip)
		{
			free(pdat);
			continue;
		}

		pdat->name = datas[i].name;
		pdat->base = datas[i].base;
		pdat->ngpio = datas[i].ngpio;
		pdat->virtbuf = bcm2836_mbox_fb_get_gpiovirt();
		pdat->status = malloc(sizeof(uint32_t) * pdat->ngpio);
		memset(pdat->status, 0, sizeof(uint32_t) * pdat->ngpio);

		chip->name = pdat->name;
		chip->base = pdat->base;
		chip->ngpio = pdat->ngpio;
		chip->set_cfg = gpiochip_set_cfg;
		chip->get_cfg = gpiochip_get_cfg;
		chip->set_pull = gpiochip_set_pull;
		chip->get_pull = gpiochip_get_pull;
		chip->set_drv = gpiochip_set_drv;
		chip->get_drv = gpiochip_get_drv;
		chip->set_rate = gpiochip_set_rate;
		chip->get_rate = gpiochip_get_rate;
		chip->set_dir = gpiochip_set_dir;
		chip->get_dir = gpiochip_get_dir;
		chip->set_value = gpiochip_set_value;
		chip->get_value = gpiochip_get_value;
		chip->to_irq = gpiochip_to_irq;
		chip->priv = pdat;

		register_gpiochip(chip);
	}
}
core_initcall(bcm2836_gpiochip_init);
