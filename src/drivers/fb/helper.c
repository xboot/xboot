/*
 * drivers/fb/helper.c
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

#include <fb/helper.h>

u32_t fb_map_color(struct fb * fb, struct color_t * col)
{
	return surface_map_color(&(fb->info->surface), col);
}

bool_t fb_fill_rect(struct fb * fb, u32_t c, u32_t x, u32_t y, u32_t w, u32_t h)
{
	struct rect_t rect;

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	surface_set_clip_rect(&(fb->info->surface), NULL);
	return surface_fill(&(fb->info->surface), &rect, c, BLEND_MODE_REPLACE);
}

bool_t fb_blit(struct fb * fb, struct surface_t * surface, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	struct rect_t dst_rect, src_rect;

	dst_rect.x = x;
	dst_rect.y = y;
	dst_rect.w = w;
	dst_rect.h = h;

	src_rect.x = ox;
	src_rect.y = oy;
	src_rect.w = w;
	src_rect.h = h;

	surface_set_clip_rect(&(fb->info->surface), NULL);
	return surface_blit(&(fb->info->surface), &dst_rect, surface, &src_rect, BLEND_MODE_REPLACE);
}

bool_t fb_putcode(struct fb * fb, u32_t code, u32_t fc, u32_t bc, u32_t x, u32_t y)
{
	struct surface_t * face;
	bool_t ret;

	face = surface_alloc_from_gimage(lookup_console_font_face(code, fc, bc));
	if(!face)
		return FALSE;

	ret = fb_blit(fb, face, x, y, face->w, face->h, 0, 0);
	surface_free(face);

	return ret;
}
