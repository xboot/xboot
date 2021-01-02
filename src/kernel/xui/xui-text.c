/*
 * kernel/xui/xui-text.c
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
#include <xui/text.h>

void xui_text(struct xui_context_t * ctx, const char * utf8)
{
	const char * family = ctx->style.font.font_family;
	struct color_t * c = &ctx->style.font.color;
	struct region_t * r;
	struct text_t txt;
	int size = ctx->style.font.size;
	int wrap = xui_get_layout(ctx)->body.w;

	text_init(&txt, utf8, c, wrap, ctx->f, family, size);
	xui_layout_row(ctx, 1, (int[]){ -1 }, txt.metrics.height);
	r = xui_layout_next(ctx);
	xui_draw_text(ctx, family, size, utf8, r->x, r->y, wrap, c);
}
