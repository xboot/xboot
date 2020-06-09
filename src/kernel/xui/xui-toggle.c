/*
 * kernel/xui/xui-toggle.c
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
#include <xui/toggle.h>

int xui_toggle_ex(struct xui_context_t * ctx, int * state, int opt)
{
	unsigned int id = xui_get_id(ctx, &state, sizeof(state));
	struct region_t region, * r = xui_layout_next(ctx);
	struct xui_style_toggle_t * sc;
	struct color_t * fc, * bc, * ic, * tc;
	int radius, width;
	int res = 0;

/*	xui_control_update(ctx, id, r, opt);
	if((ctx->mouse_pressed & XUI_MOUSE_LEFT) && (ctx->focus == id))
	{
		*state = !*state;
		res = 1;
	}
	radius = ctx->style.toggle.border_radius;
	width = ctx->style.toggle.border_width;
	if(*state)
		sc = &ctx->style.toggle.checked;
	else
		sc = &ctx->style.toggle.unchecked;
	if(ctx->focus == id)
	{
		fc = &sc->focus.face_color;
		bc = &sc->focus.border_color;
		ic = &sc->focus.icon_color;
		tc = &sc->focus.text_color;
	}
	else if(ctx->hover == id)
	{
		fc = &sc->hover.face_color;
		bc = &sc->hover.border_color;
		ic = &sc->hover.icon_color;
		tc = &sc->hover.text_color;
	}
	else
	{
		fc = &sc->normal.face_color;
		bc = &sc->normal.border_color;
		ic = &sc->normal.icon_color;
		tc = &sc->normal.text_color;
	}
	if(bc->a && (width > 0))
		xui_draw_rectangle(ctx, r->x, r->y, r->h, r->h, radius, width, bc);
	if(fc->a)
		xui_draw_rectangle(ctx, r->x, r->y, r->h, r->h, radius, *state ? 0 : 2, fc);
	if(*state)
		xui_draw_icon(ctx, ctx->style.icon_family, ctx->style.toggle.check_icon, r->x, r->y, r->h, r->h, ic);*/
	return res;
}
