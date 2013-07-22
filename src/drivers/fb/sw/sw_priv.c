/*
 * drivers/fb/sw/sw_priv.c
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

void render_sw_create_priv_data(struct render_t * render)
{
	if(!render)
		return;
	render->priv = pixman_image_create_bits_no_clear(pixel_format_to_pixman_format_code(render->format), render->width, render->height, render->pixels, render->pitch);
}

void render_sw_destroy_priv_data(struct render_t * render)
{
	if(!render)
		return;
	pixman_image_unref((pixman_image_t *)render->priv);
}
