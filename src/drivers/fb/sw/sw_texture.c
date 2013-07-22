/*
 * drivers/fb/sw/sw_texture.c
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

struct texture_t * render_sw_alloc(struct render_t * render, u32_t w, u32_t h)
{
	struct texture_t * texture;
	u32_t pitch, size;

	if(!render)
		return NULL;

	if( (w <= 0) && (h <= 0) )
		return NULL;

	texture = (struct texture_t *)malloc(sizeof(struct texture_t));
	if(! texture)
		return NULL;

	switch(render->format)
	{
	case PIXEL_FORMAT_ARGB32:
		pitch = w * 4;
		break;
	case PIXEL_FORMAT_RGB24:
		pitch = w * 3;
		break;
	case PIXEL_FORMAT_A8:
		pitch = w * 1;
		break;
	case PIXEL_FORMAT_A1:
		pitch = (w + 7) / 8;
		break;
	case PIXEL_FORMAT_RGB16_565:
		pitch = w * 2;
		break;
	default:
		pitch = w * 4;
		break;
	}
	pitch = (pitch + 0x3) & ~0x3;

	texture->width = w;
	texture->height = h;
	texture->pitch = pitch;
	texture->format = render->format;
	texture->priv = render;

	size = texture->height * pitch;
	texture->pixels = malloc(size);
	if (!texture->pixels)
	{
		free(texture);
		return NULL;
	}

	return texture;
}

void render_sw_free(struct texture_t * texture)
{
	if(!texture)
		return;

	if(texture->pixels)
		free(texture->pixels);

	free(texture);
}
