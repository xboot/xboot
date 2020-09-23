/*
 * kernel/xui/xui-image.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <xui/image.h>

void xui_image_ex(struct xui_context_t * ctx, struct surface_t * s, double angle, int opt)
{
	struct region_t * r = xui_layout_next(ctx);
	struct matrix_t m;
	double sx, sy;

	switch(opt & (0x3 << 12))
	{
	case XUI_IMAGE_NONE:
		if(angle != 0.0)
		{
			matrix_init_translate(&m, r->x + r->w / 2, r->y + r->h / 2);
			matrix_rotate(&m, angle);
			matrix_translate(&m, -surface_get_width(s) / 2, -surface_get_height(s) / 2);
		}
		else
		{
			matrix_init_translate(&m, r->x + (r->w - surface_get_width(s)) / 2, r->y + (r->h - surface_get_height(s)) / 2);
		}
		break;
	case XUI_IMAGE_CONTAIN:
		sx = (double)r->w / (double)surface_get_width(s);
		sy = (double)r->h / (double)surface_get_height(s);
		if(sx >= sy)
			sx = sy;
		else
			sy = sx;
		matrix_init_translate(&m, r->x + r->w / 2, r->y + r->h / 2);
		if(angle != 0.0)
			matrix_rotate(&m, angle);
		matrix_translate(&m, -surface_get_width(s) / 2 * sx, -surface_get_height(s) / 2 * sy);
		matrix_scale(&m, sx, sy);
		break;
	case XUI_IMAGE_COVER:
		sx = (double)r->w / (double)surface_get_width(s);
		sy = (double)r->h / (double)surface_get_height(s);
		if(sx <= sy)
			sx = sy;
		else
			sy = sx;
		matrix_init_translate(&m, r->x + r->w / 2, r->y + r->h / 2);
		if(angle != 0.0)
			matrix_rotate(&m, angle);
		matrix_translate(&m, -surface_get_width(s) / 2 * sx, -surface_get_height(s) / 2 * sy);
		matrix_scale(&m, sx, sy);
		break;
	case XUI_IMAGE_FILL:
		sx = (double)r->w / (double)surface_get_width(s);
		sy = (double)r->h / (double)surface_get_height(s);
		matrix_init_translate(&m, r->x + r->w / 2, r->y + r->h / 2);
		if(angle != 0.0)
			matrix_rotate(&m, angle);
		matrix_translate(&m, -surface_get_width(s) / 2 * sx, -surface_get_height(s) / 2 * sy);
		matrix_scale(&m, sx, sy);
		break;
	default:
		break;
	}
	xui_push_clip(ctx, r);
	xui_draw_surface(ctx, s, &m, opt & XUI_IMAGE_REFRESH);
	xui_pop_clip(ctx);
}
