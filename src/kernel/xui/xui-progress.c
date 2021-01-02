/*
 * kernel/xui/xui-progress.c
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
#include <xui/progress.h>

void xui_progress_ex(struct xui_context_t * ctx, int percent, int opt)
{
	struct region_t region, * r = xui_layout_next(ctx);
	struct xui_widget_color_t * wc;
	struct color_t * bg, * fg, * c;
	int radius;

	percent = clamp(percent, 0, 100);
	radius = ctx->style.progress.border_radius;
	switch(opt & (0x7 << 8))
	{
	case XUI_PROGRESS_PRIMARY:
		wc = &ctx->style.primary;
		break;
	case XUI_PROGRESS_SECONDARY:
		wc = &ctx->style.secondary;
		break;
	case XUI_PROGRESS_SUCCESS:
		wc = &ctx->style.success;
		break;
	case XUI_PROGRESS_INFO:
		wc = &ctx->style.info;
		break;
	case XUI_PROGRESS_WARNING:
		wc = &ctx->style.warning;
		break;
	case XUI_PROGRESS_DANGER:
		wc = &ctx->style.danger;
		break;
	case XUI_PROGRESS_LIGHT:
		wc = &ctx->style.light;
		break;
	case XUI_PROGRESS_DARK:
		wc = &ctx->style.dark;
		break;
	default:
		wc = &ctx->style.primary;
		break;
	}
	bg = &wc->normal.background;
	fg = &wc->normal.foreground;
	c = &ctx->style.progress.invalid_color;
	if(opt & XUI_PROGRESS_VERTICAL)
	{
		region_init(&region, r->x, r->y + r->h * (100 - percent) / 100, r->w, r->h * percent / 100);
		if(c->a)
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, 0, c);
		if(bg->a)
			xui_draw_rectangle(ctx, region.x, region.y, region.w, region.h, (0x3 << 16) | radius, 0, bg);
	}
	else
	{
		region_init(&region, r->x, r->y, r->w * percent / 100, r->h);
		if(c->a)
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, 0, c);
		if(bg->a)
			xui_draw_rectangle(ctx, region.x, region.y, region.w, region.h, (0x6 << 16) | radius, 0, bg);
		if(fg->a && (region.h >= ctx->style.font.size))
			xui_control_draw_text(ctx, xui_format(ctx, "%d%%", percent), &region, fg, XUI_OPT_TEXT_CENTER);
	}
}
