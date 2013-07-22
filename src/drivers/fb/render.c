/*
 * drivers/fb/renderer.c
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

#include <fb/render.h>

u32_t render_map_color(struct render_t * render , struct color_t * col)
{
	return 0;
}

void render_unmap_color(struct render_t * render, u32_t c, struct color_t * col)
{

}

void render_fill(struct render_t * render, struct rect_t * rect, u32_t c)
{
	if(render)
		render->fill(render, rect, c);
}

void render_blit(struct render_t * render, struct rect_t * drect, struct texture_t * texture, struct rect_t * srect)
{
	if(render)
		render->blit(render, drect, texture, srect);
}

struct texture_t * render_scale(struct render_t * render, struct texture_t * texture, u32_t w, u32_t h)
{
	if(!render)
		return NULL;

	return render->scale(render, texture, w, h);
}

struct texture_t * render_rotate(struct render_t * render, struct rect_t * rect, u32_t angle)
{
	if(!render)
		return NULL;

	return render->rotate(render, rect, angle);
}
