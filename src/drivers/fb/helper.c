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

void fb_fill_rect(struct fb_t * fb, struct color_t * c, u32_t x, u32_t y, u32_t w, u32_t h)
{
	struct rect_t rect;

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	render_clear(fb->alone, &rect, c);
}

void fb_blit(struct fb_t * fb, struct texture_t * texture, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	struct rect_t drect, srect;

	drect.x = x;
	drect.y = y;
	drect.w = w;
	drect.h = h;

	srect.x = ox;
	srect.y = oy;
	srect.w = w;
	srect.h = h;

	render_blit_texture(fb->alone, &drect, texture, &srect);
}

void fb_putcode(struct fb_t * fb, u32_t code, struct color_t * fc, struct color_t * bc, u32_t x, u32_t y)
{
	struct texture_t * face = lookup_console_font_face(fb->alone, code, fc, bc);

	if(face)
		fb_blit(fb, face, x, y, face->width, face->height, 0, 0);
	render_free_texture(fb->alone, face);
}
