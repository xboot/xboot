/*
 * kernel/xui/xui-button.c
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
#include <xui/button.h>

int xui_button_ex(struct xui_context_t * ctx, int icon, const char * label, int opt)
{
	unsigned int id = label ? xui_get_id(ctx, label, strlen(label)) : xui_get_id(ctx, &icon, sizeof(int));
	struct region_t r;
	struct xui_widget_color_t * wc;
	struct color_t * bg, * fg, * bc;
	int radius, width;

	region_clone(&r, xui_layout_next(ctx));
	xui_control_update(ctx, id, &r, opt);
	if(opt & XUI_BUTTON_ROUNDED)
		radius = (r.h >> 1);
	else
		radius = ctx->style.button.border_radius;
	width = ctx->style.button.border_width;
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
	case XUI_BUTTON_LIGHT:
		wc = &ctx->style.light;
		break;
	case XUI_BUTTON_DARK:
		wc = &ctx->style.dark;
		break;
	default:
		wc = &ctx->style.primary;
		break;
	}

	if(ctx->active == id)
	{
		bg = &wc->active.background;
		fg = &wc->active.foreground;
		bc = &wc->active.border;
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, radius, width, bc);
		if(bg->a)
			xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, radius, 0, bg);
		if(fg->a)
		{
			if((icon > 0) && label)
			{
				xui_draw_icon(ctx, ctx->style.font.icon_family, icon, r.x, r.y, r.h, r.h, fg);
				r.x += r.h - ctx->style.layout.padding;
				r.w -= r.h - ctx->style.layout.padding;
				xui_control_draw_text(ctx, label, &r, fg, opt);
			}
			else if(label)
				xui_control_draw_text(ctx, label, &r, fg, opt);
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
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, radius, width, bc);
		if(bg->a)
			xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, radius, 0, bg);
		if(fg->a)
		{
			if((icon > 0) && label)
			{
				xui_draw_icon(ctx, ctx->style.font.icon_family, icon, r.x, r.y, r.h, r.h, fg);
				r.x += r.h - ctx->style.layout.padding;
				r.w -= r.h - ctx->style.layout.padding;
				xui_control_draw_text(ctx, label, &r, fg, opt);
			}
			else if(label)
				xui_control_draw_text(ctx, label, &r, fg, opt);
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
				xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, radius, ctx->style.button.outline_width, bg);
				if((icon > 0) && label)
				{
					xui_draw_icon(ctx, ctx->style.font.icon_family, icon, r.x, r.y, r.h, r.h, bg);
					r.x += r.h - ctx->style.layout.padding;
					r.w -= r.h - ctx->style.layout.padding;
					xui_control_draw_text(ctx, label, &r, bg, opt);
				}
				else if(label)
					xui_control_draw_text(ctx, label, &r, bg, opt);
				else
					xui_draw_icon(ctx, ctx->style.font.icon_family, icon, r.x, r.y, r.w, r.h, bg);
			}
		}
		else
		{
			if(bc->a && (width > 0))
				xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, radius, width, bc);
			if(bg->a)
				xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, radius, 0, bg);
			if(fg->a)
			{
				if((icon > 0) && label)
				{
					xui_draw_icon(ctx, ctx->style.font.icon_family, icon, r.x, r.y, r.h, r.h, fg);
					r.x += r.h - ctx->style.layout.padding;
					r.w -= r.h - ctx->style.layout.padding;
					xui_control_draw_text(ctx, label, &r, fg, opt);
				}
				else if(label)
					xui_control_draw_text(ctx, label, &r, fg, opt);
				else
					xui_draw_icon(ctx, ctx->style.font.icon_family, icon, r.x, r.y, r.w, r.h, fg);
			}
		}
	}
	return 0;
}
