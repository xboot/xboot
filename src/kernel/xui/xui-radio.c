/*
 * kernel/xui/xui-radio.c
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
#include <xui/radio.h>

int xui_radio_ex(struct xui_context_t * ctx, const char * label, int active, int opt)
{
	unsigned int id = xui_get_id(ctx, label, strlen(label));
	struct region_t region, * r = xui_layout_next(ctx);
	struct xui_style_radio_t * sr;
	struct color_t * fc, * bc, * ic, * tc;
	int radius, width;
	int res = 0;

	xui_control_update(ctx, id, r, opt);
	if((ctx->mouse_pressed & XUI_MOUSE_LEFT) && (ctx->focus == id))
	{
		res = 1;
	}
	radius = r->h / 2;
	width = ctx->style.radio.border_width;
	if(active)
		sr = &ctx->style.radio.checked;
	else
		sr = &ctx->style.radio.unchecked;
	if(ctx->focus == id)
	{
		fc = &sr->focus.face_color;
		bc = &sr->focus.border_color;
		ic = &sr->focus.icon_color;
		tc = &sr->focus.text_color;
	}
	else if(ctx->hover == id)
	{
		fc = &sr->hover.face_color;
		bc = &sr->hover.border_color;
		ic = &sr->hover.icon_color;
		tc = &sr->hover.text_color;
	}
	else
	{
		fc = &sr->normal.face_color;
		bc = &sr->normal.border_color;
		ic = &sr->normal.icon_color;
		tc = &sr->normal.text_color;
	}
	if(bc->a && (width > 0))
		xui_draw_circle(ctx, r->x + radius, r->y + radius, radius, width, bc);
	if(fc->a)
		xui_draw_circle(ctx, r->x + radius, r->y + radius, radius, active ? 0 : 2, fc);
	if(active)
		xui_draw_circle(ctx, r->x + radius, r->y + radius, radius * 392 / 1000, 0, ic);
	if(label && tc->a)
	{
		region_init(&region, r->x + r->h, r->y, r->w - r->h, r->h);
		xui_control_draw_text(ctx, label, &region, tc, opt);
	}
	return res;
}
