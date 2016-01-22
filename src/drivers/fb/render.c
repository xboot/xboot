/*
 * drivers/fb/renderer.c
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

#include <fb/render.h>

void render_clear(struct render_t * render, struct rect_t * r, struct color_t * c)
{
	if(render)
		render->clear(render, r, c);
}

struct texture_t * render_snapshot(struct render_t * render)
{
	if(render)
		return render->snapshot(render);
	return NULL;
}

struct texture_t * render_alloc_texture(struct render_t * render, void * pixels, u32_t w, u32_t h, enum pixel_format_t format)
{
	if(render)
		return render->alloc_texture(render, pixels, w, h, format);
	return NULL;
}

struct texture_t * render_alloc_texture_similar(struct render_t * render, u32_t w, u32_t h)
{
	if(render)
		return render->alloc_texture_similar(render, w, h);
	return NULL;
}

struct texture_t * render_alloc_texture_from_gimage(struct render_t * render, const struct gimage_t * image)
{
	if(!render || !image)
		return NULL;

	switch (image->bytes_per_pixel)
	{
	case 3:
		return render->alloc_texture(render, image->pixels, image->width, image->height, PIXEL_FORMAT_RGB24);

	case 4:
		return render->alloc_texture(render, image->pixels, image->width, image->height, PIXEL_FORMAT_ARGB32);

	default:
		break;
	}

	return NULL;
}

void render_free_texture(struct render_t * render, struct texture_t * texture)
{
	if(render)
		render->free_texture(render, texture);
}

void render_fill_texture(struct render_t * render, struct texture_t * texture, struct rect_t * r, struct color_t * c)
{
	if(render)
		render->fill_texture(render, texture, r, c);
}

void render_blit_texture(struct render_t * render, struct rect_t * drect, struct texture_t * texture, struct rect_t * srect)
{
	if(render)
		render->blit_texture(render, drect, texture, srect);
}
