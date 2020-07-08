/*
 * kernel/xui/xui-split.c
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
#include <xui/split.h>

void xui_split_ex(struct xui_context_t * ctx, int opt)
{
	struct region_t * r = xui_layout_next(ctx);
	struct xui_widget_color_t * wc;
	struct color_t * bg;
	struct point_t p0, p1;

	switch(opt & (0x7 << 8))
	{
	case XUI_SPLIT_PRIMARY:
		wc = &ctx->style.primary;
		break;
	case XUI_SPLIT_SECONDARY:
		wc = &ctx->style.secondary;
		break;
	case XUI_SPLIT_SUCCESS:
		wc = &ctx->style.success;
		break;
	case XUI_SPLIT_INFO:
		wc = &ctx->style.info;
		break;
	case XUI_SPLIT_WARNING:
		wc = &ctx->style.warning;
		break;
	case XUI_SPLIT_DANGER:
		wc = &ctx->style.danger;
		break;
	case XUI_SPLIT_LIGHT:
		wc = &ctx->style.light;
		break;
	case XUI_SPLIT_DARK:
		wc = &ctx->style.dark;
		break;
	default:
		wc = &ctx->style.primary;
		break;
	}
	bg = &wc->normal.background;
	if(opt & XUI_SPLIT_VERTICAL)
	{
		p0.x = r->x + r->w / 2;
		p0.y = r->y;
		p1.x = p0.x;
		p1.y = r->y + r->h;
	}
	else
	{
		p0.x = r->x;
		p0.y = r->y + r->h / 2;
		p1.x = r->x + r->w;
		p1.y = p0.y;
	}
	if(bg->a)
		xui_draw_line(ctx, &p0, &p1, ctx->style.split.width, bg);
}
