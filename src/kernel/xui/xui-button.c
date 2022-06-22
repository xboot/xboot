/*
 * kernel/xui/xui-button.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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
#include <xui/button.h>

int xui_button_ex(struct xui_context_t * ctx, int icon, const char * label, int opt)
{
	unsigned int id = label ? xui_get_id(ctx, label, strlen(label)) : xui_get_id(ctx, &icon, sizeof(int));
	int idx = xui_pool_get(ctx, ctx->spring_pool, XUI_COLLAPSE_POOL_SIZE, id);
	struct region_t r;
	struct xui_widget_color_t * wc;
	struct color_t * bg, * fg, * bc;
	double alpha = 0;
	int br, bw;

	region_clone(&r, xui_layout_next(ctx));
	xui_control_update(ctx, id, &r, opt);
	if(opt & XUI_BUTTON_ROUNDED)
		br = (r.h >> 1);
	else
		br = ctx->style.button.border_radius;
	bw = ctx->style.button.border_width;
	switch(opt & (0x7 << 8))
	{
	case XUI_BUTTON_PRIMARY:
		wc = &ctx->style.primary;
		break;
	case XUI_BUTTON_SECONDARY:
		wc = &ctx->style.secondary;
		break;
	case XUI_BUTTON_SUCCESS:
		wc = &ctx->style.success;
		break;
	case XUI_BUTTON_INFO:
		wc = &ctx->style.info;
		break;
	case XUI_BUTTON_WARNING:
		wc = &ctx->style.warning;
		break;
	case XUI_BUTTON_DANGER:
		wc = &ctx->style.danger;
		break;
	default:
		wc = &ctx->style.primary;
		break;
	}
	if(idx >= 0)
	{
		xui_pool_update(ctx, ctx->spring_pool, idx);
		if(spring_step(&ctx->springs[idx], ktime_to_ns(ctx->delta) / 1000000000.0f))
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
		if(bc->a && (bw > 0))
			xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, br, bw, bc);
		if(bg->a)
			xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, br, 0, bg);
		if(alpha > 0)
		{
			int a = max(ctx->mouse.ox - r.x, r.x + r.w - ctx->mouse.ox);
			int b = max(ctx->mouse.oy - r.y, r.y + r.h - ctx->mouse.oy);
			int l  = pow(a * a + b * b, 0.5) * alpha;
			xui_draw_ripple(ctx, &(struct mask_t){r.x, r.y, r.w, r.h, br}, ctx->mouse.ox, ctx->mouse.oy, l, 0, &(struct color_t ){255, 255, 255, 51});
		}
		if(fg->a)
		{
			if((icon > 0) && label)
			{
				xui_draw_icon(ctx, ctx->style.font.icon_family, icon, r.x, r.y, r.h, r.h, fg);
				r.x += r.h - ctx->style.layout.padding;
				r.w -= r.h - ctx->style.layout.padding;
				xui_draw_text_align(ctx, ctx->style.font.font_family, ctx->style.font.size, label, &r, 0, fg, opt);
			}
			else if(label)
				xui_draw_text_align(ctx, ctx->style.font.font_family, ctx->style.font.size, label, &r, 0, fg, opt);
			else
				xui_draw_icon(ctx, ctx->style.font.icon_family, icon, r.x, r.y, r.w, r.h, fg);
		}
		if(ctx->mouse.up & XUI_MOUSE_LEFT)
			return 1;
	}
	else if(ctx->hover == id)
	{
		bg = &wc->hover.background;
		fg = &wc->hover.foreground;
		bc = &wc->hover.border;
		if(bc->a && (bw > 0))
			xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, br, bw, bc);
		if(bg->a)
			xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, br, 0, bg);
		if(fg->a)
		{
			if((icon > 0) && label)
			{
				xui_draw_icon(ctx, ctx->style.font.icon_family, icon, r.x, r.y, r.h, r.h, fg);
				r.x += r.h - ctx->style.layout.padding;
				r.w -= r.h - ctx->style.layout.padding;
				xui_draw_text_align(ctx, ctx->style.font.font_family, ctx->style.font.size, label, &r, 0, fg, opt);
			}
			else if(label)
				xui_draw_text_align(ctx, ctx->style.font.font_family, ctx->style.font.size, label, &r, 0, fg, opt);
			else
				xui_draw_icon(ctx, ctx->style.font.icon_family, icon, r.x, r.y, r.w, r.h, fg);
		}
	}
	else
	{
		bg = &wc->normal.background;
		fg = &wc->normal.foreground;
		bc = &wc->normal.border;
		if(opt & XUI_BUTTON_OUTLINE)
		{
			if(bg->a)
			{
				xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, br, ctx->style.button.outline_width, bg);
				if((icon > 0) && label)
				{
					xui_draw_icon(ctx, ctx->style.font.icon_family, icon, r.x, r.y, r.h, r.h, bg);
					r.x += r.h - ctx->style.layout.padding;
					r.w -= r.h - ctx->style.layout.padding;
					xui_draw_text_align(ctx, ctx->style.font.font_family, ctx->style.font.size, label, &r, 0, bg, opt);
				}
				else if(label)
					xui_draw_text_align(ctx, ctx->style.font.font_family, ctx->style.font.size, label, &r, 0, bg, opt);
				else
					xui_draw_icon(ctx, ctx->style.font.icon_family, icon, r.x, r.y, r.w, r.h, bg);
			}
		}
		else
		{
			if(bc->a && (bw > 0))
				xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, br, bw, bc);
			if(bg->a)
				xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, br, 0, bg);
			if(fg->a)
			{
				if((icon > 0) && label)
				{
					xui_draw_icon(ctx, ctx->style.font.icon_family, icon, r.x, r.y, r.h, r.h, fg);
					r.x += r.h - ctx->style.layout.padding;
					r.w -= r.h - ctx->style.layout.padding;
					xui_draw_text_align(ctx, ctx->style.font.font_family, ctx->style.font.size, label, &r, 0, fg, opt);
				}
				else if(label)
					xui_draw_text_align(ctx, ctx->style.font.font_family, ctx->style.font.size, label, &r, 0, fg, opt);
				else
					xui_draw_icon(ctx, ctx->style.font.icon_family, icon, r.x, r.y, r.w, r.h, fg);
			}
		}
	}
	return 0;
}
