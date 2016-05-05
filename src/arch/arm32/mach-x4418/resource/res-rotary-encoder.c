/*
 * resource/res-rotary-encoder.c
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
#include <input/rotary-encoder.h>
#include <s5p4418-gpio.h>

static struct rotary_encoder_data_t rotary_encoder_datas[] = {
	[0] = {
		.gpio_a		= S5P4418_GPIOD(28),
		.gpio_b		= S5P4418_GPIOD(31),
		.gpio_c		= S5P4418_GPIOE(2),
		.inverted_a	= 0,
		.inverted_b	= 0,
		.inverted_c	= 0,
		.step		= 1,
	},
};

static struct resource_t res_rotary_encoders[] = {
	{
		.name	= "rotary-encoder",
		.id		= -1,
		.data	= &rotary_encoder_datas[0],
	},
};

static __init void resource_rotary_encoder_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(res_rotary_encoders); i++)
		register_resource(&res_rotary_encoders[i]);
}
resource_initcall(resource_rotary_encoder_init);
