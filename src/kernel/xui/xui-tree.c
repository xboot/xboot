/*
 * kernel/xui/xui-tree.c
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
#include <xui/tree.h>

static int header(struct xui_context_t * ctx, const char * label, int istree, int opt)
{
	unsigned int id = xui_get_id(ctx, label, strlen(label));
	int idx = xui_pool_get(ctx, ctx->tree_pool, XUI_TREE_POOL_SIZE, id);
	int active, expanded;
	struct region_t r;
	struct color_t * fc, * bc, * tc;
	int radius, width;

	xui_layout_row(ctx, 1, (int[]){ -1 }, 0);
	active = (idx >= 0);
	expanded = (opt & XUI_OPT_EXPANDED) ? !active : active;
	region_clone(&r, xui_layout_next(ctx));
	xui_control_update(ctx, id, &r, 0);
	active ^= ((ctx->focus == id) && (ctx->mouse.down & XUI_MOUSE_LEFT));
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
	if(ctx->focus == id)
	{
		fc = &ctx->style.tree.focus.face_color;
		bc = &ctx->style.tree.focus.border_color;
		tc = &ctx->style.tree.focus.text_color;
	}
	else if(ctx->hover == id)
	{
		fc = &ctx->style.tree.hover.face_color;
		bc = &ctx->style.tree.hover.border_color;
		tc = &ctx->style.tree.hover.text_color;
	}
	else
	{
		fc = &ctx->style.tree.normal.face_color;
		bc = &ctx->style.tree.normal.border_color;
		tc = &ctx->style.tree.normal.text_color;
	}
	if(istree)
	{
		if(ctx->hover == id)
		{
			if(bc->a && (width > 0))
				xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, radius, width, bc);
			if(fc->a)
				xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, radius, 0, fc);
		}
	}
	else
	{
		if(bc->a && (width > 0))
			xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, radius, width, bc);
		if(fc->a)
			xui_draw_rectangle(ctx, r.x, r.y, r.w, r.h, radius, 0, fc);
	}
	xui_draw_icon(ctx, ctx->style.font.icon_family, expanded ? ctx->style.tree.expanded_icon : ctx->style.tree.collapsed_icon, r.x, r.y, r.h, r.h, tc);
	r.x += r.h - ctx->style.layout.padding;
	r.w -= r.h - ctx->style.layout.padding;
	if(label && tc->a)
		xui_control_draw_text(ctx, label, &r, tc, opt);

	return expanded ? 1 : 0;
}

int xui_begin_tree_ex(struct xui_context_t * ctx, const char * label, int opt)
{
	int res = header(ctx, label, 1, opt);
	if(res)
	{
		xui_get_layout(ctx)->indent += ctx->style.layout.indent;
		xui_push(ctx->id_stack, ctx->last_id);
	}
	return res;
}

int xui_begin_tree(struct xui_context_t * ctx, const char * label)
{
	return xui_begin_tree_ex(ctx, label, 0);
}

void xui_end_tree(struct xui_context_t * ctx)
{
	xui_get_layout(ctx)->indent -= ctx->style.layout.indent;
	xui_pop_id(ctx);
}

int xui_header_ex(struct xui_context_t * ctx, const char * label, int opt)
{
	return header(ctx, label, 0, opt);
}

int xui_header(struct xui_context_t * ctx, const char * label)
{
	return header(ctx, label, 0, 0);
}
