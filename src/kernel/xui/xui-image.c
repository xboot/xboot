/*
 * kernel/xui/xui-image.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

int xui_image_ex(struct xui_context_t * ctx, struct surface_t * s, double angle, int opt)
{
	unsigned int id = xui_get_id(ctx, &s, sizeof(struct surface_t *));
	struct region_t * r = xui_layout_next(ctx);
	struct matrix_t m;
	double sx, sy;

	xui_control_update(ctx, id, r, opt);
	switch(opt & (0x3 << 12))
	{
	case XUI_IMAGE_NONE:
		sx = 1.0;
		sy = 1.0;
		break;
	case XUI_IMAGE_CONTAIN:
		sx = (double)r->w / (double)surface_get_width(s);
		sy = (double)r->h / (double)surface_get_height(s);
		if(sx >= sy)
			sx = sy;
		else
			sy = sx;
		break;
	case XUI_IMAGE_COVER:
		sx = (double)r->w / (double)surface_get_width(s);
		sy = (double)r->h / (double)surface_get_height(s);
		if(sx <= sy)
			sx = sy;
		else
			sy = sx;
		break;
	case XUI_IMAGE_FILL:
		sx = (double)r->w / (double)surface_get_width(s);
		sy = (double)r->h / (double)surface_get_height(s);
		break;
	default:
		sx = 1.0;
		sy = 1.0;
		break;
	}
	matrix_init_translate(&m, r->x + r->w / 2, r->y + r->h / 2);
	if(angle != 0.0)
		matrix_rotate(&m, angle);
	matrix_translate(&m, -surface_get_width(s) / 2 * sx, -surface_get_height(s) / 2 * sy);
	matrix_scale(&m, sx, sy);

	xui_push_clip(ctx, r);
	xui_draw_surface(ctx, s, &m, opt & XUI_IMAGE_REFRESH);
	xui_pop_clip(ctx);
	if(ctx->active == id)
	{
		xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 0, 0, &(struct color_t){ 0x7f, 0x7f, 0x7f, 0x7f });
		if(ctx->mouse.up & XUI_MOUSE_LEFT)
			return 1;
	}
	else if(ctx->hover == id)
	{
		xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, 0, 0, &(struct color_t){ 0x7f, 0x7f, 0x7f, 0x7f });
	}
	return 0;
}
