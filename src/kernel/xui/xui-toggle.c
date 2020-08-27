/*
 * kernel/xui/xui-toggle.c
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
#include <xui/toggle.h>

int xui_toggle_ex(struct xui_context_t * ctx, int * state, int opt)
{
	unsigned int id = xui_get_id(ctx, &state, sizeof(int *));
	int idx = xui_pool_get(ctx, ctx->spring_pool, XUI_COLLAPSE_POOL_SIZE, id);
	struct region_t * r = xui_layout_next(ctx);
	struct xui_widget_color_t * wc;
	struct color_t * bg, * fg, * bc;
	double alpha = 1;
	int click = 0;
	int radius, width;

	xui_control_update(ctx, id, r, opt);
	if((ctx->active == id) && (ctx->mouse.down & XUI_MOUSE_LEFT))
	{
		*state = !*state;
		click = 1;
	}
	radius = min(r->w, r->h) >> 1;
	width = ctx->style.toggle.border_width;
	if(*state)
	{
		switch(opt & (0x7 << 8))
		{
		case XUI_TOGGLE_PRIMARY:
			wc = &ctx->style.primary;
			break;
		case XUI_TOGGLE_SECONDARY:
			wc = &ctx->style.secondary;
			break;
		case XUI_TOGGLE_SUCCESS:
			wc = &ctx->style.success;
			break;
		case XUI_TOGGLE_INFO:
			wc = &ctx->style.info;
			break;
		case XUI_TOGGLE_WARNING:
			wc = &ctx->style.warning;
			break;
		case XUI_TOGGLE_DANGER:
			wc = &ctx->style.danger;
			break;
		case XUI_TOGGLE_LIGHT:
			wc = &ctx->style.light;
			break;
		case XUI_TOGGLE_DARK:
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
	if(idx >= 0)
	{
		xui_pool_update(ctx, ctx->spring_pool, idx);
		if(spring_step(&ctx->springs[idx], ctx->delta))
			alpha = spring_position(&ctx->springs[idx]);
		else
			memset(&ctx->spring_pool[idx], 0, sizeof(struct xui_pool_item_t));
	}
	if(ctx->active != ctx->oactive)
	{
		if(ctx->active == id)
		{
			if(idx < 0)
				idx = xui_pool_init(ctx, ctx->spring_pool, XUI_COLLAPSE_POOL_SIZE, id);
			spring_init(&ctx->springs[idx], 0, 1, 0, 618, 60);
		}
	}
	if(ctx->active == id)
	{
		bg = &wc->active.background;
		fg = &wc->active.foreground;
		bc = &wc->active.border;
	}
	else if(ctx->hover == id)
	{
		bg = &wc->hover.background;
		fg = &wc->hover.foreground;
		bc = &wc->hover.border;
	}
	else
	{
		bg = &wc->normal.background;
		fg = &wc->normal.foreground;
		bc = &wc->normal.border;
	}
	if(*state)
	{
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, r->x, r->y, radius << 2, radius << 1, radius, width, bc);
		if(bg->a)
			xui_draw_rectangle(ctx, r->x, r->y, radius << 2, radius << 1, radius, 0, bg);
		if(fg->a)
			xui_draw_circle(ctx, r->x + radius + (radius << 1) * alpha, r->y + radius, (radius * 7) >> 3, 0, fg);
	}
	else
	{
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, r->x, r->y, radius << 2, radius << 1, radius, width, bc);
		if(bg->a)
		{
			xui_draw_rectangle(ctx, r->x, r->y, radius << 2, radius << 1, radius, ctx->style.toggle.outline_width, bg);
			xui_draw_circle(ctx, r->x + radius + (radius << 1) * (1 - alpha), r->y + radius, (radius * 7) >> 3, 0, bg);
		}
	}
	return click;
}
