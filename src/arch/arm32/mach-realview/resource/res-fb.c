/*
 * resource/res-fb.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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
#include <realview-fb.h>

static void lcd_init(struct realview_fb_data_t * dat)
{
}

static void lcd_exit(struct realview_fb_data_t * dat)
{
}

static void lcd_set_backlight(struct realview_fb_data_t * dat, int brightness)
{
	dat->brightness = brightness;
	if(dat->brightness)
	{
	}
	else
	{
	}
}

static int lcd_get_backlight(struct realview_fb_data_t * dat)
{
	return dat->brightness;
}

static struct realview_fb_data_t realview_fb_data = {
	.regbase			= REALVIEW_CLCD_BASE,

	.width				= 640,
	.height				= 480,
	.xdpi				= 133,
	.ydpi				= 133,
	.bits_per_pixel		= 32,
	.bytes_per_pixel	= 4,

	.timing = {
		.h_fp			= 2,
		.h_bp			= 2,
		.h_sw			= 2,
		.v_fp			= 2,
		.v_bp			= 2,
		.v_sw			= 2,
	},

	.init				= lcd_init,
	.exit				= lcd_exit,

	.brightness			= 0,
	.set_backlight		= lcd_set_backlight,
	.get_backlight		= lcd_get_backlight,
};

static struct resource_t res_fb = {
	.name		= "realview-fb",
	.id			= -1,
	.data		= &realview_fb_data,
};

static __init void resource_fb_init(void)
{
	if(register_resource(&res_fb))
		LOG("Register resource %s:'%s.%d'", res_fb.mach, res_fb.name, res_fb.id);
	else
		LOG("Failed to register resource %s:'%s.%d'", res_fb.mach, res_fb.name, res_fb.id);
}

static __exit void resource_fb_exit(void)
{
	if(unregister_resource(&res_fb))
		LOG("Unregister resource %s:'%s.%d'", res_fb.mach, res_fb.name, res_fb.id);
	else
		LOG("Failed to unregister resource %s:'%s.%d'", res_fb.mach, res_fb.name, res_fb.id);
}

resource_initcall(resource_fb_init);
resource_exitcall(resource_fb_exit);
