/*
 * kernel/xui/xui-textedit.c
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
#include <xui/textedit.h>

int xui_textedit_ex(struct xui_context_t * ctx, char * buf, int size, int opt)
{
	unsigned int id = xui_get_id(ctx, &buf, sizeof(char *));
	struct region_t * r = xui_layout_next(ctx);
	struct xui_widget_color_t * wc;
	struct color_t * bg, * fg, * bc;
	int radius, width;
	int change = 0;

	xui_control_update(ctx, id, r, opt | XUI_OPT_HOLDFOCUS);
	if(opt & XUI_TEXTEDIT_ROUNDED)
		radius = r->h / 2;
	else
		radius = ctx->style.textedit.border_radius;
	width = ctx->style.textedit.border_width;
	switch(opt & (0x7 << 8))
	{
	case XUI_TEXTEDIT_PRIMARY:
		wc = &ctx->style.primary;
		break;
	case XUI_TEXTEDIT_SECONDARY:
		wc = &ctx->style.secondary;
		break;
	case XUI_TEXTEDIT_SUCCESS:
		wc = &ctx->style.success;
		break;
	case XUI_TEXTEDIT_INFO:
		wc = &ctx->style.info;
		break;
	case XUI_TEXTEDIT_WARNING:
		wc = &ctx->style.warning;
		break;
	case XUI_TEXTEDIT_DANGER:
		wc = &ctx->style.danger;
		break;
	case XUI_TEXTEDIT_LIGHT:
		wc = &ctx->style.light;
		break;
	case XUI_TEXTEDIT_DARK:
		wc = &ctx->style.dark;
		break;
	default:
		wc = &ctx->style.primary;
		break;
	}
	if(ctx->active == id)
	{
		int len = strlen(buf);
		int n = min(size - len - 1, (int)strlen(ctx->input_text));
		if(n > 0)
		{
			memcpy(buf + len, ctx->input_text, n);
			len += n;
			buf[len] = 0;
			change |= (1 << 0);
		}
		if((ctx->key_pressed & XUI_KEY_BACK) && (len > 0))
		{
			while(((buf[--len] & 0xc0) == 0x80) && (len > 0));
			buf[len] = 0;
			change |= (1 << 0);
		}
		if(ctx->key_pressed & XUI_KEY_ENTER)
			change |= (1 << 1);
		bg = &wc->active.background;
		fg = &wc->active.foreground;
		bc = &wc->active.border;
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, width, bc);
		if(bg->a)
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, 0, bg);
		if(fg->a)
		{
			const char * family = ctx->style.font.font_family;
			int size = ctx->style.font.size;
			struct text_t txt;
			text_init(&txt, buf, fg, 0, ctx->f, family, size);
			int textw = txt.metrics.width;
			int texth = txt.metrics.height;
			int ofx = r->w - ctx->style.layout.padding - textw - 1;
			int textx = r->x + min(ofx, ctx->style.layout.padding);
			int texty = r->y + (r->h - texth) / 2;
			xui_push_clip(ctx, r);
			xui_draw_text(ctx, family, size, buf, textx, texty, 0, fg);
			xui_draw_rectangle(ctx, textx + textw, r->y, 2, r->h, 0, 0, fg);
			xui_pop_clip(ctx);
		}
	}
	else if(ctx->hover == id)
	{
		bg = &wc->hover.background;
		fg = &wc->hover.foreground;
		bc = &wc->hover.border;
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, width, bc);
		if(bg->a)
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, 0, bg);
		if(fg->a)
			xui_control_draw_text(ctx, buf, r, fg, opt);
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
				xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, ctx->style.textedit.outline_width, bg);
				xui_control_draw_text(ctx, buf, r, bg, opt);
			}
		}
		else
		{
			if(bc->a && (width > 0))
				xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, width, bc);
			if(bg->a)
				xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, 0, bg);
			if(fg->a)
				xui_control_draw_text(ctx, buf, r, fg, opt);
		}
	}
	return change;
}
