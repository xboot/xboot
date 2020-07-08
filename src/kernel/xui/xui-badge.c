/*
 * kernel/xui/xui-badge.c
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
#include <xui/badge.h>

void xui_badge_ex(struct xui_context_t * ctx, const char * label, int opt)
{
	struct region_t * r = xui_layout_next(ctx);
	struct xui_widget_color_t * wc;
	struct color_t * bg, * fg, * bc;
	struct text_t txt;
	const char * family = ctx->style.font.font_family;
	int size = ctx->style.font.size;
	int radius, width;
	int x, y, w, h;

	switch(opt & (0x7 << 8))
	{
	case XUI_BADGE_PRIMARY:
		wc = &ctx->style.primary;
		break;
	case XUI_BADGE_SECONDARY:
		wc = &ctx->style.secondary;
		break;
	case XUI_BADGE_SUCCESS:
		wc = &ctx->style.success;
		break;
	case XUI_BADGE_INFO:
		wc = &ctx->style.info;
		break;
	case XUI_BADGE_WARNING:
		wc = &ctx->style.warning;
		break;
	case XUI_BADGE_DANGER:
		wc = &ctx->style.danger;
		break;
	case XUI_BADGE_LIGHT:
		wc = &ctx->style.light;
		break;
	case XUI_BADGE_DARK:
		wc = &ctx->style.dark;
		break;
	default:
		wc = &ctx->style.primary;
		break;
	}
	bg = &wc->normal.background;
	fg = &wc->normal.foreground;
	bc = &wc->normal.border;
	text_init(&txt, label, fg, 0, ctx->f, family, size);
	w = txt.metrics.width + ctx->style.layout.padding * 2;
	h = txt.metrics.height + ctx->style.layout.padding * 2;
	if(w < h)
		w = h;
	x = r->x + (r->w - w) / 2;
	y = r->y + (r->h - h) / 2;
	if(opt & XUI_BADGE_ROUNDED)
		radius = h / 2;
	else
		radius = ctx->style.badge.border_radius;
	width = ctx->style.badge.border_width;

	xui_push_clip(ctx, r);
	if(opt & XUI_BADGE_OUTLINE)
	{
		if(bg->a)
		{
			xui_draw_rectangle(ctx, x, y, w, h, radius, ctx->style.badge.outline_width, bg);
			xui_draw_text(ctx, family, size, label, x + (w - txt.metrics.width) / 2, y + (h - txt.metrics.height) / 2, 0, bg);
		}
	}
	else
	{
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, x, y, w, h, radius, width, bc);
		if(bg->a)
			xui_draw_rectangle(ctx, x, y, w, h, radius, 0, bg);
		if(fg->a)
			xui_draw_text(ctx, family, size, label, x + (w - txt.metrics.width) / 2, y + (h - txt.metrics.height) / 2, 0, fg);
	}
	xui_pop_clip(ctx);
}
