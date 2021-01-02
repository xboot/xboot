/*
 * kernel/xui/xui-tree.c
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
#include <xui/tree.h>

int xui_begin_tree_ex(struct xui_context_t * ctx, const char * label, int opt)
{
	unsigned int id = xui_get_id(ctx, label, strlen(label));
	int idx = xui_pool_get(ctx, ctx->tree_pool, XUI_TREE_POOL_SIZE, id);
	struct region_t r;
	struct xui_widget_color_t * wc;
	struct color_t * bg, * fg, * bc;
	int radius, width;
	int active, expanded;

	xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
	region_clone(&r, xui_layout_next(ctx));
	xui_control_update(ctx, id, &r, 0);
	active = (idx >= 0);
	expanded = (opt & XUI_TREE_EXPANDED) ? !active : active;
	active ^= ((ctx->active == id) && (ctx->mouse.down & XUI_MOUSE_LEFT));
	if(idx >= 0)
	{
		if(active)
			xui_pool_update(ctx, ctx->tree_pool, idx);
		else
			memset(&ctx->tree_pool[idx], 0, sizeof(struct xui_pool_item_t));
	}
	else if(active)
	{
		xui_pool_init(ctx, ctx->tree_pool, XUI_TREE_POOL_SIZE, id);
	}
	radius = ctx->style.tree.border_radius;
	width = ctx->style.tree.border_width;
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
			xui_draw_icon(ctx, ctx->style.font.icon_family, expanded ? ctx->style.tree.expanded_icon : ctx->style.tree.collapsed_icon, r.x, r.y, r.h, r.h, fg);
			r.x += r.h - ctx->style.layout.padding;
			r.w -= r.h - ctx->style.layout.padding;
			if(label)
				xui_control_draw_text(ctx, label, &r, fg, opt);
		}
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
			xui_draw_icon(ctx, ctx->style.font.icon_family, expanded ? ctx->style.tree.expanded_icon : ctx->style.tree.collapsed_icon, r.x, r.y, r.h, r.h, fg);
			r.x += r.h - ctx->style.layout.padding;
			r.w -= r.h - ctx->style.layout.padding;
			if(label)
				xui_control_draw_text(ctx, label, &r, fg, opt);
		}
	}
	else
	{
		bg = &wc->normal.background;
		fg = &wc->normal.foreground;
		bc = &wc->normal.border;
		if(bg->a)
		{
			xui_draw_icon(ctx, ctx->style.font.icon_family, expanded ? ctx->style.tree.expanded_icon : ctx->style.tree.collapsed_icon, r.x, r.y, r.h, r.h, bg);
			r.x += r.h - ctx->style.layout.padding;
			r.w -= r.h - ctx->style.layout.padding;
			if(label)
				xui_control_draw_text(ctx, label, &r, bg, opt);
		}
	}
	if(expanded)
	{
		xui_get_layout(ctx)->indent += ctx->style.layout.indent;
		xui_push(ctx->id_stack, ctx->last_id);
		return 1;
	}
	return 0;
}

void xui_end_tree(struct xui_context_t * ctx)
{
	xui_get_layout(ctx)->indent -= ctx->style.layout.indent;
	xui_pop_id(ctx);
}
