/*
 * kernel/xui/xui-colorpicker.c
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
static struct color_t trans_color = { 0x00, 0x00, 0x00, 0x00 };

int xui_colorpicker_ex(struct xui_context_t * ctx, struct color_t * c, int opt)
{
	struct region_t * r;
	struct point_t p0, p1;
	struct color_t t;
	float h, s, v, a;
	unsigned int id;
	int change = 0;
	int i;

	xui_push_id(ctx, &c, sizeof(struct color_t *));
	xui_layout_begin_column(ctx);
	xui_layout_row(ctx, 3, (int[]){ -32 - ctx->style.layout.padding * 3, 16, 16 }, -1);
	r = xui_layout_next(ctx);
	id = xui_get_id(ctx, "!svpicker", 9);
	xui_control_update(ctx, id, r, opt);
	if((ctx->active == id) && ((ctx->mouse.state & XUI_MOUSE_LEFT) || (ctx->mouse.down & XUI_MOUSE_LEFT)))
	{
		color_get_hsva(c, &h, &s, &v, &a);
		s = clamp(ctx->mouse.x - r->x, 0, r->w) / (float)r->w;
		v = (r->h - clamp(ctx->mouse.y - r->y, 0, r->h)) / (float)r->h;
		color_set_hsva(c, h, s, v, a);
		change = 1;
	}
	else
	{
		color_get_hsva(c, &h, &s, &v, &a);
	}
	color_set_hsva(&t, h, 1.0f, 1.0f, 1.0f);
	xui_draw_gradient(ctx, r->x, r->y, r->w, r->h, &white_color, &t, &black_color, &black_color);
	xui_draw_circle(ctx, roundf(r->x + r->w * s), roundf(r->y + r->h - r->h * v), 6, 2, &white_color);
	r = xui_layout_next(ctx);
	id = xui_get_id(ctx, "!hpicker", 8);
	xui_control_update(ctx, id, r, opt);
	if((ctx->active == id) && ((ctx->mouse.state & XUI_MOUSE_LEFT) || (ctx->mouse.down & XUI_MOUSE_LEFT)))
	{
		h = clamp(ctx->mouse.y - r->y, 0, r->h) / (float)r->h;
		color_set_hsva(c, h, s, v, a);
		color_set_hsva(&t, h, 1.0f, 1.0f, 1.0f);
		change = 1;
	}
	p0.x = r->x;
	p0.y = roundf(r->y + r->h * h);
	p1.x = r->x + r->w;
	p1.y = p0.y;
	for(i = 0; i < 6; i++)
	{
		xui_draw_gradient(ctx, r->x, roundf(r->y + i * (r->h / 6.0f)), r->w, roundf(r->h / 6.0f), &hue_color[i], &hue_color[i], &hue_color[i + 1], &hue_color[i + 1]);
	}
	xui_draw_line(ctx, &p0, &p1, 2, &white_color);
	r = xui_layout_next(ctx);
	id = xui_get_id(ctx, "!apicker", 8);
	xui_control_update(ctx, id, r, opt);
	if((ctx->active == id) && ((ctx->mouse.state & XUI_MOUSE_LEFT) || (ctx->mouse.down & XUI_MOUSE_LEFT)))
	{
		a = clamp(ctx->mouse.y - r->y, 0, r->h) / (float)r->h;
		c->a = roundf(a * 255.0f);
		change = 1;
	}
	p0.x = r->x;
	p0.y = roundf(r->y + r->h * a);
	p1.x = r->x + r->w;
	p1.y = p0.y;
	xui_draw_checkerboard(ctx, r->x, r->y, r->w, r->h);
	xui_draw_gradient(ctx, r->x, r->y, r->w, r->h, &trans_color, &trans_color, &(struct color_t){ c->r, c->g, c->b, 255 }, &(struct color_t){ c->r, c->g, c->b, 255 });
	xui_draw_line(ctx, &p0, &p1, 2, &white_color);
	xui_layout_end_column(ctx);
	xui_pop_id(ctx);

	return change;
}
