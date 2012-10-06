/*
 * sandbox-fb.c
 *
 * Copyright (c) 2007-2012  jianjun jiang <jerryjianjun@gmail.com>
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
#include <fb/fb.h>

extern struct sandbox_t * sandbox;

static struct fb_info info;
static void fb_init(struct fb * fb)
{
	info.name = "fb";

/*	info.surface.info.bits_per_pixel = sandbox->display->bits_per_pixel;
	info.surface.info.bytes_per_pixel = sandbox->display->bytes_per_pixel;
	info.surface.info.red_mask_size = sandbox->display->red_mask_size;
	info.surface.info.red_field_pos = sandbox->display->red_field_pos;
	info.surface.info.green_mask_size = sandbox->display->green_mask_size;
	info.surface.info.green_field_pos = sandbox->display->green_field_pos;
	info.surface.info.blue_mask_size = sandbox->display->blue_mask_size;
	info.surface.info.blue_field_pos = sandbox->display->blue_field_pos;
	info.surface.info.alpha_mask_size = sandbox->display->alpha_mask_size;
	info.surface.info.alpha_field_pos = sandbox->display->alpha_field_pos;

	info.surface.w = sandbox->display->width;
	info.surface.h = sandbox->display->height;
	info.surface.pitch = sandbox->display->width * sandbox->display->bytes_per_pixel / 8;
	info.surface.flag = SURFACE_PIXELS_DONTFREE;
	info.surface.pixels = sandbox->display->pixels;

	info.surface.clip.x = 0;
	info.surface.clip.y = 0;
	info.surface.clip.w = sandbox->display->width;
	info.surface.clip.h = sandbox->display->height;

	info.surface.maps.point = map_software_point;
	info.surface.maps.hline = map_software_hline;
	info.surface.maps.vline = map_software_vline;
	info.surface.maps.fill = map_software_fill;
	info.surface.maps.blit = map_software_blit;
	info.surface.maps.scale = map_software_scale;
	info.surface.maps.rotate = map_software_rotate;
	info.surface.maps.transform = map_software_transform;*/
}

static void fb_exit(struct fb * fb)
{
}

static void fb_swap(struct fb * fb)
{
}

static void fb_flush(struct fb * fb)
{
}

static int fb_ioctl(struct fb * fb, int cmd, void * arg)
{
	static u8_t brightness = 0;
	u8_t * p;

	switch(cmd)
	{
	case IOCTL_SET_FB_BACKLIGHT:
		p = (u8_t *)arg;
		brightness = (*p) & 0xff;
		return 0;

	case IOCTL_GET_FB_BACKLIGHT:
		p = (u8_t *)arg;
		*p = brightness;
		return 0;

	default:
		break;
	}

	return -1;
}

static struct fb sandbox_fb = {
	.info			= &info,
	.init			= fb_init,
	.exit			= fb_exit,
	.swap			= fb_swap,
	.flush			= fb_flush,
	.ioctl			= fb_ioctl,
	.priv			= NULL,
};

static __init void sandbox_fb_init(void)
{
	if(!register_framebuffer(&sandbox_fb))
		LOG_E("failed to register framebuffer driver '%s'", sandbox_fb.info->name);
}

static __exit void sandbox_fb_exit(void)
{
	if(!unregister_framebuffer(&sandbox_fb))
		LOG_E("failed to unregister framebuffer driver '%s'", sandbox_fb.info->name);
}

module_init(sandbox_fb_init, LEVEL_DRIVER);
module_exit(sandbox_fb_exit, LEVEL_DRIVER);
