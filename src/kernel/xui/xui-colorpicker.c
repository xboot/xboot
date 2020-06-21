/*
 * kernel/xui/xui-colorpicker.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
#include <xui/colorpicker.h>

static struct color_t hue_color[] = {
	{ 0xff, 0x00, 0x00, 0xff },
	{ 0xff, 0xff, 0x00, 0xff },
	{ 0x00, 0xff, 0x00, 0xff },
	{ 0x00, 0xff, 0xff, 0xff },
	{ 0x00, 0x00, 0xff, 0xff },
	{ 0xff, 0x00, 0xff, 0xff },
	{ 0xff, 0x00, 0x00, 0xff },
};

static struct color_t white_color = { 0xff, 0xff, 0xff, 0xff };
static struct color_t black_color = { 0x00, 0x00, 0x00, 0xff };
static struct color_t black_tran_color = { 0x00, 0x00, 0x00, 0x00 };

int xui_colorpicker_ex(struct xui_context_t * ctx, struct color_t * c, int opt)
{
	struct region_t * r;
	struct color_t t;
	int h, s, v;
	int i;

	color_get_hsv(c, &h, &s, &v);
	color_set_hsv(&t, h, 100, 100);

	xui_layout_begin_column(ctx);
	xui_layout_row(ctx, 3, (int[]){ -40, 20, 20 }, 100);
	r = xui_layout_next(ctx);
	xui_draw_gradient(ctx, r->x, r->y, r->w, r->h, &white_color, &t, &black_color, &black_color);
//	xui_draw_gradient(ctx, r->x, r->y, r->w, r->h, &black_tran_color, &black_tran_color, &black_color, &black_color);
	r = xui_layout_next(ctx);
	for(i = 0; i < 6; ++i)
	{
		xui_draw_gradient(ctx, r->x, r->y + (r->h / 6) * i, r->w, r->h / 6, &hue_color[i], &hue_color[i], &hue_color[i + 1], &hue_color[i + 1]);
	}
	r = xui_layout_next(ctx);
	xui_draw_gradient(ctx, r->x, r->y, r->w, r->h, &white_color, &white_color, &black_color, &black_color);
	xui_layout_end_column(ctx);
	return 0;
}
