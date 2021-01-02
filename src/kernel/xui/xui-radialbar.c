/*
 * kernel/xui/xui-radialbar.c
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
#include <xui/radialbar.h>

void xui_radialbar_ex(struct xui_context_t * ctx, int percent, int opt)
{
	struct region_t region, * r = xui_layout_next(ctx);
	struct xui_widget_color_t * wc;
	struct color_t * bg, * fg, * c;
	int width, radius;
	int x, y;

	percent = clamp(percent, 0, 100);
	switch(opt & (0x7 << 8))
	{
	case XUI_RADIALBAR_PRIMARY:
		wc = &ctx->style.primary;
		break;
	case XUI_RADIALBAR_SECONDARY:
		wc = &ctx->style.secondary;
		break;
	case XUI_RADIALBAR_SUCCESS:
		wc = &ctx->style.success;
		break;
	case XUI_RADIALBAR_INFO:
		wc = &ctx->style.info;
		break;
	case XUI_RADIALBAR_WARNING:
		wc = &ctx->style.warning;
		break;
	case XUI_RADIALBAR_DANGER:
		wc = &ctx->style.danger;
		break;
	case XUI_RADIALBAR_LIGHT:
		wc = &ctx->style.light;
		break;
	case XUI_RADIALBAR_DARK:
		wc = &ctx->style.dark;
		break;
	default:
		wc = &ctx->style.primary;
		break;
	}
	bg = &wc->normal.background;
	fg = &ctx->style.font.color;
	c = &ctx->style.radialbar.invalid_color;
	width = ctx->style.radialbar.width;
	radius = (min(r->w, r->h) - width - ctx->style.layout.padding * 2) / 2;
	x = r->x + r->w / 2;
	y = r->y + r->h / 2;
	if(radius > width)
	{
		region_init(&region, x - radius + width, y - radius + width, (radius - width) * 2, (radius - width) * 2);
		if(c->a)
			xui_draw_circle(ctx, x, y, radius, width, c);
		if(bg->a)
			xui_draw_arc(ctx, x, y, radius, 270, 360 * percent / 100 + 270, width, bg);
		if(fg->a && (region.h >= ctx->style.font.size))
			xui_control_draw_text(ctx, xui_format(ctx, "%d%%", percent), &region, fg, XUI_OPT_TEXT_CENTER);
	}
}
