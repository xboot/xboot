/*
 * drivers/fb/sw/sw_texture_fill.c
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

#include <fb/sw/sw.h>

void sw_render_fill_texture(struct render_t * render, struct texture_t * texture, struct rect_t * r, struct color_t * c)
{
	pixman_rectangle16_t rect;
	pixman_color_t color;

	if(!texture)
		return;

	if(r)
	{
		rect.x = r->x;
		rect.y = r->y;
		rect.width = r->w;
		rect.height = r->h;
	}
	else
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = texture->width;
		rect.height = texture->height;
	}

	if(c)
	{
		color.red = c->r << 8;
		color.green = c->g << 8;
		color.blue = c->b << 8;
		color.alpha = c->a << 8;
	}
	else
	{
		color.red = 0xffff;
		color.green = 0xffff;
		color.blue = 0xffff;
		color.alpha = 0xffff;
	}

	pixman_image_fill_rectangles(PIXMAN_OP_SRC, texture->priv, &color, 1, &rect);
}
