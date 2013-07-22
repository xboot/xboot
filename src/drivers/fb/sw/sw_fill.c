/*
 * drivers/fb/sw/sw_fill.c
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

static void sw_fill_1byte(struct render_t * render, struct rect_t * rect, u32_t c)
{
	u8_t * p, *q;
	u8_t * t;
	u32_t len, skip;
	u32_t i;
	u8_t fill = (u8_t) (c & 0xff);

	u32_t x = rect->x;
	u32_t y = rect->y;
	u32_t w = rect->w;
	u32_t h = rect->h;

	len = w;
	skip = render->pitch;
	t = (u8_t *) (render->pixels + y * render->pitch + x);
	p = q = (u8_t *) t;

	for (i = 0; i < w; i++)
		*t++ = fill;

	for (i = 1; i < h; i++)
	{
		q += skip;
		memcpy(q, p, len);
	}
}

static void sw_fill_2byte(struct render_t * render, struct rect_t * rect, u32_t c)
{
	u8_t * p, *q;
	u16_t * t;
	u32_t len, skip;
	u32_t i;
	u16_t fill = (u16_t)(c & 0xffff);

	u32_t x = rect->x;
	u32_t y = rect->y;
	u32_t w = rect->w;
	u32_t h = rect->h;

	len = 2 * w;
	skip = render->pitch;
	t = (u16_t *) (render->pixels + y * render->pitch + x * 2);
	p = q = (u8_t *) t;

	for (i = 0; i < w; i++)
		*t++ = fill;

	for (i = 1; i < h; i++)
	{
		q += skip;
		memcpy(q, p, len);
	}
}

static void sw_fill_3byte(struct render_t * render, struct rect_t * rect, u32_t c)
{
	u8_t * p, *q;
	u8_t * t;
	u32_t len, skip;
	u32_t i;
	u8_t fill0 = (u8_t) ((c >> 0) & 0xff);
	u8_t fill1 = (u8_t) ((c >> 8) & 0xff);
	u8_t fill2 = (u8_t) ((c >> 16) & 0xff);

	u32_t x = rect->x;
	u32_t y = rect->y;
	u32_t w = rect->w;
	u32_t h = rect->h;

	len = 3 * w;
	skip = render->pitch;
	t = (u8_t *) (render->pixels + y * render->pitch + x * 3);
	p = q = (u8_t *) t;

	for (i = 0; i < w; i++)
	{
#if (BYTE_ORDER == BIG_ENDIAN)
		*t++ = fill2;
		*t++ = fill1;
		*t++ = fill0;
#else
		*t++ = fill0;
		*t++ = fill1;
		*t++ = fill2;
#endif
	}

	for (i = 1; i < h; i++)
	{
		q += skip;
		memcpy(q, p, len);
	}
}

static void sw_fill_4byte(struct render_t * render, struct rect_t * rect, u32_t c)
{
	u8_t * p, *q;
	u32_t * t;
	u32_t len, skip;
	u32_t i;

	u32_t x = rect->x;
	u32_t y = rect->y;
	u32_t w = rect->w;
	u32_t h = rect->h;

	len = 4 * w;
	skip = render->pitch;
	t = (u32_t *) (render->pixels + y * render->pitch + x * 4);
	p = q = (u8_t *) t;

	for (i = 0; i < w; i++)
		*t++ = c;

	for (i = 1; i < h; i++)
	{
		q += skip;
		memcpy(q, p, len);
	}
}

void render_sw_fill(struct render_t * render, struct rect_t * rect, u32_t c)
{
	struct rect_t rclip, clipped;

	if(!render)
		return;

	rclip.x = 0;
	rclip.y = 0;
	rclip.w = render->width;
	rclip.h = render->height;

	if(rect)
	{
		if(!rect_intersect(rect, &rclip, &clipped))
			return;
		rect = &clipped;
	}
	else
		rect = &rclip;

	switch(render->format)
	{
	case PIXEL_FORMAT_ARGB32:
		sw_fill_4byte(render, rect, c);
		break;
	case PIXEL_FORMAT_RGB24:
		sw_fill_3byte(render, rect, c);
		break;
	case PIXEL_FORMAT_A8:
		sw_fill_1byte(render, rect, c);
		break;
	case PIXEL_FORMAT_A1:
		break;
	case PIXEL_FORMAT_RGB16_565:
		sw_fill_2byte(render, rect, c);
		break;
	default:
		break;
	}
}
