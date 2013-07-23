/*
 * drivers/fb/sw/sw_render.c
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

#include <fb/sw/sw.h>

void sw_render_create_priv_data(struct render_t * render)
{
	if(!render)
		return;
	render->priv = pixman_image_create_bits_no_clear(pixel_format_to_pixman_format_code(render->format), render->width, render->height, render->pixels, render->pitch);
}

void sw_render_destroy_priv_data(struct render_t * render)
{
	if(!render)
		return;
	pixman_image_unref((pixman_image_t *)render->priv);
}

void sw_render_clear(struct render_t * render, struct rect_t * r, struct color_t * c)
{
	pixman_rectangle16_t rect;
	pixman_color_t color;

	if(!render)
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
		rect.width = render->width;
		rect.height = render->height;
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

	pixman_image_fill_rectangles(PIXMAN_OP_SRC, render->priv, &color, 1, &rect);
}

struct texture_t * sw_render_snapshot(struct render_t * render)
{
	if(!render)
		return NULL;

	return sw_render_alloc_texture(render, render->pixels, render->width, render->height, render->format);
}
