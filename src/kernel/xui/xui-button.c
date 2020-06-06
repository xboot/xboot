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
	unsigned int id = label ? xui_get_id(ctx, label, strlen(label)) : xui_get_id(ctx, &icon, sizeof(icon));
	struct region_t * r = xui_layout_next(ctx);
	struct xui_style_button_t * sb;
	struct color_t * fc, * bc, * tc;
	int radius, width;

	xui_control_update(ctx, id, r, opt);
	if(opt & XUI_BUTTON_ROUNDED)
		radius = r->h / 2;
	else
		radius = ctx->style.button.border_radius;
	width = ctx->style.button.border_width;
	switch(opt & (0x7 << 8))
	{
	case XUI_BUTTON_PRIMARY:
		sb = &ctx->style.button.primary;
		break;
	case XUI_BUTTON_SECONDARY:
		sb = &ctx->style.button.secondary;
		break;
	case XUI_BUTTON_SUCCESS:
		sb = &ctx->style.button.success;
		break;
	case XUI_BUTTON_INFO:
		sb = &ctx->style.button.info;
		break;
	case XUI_BUTTON_WARNING:
		sb = &ctx->style.button.warning;
		break;
	case XUI_BUTTON_DANGER:
		sb = &ctx->style.button.danger;
		break;
	case XUI_BUTTON_LIGHT:
		sb = &ctx->style.button.light;
		break;
	case XUI_BUTTON_DARK:
		sb = &ctx->style.button.dark;
		break;
	default:
		sb = &ctx->style.button.primary;
		break;
	}
	if(ctx->focus == id)
	{
		fc = &sb->focus.face_color;
		bc = &sb->focus.border_color;
		tc = &sb->focus.text_color;
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, width, bc);
		if(fc->a)
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, 0, fc);
		if(tc->a)
		{
			if(label)
				xui_control_draw_text(ctx, label, r, tc, opt);
			else
				xui_draw_icon(ctx, ctx->style.icon_family, icon, r->x, r->y, r->w, r->h, tc);
		}
		if(ctx->mouse_pressed & XUI_MOUSE_LEFT)
			return 1;
	}
	else if(ctx->hover == id)
	{
		fc = &sb->hover.face_color;
		bc = &sb->hover.border_color;
		tc = &sb->hover.text_color;
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, width, bc);
		if(fc->a)
			xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, 0, fc);
		if(tc->a)
		{
			if(label)
				xui_control_draw_text(ctx, label, r, tc, opt);
			else
				xui_draw_icon(ctx, ctx->style.icon_family, icon, r->x, r->y, r->w, r->h, tc);
		}
	}
	else
	{
		fc = &sb->normal.face_color;
		bc = &sb->normal.border_color;
		tc = &sb->normal.text_color;
		if(opt & XUI_BUTTON_OUTLINE)
		{
			if(fc->a)
			{
				xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, ctx->style.button.outline_width, fc);
				if(label)
					xui_control_draw_text(ctx, label, r, fc, opt);
				else
					xui_draw_icon(ctx, ctx->style.icon_family, icon, r->x, r->y, r->w, r->h, fc);
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
					xui_draw_icon(ctx, ctx->style.icon_family, icon, r->x, r->y, r->w, r->h, tc);
			}
		}
	}
	return 0;
}
