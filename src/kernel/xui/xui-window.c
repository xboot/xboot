/*
 * kernel/xui/xui-window.c
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
#include <xui/window.h>

int xui_begin_window_ex(struct xui_context_t * ctx, const char * title, struct region_t * r, int opt)
{
	unsigned int id = title ? xui_get_id(ctx, title, strlen(title)) : xui_get_id(ctx, &title, sizeof(title));
	struct xui_container_t * c = get_container(ctx, id, opt);
	struct region_t body, region, hr, tr;

	if(c && c->open)
	{
		xui_push(ctx->id_stack, id);
		if(opt & XUI_WINDOW_FULLSCREEN)
			region_clone(&c->region, &ctx->screen);
		else if(c->region.w == 0)
			region_clone(&c->region, r ? r : &ctx->screen);
		root_container_begin(ctx, c);
		region_clone(&body, &c->region);
		region_clone(&region, &c->region);
		if(opt & XUI_WINDOW_FULLSCREEN)
		{
			xui_draw_rectangle(ctx, region.x, region.y, region.w, region.h, 0, 0, &ctx->style.window.face_color);
			push_container_body(ctx, c, &body, opt);
		}
		else
		{
			if(ctx->style.window.border_color.a && (ctx->style.window.border_width > 0))
				xui_draw_rectangle(ctx, region.x, region.y, region.w, region.h, ctx->style.window.border_radius, ctx->style.window.border_width, &ctx->style.window.border_color);
			xui_draw_rectangle(ctx, region.x, region.y, region.w, region.h, ctx->style.window.border_radius, 0, &ctx->style.window.face_color);
			if(~opt & XUI_WINDOW_NOTITLE)
			{
				region_clone(&hr, &region);
				hr.h = ctx->style.window.title_height;
				xui_draw_rectangle(ctx, hr.x, hr.y, hr.w, hr.h, (0xc << 16) | ctx->style.window.border_radius, 0, &ctx->style.window.title_color);
				id = xui_get_id(ctx, "!title", 6);
				xui_control_update(ctx, id, &hr, opt);
				xui_control_draw_text(ctx, title, &hr, &ctx->style.window.text_color, opt);
				if((ctx->focus == id) && (ctx->mouse.state & XUI_MOUSE_LEFT))
				{
					c->region.x += ctx->mouse.dx;
					c->region.y += ctx->mouse.dy;
				}
				body.y += hr.h;
				body.h -= hr.h;
				if(~opt & XUI_WINDOW_NOCLOSE)
				{
					id = xui_get_id(ctx, "!close", 6);
					region_init(&tr, hr.x + hr.w - hr.h, hr.y, hr.h, hr.h);
					hr.w -= tr.w;
					xui_draw_icon(ctx, ctx->style.font.icon_family, ctx->style.window.close_icon, tr.x, tr.y, tr.w, tr.h, &ctx->style.window.text_color);
					xui_control_update(ctx, id, &tr, opt);
					if((ctx->focus == id) && (ctx->mouse.up & XUI_MOUSE_LEFT))
						c->open = 0;
				}
			}
			push_container_body(ctx, c, &body, opt);
			if(~opt & XUI_WINDOW_NORESIZE)
			{
				int sz = ctx->style.window.title_height;
				id = xui_get_id(ctx, "!resize", 7);
				region_init(&tr, region.x + region.w - sz, region.y + region.h - sz, sz, sz);
				xui_control_update(ctx, id, &tr, opt);
				if((ctx->focus == id) && (ctx->mouse.state & XUI_MOUSE_LEFT))
				{
					if(ctx->resize_id != id)
					{
						ctx->resize_id = id;
						ctx->resize_cursor_x = ctx->mouse.x - tr.x;
						ctx->resize_cursor_y = ctx->mouse.y - tr.y;
					}
					c->region.w = max(64, ctx->mouse.x - region.x + sz - ctx->resize_cursor_x);
					c->region.h = max(64, ctx->mouse.y - region.y + sz - ctx->resize_cursor_y);
				}
				else if(ctx->resize_id == id)
				{
					ctx->resize_id = 0;
				}
			}
			if(opt & XUI_WINDOW_POPUP)
			{
				struct region_t * pr = &xui_get_layout(ctx)->body;
				c->region.w = c->content_width + (c->region.w - pr->w);
				c->region.h = c->content_height + (c->region.h - pr->h);
				if(ctx->mouse.down && (ctx->hover_root != c))
					c->open = 0;
			}
		}
		xui_push_clip(ctx, &c->body);
		return 1;
	}
	return 0;
}

void xui_end_window(struct xui_context_t * ctx)
{
	xui_pop_clip(ctx);
	root_container_end(ctx);
}
