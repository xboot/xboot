/*
 * kernel/graphic/graphic.c
 *
 * Copyright (c) 2007-2011  jianjun jiang <jerryjianjun@gmail.com>
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

#include <graphic/surface.h>


//XXX FOR DEBUG
#include <rect.h>
#include <fb/bitmap.h>
#include <fb/graphic.h>
#include <fb/fb.h>
bool_t display_surface(struct fb * fb, struct surface_t * surface)
{
	struct bitmap * bitmap;
	struct rect rect, to, r, save;
	s32_t ox, oy;
	u32_t color;
	struct picture * pic;

	if(!surface || !fb)
		return FALSE;

	pic = malloc(sizeof(struct picture));
	if(!pic)
		return FALSE;

	pic->width = surface->w;
	pic->height = surface->h;
	pic->bytes_per_pixel = 4;
	pic->data = surface->pixels;

	if(bitmap_load_from_picture(&bitmap, pic) != TRUE)
		return FALSE;

	rect_set(&rect, 0, 0, fb->info->bitmap.info.width, fb->info->bitmap.info.height);
	rect_set(&to, 0, 0, bitmap->info.width, bitmap->info.height);
	rect_align(&rect, &to, ALIGN_CENTER);

	if(rect_intersect_old(&r, &rect, &to) != TRUE)
	{
		bitmap_destroy(bitmap);
		return FALSE;
	}

	ox = r.left - to.left;
	oy = r.top - to.top;
	if(ox < 0)
		ox = 0;
	if(oy < 0)
		oy = 0;

	fb_get_viewport(fb, &save);

	fb_set_viewport(fb, &rect);
	color = fb_map_color(fb, 0, 0, 0, 255);
	fb_fill_rect(fb, color, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);

	fb_set_viewport(fb, &r);
	fb_blit_bitmap(fb, bitmap, BLIT_MODE_REPLACE, r.left, r.top, r.right-r.left, r.bottom-r.top, ox, oy);

	fb_set_viewport(fb, &save);

	bitmap_destroy(bitmap);
	free(pic);

	return TRUE;
}
