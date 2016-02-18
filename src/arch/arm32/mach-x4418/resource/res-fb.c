/*
 * resource/res-fb.c
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
#include <s5p4418-fb.h>

static struct s5p4418_fb_data_t s5p4418_fb_data = {
	.physdpc			= S5P4418_DPC0_BASE,
	.physmlc			= S5P4418_MLC0_BASE,

	.width				= 1024,
	.height				= 600,
	.xdpi				= 133,
	.ydpi				= 133,
	.bits_per_pixel		= 32,
	.bytes_per_pixel	= 4,

	.mode = {
		.rgbmode		= 1,
		.scanmode		= 0,
		.enbedsync		= 0,
		.polfield		= 0,
		.swaprb			= 0,
		.format			= S5P4418_DPC_FORMAT_RGB888,
		.ycorder		= S5P4418_DPC_YCORDER_CBYCRY,
		.rdither		= S5P4418_DPC_DITHER_BYPASS,
		.gdither		= S5P4418_DPC_DITHER_BYPASS,
		.bdither		= S5P4418_DPC_DITHER_BYPASS,
	},

	.timing = {
		.pixel_clock_hz	= 52 * 1000 * 1000,
		.h_front_porch	= 160,
		.h_sync_width	= 20,
		.h_back_porch	= 140,
		.v_front_porch	= 12,
		.v_sync_width	= 3,
		.v_back_porch	= 20,

		.vs_start_offset= 1,
		.vs_end_offset	= 1,
		.ev_start_offset= 1,
		.ev_end_offset	= 1,

		.d_rgb_pvd		= 0,
		.d_hsync_cp1	= 7,
		.d_vsync_fram	= 7,
		.d_de_cp2		= 7,

		.h_sync_invert	= 1,
		.v_sync_invert	= 1,
	},

	.backlight	= "led-pwm-bl.0",
};

static struct resource_t res_fb = {
	.name		= "s5p4418-fb",
	.id			= -1,
	.data		= &s5p4418_fb_data,
};

static __init void resource_fb_init(void)
{
	register_resource(&res_fb);
}
resource_initcall(resource_fb_init);
