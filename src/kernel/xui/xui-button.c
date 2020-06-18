/*
 * kernel/xui/xui-button.c
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
#include <xui/button.h>

int xui_button_ex(struct xui_context_t * ctx, const char * label, int icon, int opt)
{
	unsigned int id = label ? xui_get_id(ctx, label, strlen(label)) : xui_get_id(ctx, &icon, sizeof(int));
	struct region_t * r = xui_layout_next(ctx);
	struct xui_widget_color_t * wc;
	struct color_t * fc, * bc, * tc;
	int radius, width;

	xui_control_update(ctx, id, r, opt);
	if(opt & XUI_BUTTON_ROUNDED)
		radius = r->h / 2;
	else
		radius = ctx->style.common.border_radius;
	width = ctx->style.common.border_width;
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
	if(ctx->focus == id)
	{
		fc = &wc->focus.face;
		bc = &wc->focus.border;
		tc = &wc->focus.text;
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, width, bc);
		if(fc->a)
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, 0, fc);
		if(tc->a)
		{
			if(label)
				xui_control_draw_text(ctx, label, r, tc, opt);
			else
				xui_draw_icon(ctx, ctx->style.common.icon_family, icon, r->x, r->y, r->w, r->h, tc);
		}
		if(ctx->mouse.up & XUI_MOUSE_LEFT)
			return 1;
	}
	else if(ctx->hover == id)
	{
		fc = &wc->hover.face;
		bc = &wc->hover.border;
		tc = &wc->hover.text;
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, width, bc);
		if(fc->a)
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, 0, fc);
		if(tc->a)
		{
			if(label)
				xui_control_draw_text(ctx, label, r, tc, opt);
			else
				xui_draw_icon(ctx, ctx->style.common.icon_family, icon, r->x, r->y, r->w, r->h, tc);
		}
	}
	else
	{
		fc = &wc->normal.face;
		bc = &wc->normal.border;
		tc = &wc->normal.text;
		if(opt & XUI_BUTTON_OUTLINE)
		{
			if(fc->a)
			{
				xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, ctx->style.common.outline_width, fc);
				if(label)
					xui_control_draw_text(ctx, label, r, fc, opt);
				else
					xui_draw_icon(ctx, ctx->style.common.icon_family, icon, r->x, r->y, r->w, r->h, fc);
			}
		}
		else
		{
			if(bc->a && (width > 0))
				xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, width, bc);
			if(fc->a)
				xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, 0, fc);
			if(tc->a)
			{
				if(label)
					xui_control_draw_text(ctx, label, r, tc, opt);
				else
					xui_draw_icon(ctx, ctx->style.common.icon_family, icon, r->x, r->y, r->w, r->h, tc);
			}
		}
	}
	return 0;
}
