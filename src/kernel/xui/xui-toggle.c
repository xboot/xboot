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
	unsigned int id = xui_get_id(ctx, &state, sizeof(int *));
	struct region_t * r = xui_layout_next(ctx);
	struct xui_widget_color_t * wc;
	struct color_t * fc, * bc, * tc;
	int radius, width;
	int click = 0;

	xui_control_update(ctx, id, r, opt);
	if((ctx->focus == id) && (ctx->mouse.up & XUI_MOUSE_LEFT))
	{
		*state = !*state;
		click = 1;
	}
	radius = min(r->w, r->h) / 2;
	width = ctx->style.toggle.border_width;
	if(*state)
	{
		switch(opt & (0x7 << 8))
		{
		case XUI_CHECKBOX_PRIMARY:
			wc = &ctx->style.primary;
			break;
		case XUI_CHECKBOX_SECONDARY:
			wc = &ctx->style.secondary;
			break;
		case XUI_CHECKBOX_SUCCESS:
			wc = &ctx->style.success;
			break;
		case XUI_CHECKBOX_INFO:
			wc = &ctx->style.info;
			break;
		case XUI_CHECKBOX_WARNING:
			wc = &ctx->style.warning;
			break;
		case XUI_CHECKBOX_DANGER:
			wc = &ctx->style.danger;
			break;
		case XUI_CHECKBOX_LIGHT:
			wc = &ctx->style.light;
			break;
		case XUI_CHECKBOX_DARK:
			wc = &ctx->style.dark;
			break;
		default:
			wc = &ctx->style.primary;
			break;
		}
	}
	else
	{
		wc = &ctx->style.secondary;
	}
	if(ctx->focus == id)
	{
		fc = &wc->focus.face;
		bc = &wc->focus.border;
		tc = &wc->focus.text;
	}
	else if(ctx->hover == id)
	{
		fc = &wc->hover.face;
		bc = &wc->hover.border;
		tc = &wc->hover.text;
	}
	else
	{
		fc = &wc->normal.face;
		bc = &wc->normal.border;
		tc = &wc->normal.text;
	}
	if(*state)
	{
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, r->x, r->y, radius * 4, radius * 2, radius, width, bc);
		if(fc->a)
			xui_draw_rectangle(ctx, r->x, r->y, radius * 4, radius * 2, radius, 0, fc);
		if(tc->a)
			xui_draw_circle(ctx, r->x + radius * 3, r->y + radius, radius * 4 / 5, 0, tc);
	}
	else
	{
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, r->x, r->y, radius * 4, radius * 2, radius, width, bc);
		if(fc->a)
		{
			xui_draw_rectangle(ctx, r->x, r->y, radius * 4, radius * 2, radius, ctx->style.toggle.outline_width, fc);
			xui_draw_circle(ctx, r->x + radius, r->y + radius, radius * 4 / 5, 0, fc);
		}
	}
	return click;
}
