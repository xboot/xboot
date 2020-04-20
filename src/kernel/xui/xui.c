/*
 * kernel/xui/xui.c
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
#include <xui/xui.h>

static int text_width(mu_Font font, const char *text, int len)
{
	if(len == -1)
	{
		len = strlen(text);
	}
	return 16 * len;
}

static int text_height(mu_Font font)
{
	return 16;
}

struct xui_context_t * xui_context_alloc(const char * fb, const char * input, struct color_t * c)
{
	struct xui_context_t * ctx;

	ctx = malloc(sizeof(struct xui_context_t));
	if(!ctx)
		return NULL;

	memset(ctx, 0, sizeof(struct xui_context_t));
	ctx->w = window_alloc(fb, input, ctx);
	ctx->f = font_context_alloc();
	region_init(&ctx->clip, 0, 0, window_get_width(ctx->w), window_get_height(ctx->w));
	if(c)
		memcpy(&ctx->c, c, sizeof(struct color_t));
	else
		color_init(&ctx->c, 255, 255, 255, 255);

	mu_init(&ctx->mu);
	ctx->mu.text_width = text_width;
	ctx->mu.text_height = text_height;
	return ctx;
}

void xui_context_free(struct xui_context_t * ctx)
{
	if(ctx)
	{
		window_free(ctx->w);
		font_context_free(ctx->f);
		free(ctx);
	}
}

static void xui_draw(struct window_t * w, void * o)
{
	struct xui_context_t * ctx = (struct xui_context_t *)o;
	struct surface_t * s = ctx->w->s;
	struct region_t * clip = &ctx->clip;
//	struct xui_command_t cmd;
    mu_Command * cmd = NULL;
    struct color_t c;

	while(mu_next_command(&ctx->mu, &cmd))
	{
		switch(cmd->type)
		{
		case MU_COMMAND_TEXT:
/*			cairo_save(cr);
			cairo_set_font_size(cr, 16);
			cairo_set_source_rgb(cr, cmd->text.color.r / 255.0, cmd->text.color.g / 255.0, cmd->text.color.b / 255.0);
			cairo_move_to(cr, cmd->text.pos.x, cmd->text.pos.y + 16);
			cairo_show_text(cr, cmd->text.str);
			cairo_restore(cr);*/
			break;
		case MU_COMMAND_RECT:
			c.r = cmd->rect.color.r;
			c.g = cmd->rect.color.g;
			c.b = cmd->rect.color.b;
			c.a = 255;
			surface_shape_rectangle(s, clip, cmd->rect.rect.x, cmd->rect.rect.y, cmd->rect.rect.w, cmd->rect.rect.h, 0, 0, &c);
			break;
		case MU_COMMAND_ICON:
			//r_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color);
			break;
		case MU_COMMAND_CLIP:
			{
				struct region_t a, b;
				region_init(&a, cmd->clip.rect.x, cmd->clip.rect.y, cmd->clip.rect.w, cmd->clip.rect.h);
				region_init(&b, 0, 0, window_get_width(ctx->w), window_get_height(ctx->w));
				region_intersect(clip, &a, &b);
			}
			break;
		}
	}
}

void xui_loop(struct xui_context_t * ctx, void (*func)(struct xui_context_t *))
{
	struct event_t e;

	while(1)
	{
		if(window_pump_event(ctx->w, &e))
		{
			switch(e.type)
			{
			case EVENT_TYPE_KEY_DOWN:
				mu_input_keydown(&ctx->mu, e.e.key_down.key);
				break;
			case EVENT_TYPE_KEY_UP:
				mu_input_keyup(&ctx->mu, e.e.key_up.key);
				break;
			case EVENT_TYPE_MOUSE_DOWN:
				mu_input_mousedown(&ctx->mu, e.e.mouse_down.x, e.e.mouse_down.y, e.e.mouse_down.button);
				break;
			case EVENT_TYPE_MOUSE_MOVE:
				mu_input_mousemove(&ctx->mu, e.e.mouse_move.x, e.e.mouse_move.y);
				break;
			case EVENT_TYPE_MOUSE_UP:
				mu_input_mouseup(&ctx->mu, e.e.mouse_up.x, e.e.mouse_up.y, e.e.mouse_up.button);
				break;
			case EVENT_TYPE_MOUSE_WHEEL:
				mu_input_scroll(&ctx->mu, e.e.mouse_wheel.dx * 30, e.e.mouse_wheel.dy * -30);
				break;
			default:
				break;
			}
		}
		if(func)
			func(ctx);
		if(window_is_active(ctx->w))
		{
			ctx->w->wm->refresh = 1;
			window_present(ctx->w, &ctx->c, ctx, xui_draw);
		}
		task_yield();
	}
}

#if 0
struct xui_context_t * xui_context_alloc(const char * fb, const char * input, struct color_t * c)
{
	struct xui_context_t * ctx;

	ctx = malloc(sizeof(struct xui_context_t));
	if(!ctx)
		return NULL;

	memset(ctx, 0, sizeof(struct xui_context_t));
	ctx->w = window_alloc(fb, input, ctx);
	ctx->f = font_context_alloc();
	region_init(&ctx->clip, 0, 0, window_get_width(ctx->w), window_get_height(ctx->w));
	if(c)
		memcpy(&ctx->bgcolor, c, sizeof(struct color_t));
	else
		color_init(&ctx->bgcolor, 255, 255, 255, 255);
	ctx->cmdfifo = fifo_alloc(sizeof(struct xui_command_t) * 1024);

	return ctx;
}

void xui_context_free(struct xui_context_t * ctx)
{
	if(ctx)
	{
		window_free(ctx->w);
		font_context_free(ctx->f);
		fifo_free(ctx->cmdfifo);
		free(ctx);
	}
}

void xui_context_clear(struct xui_context_t * ctx)
{
	if(ctx)
	{
		__fifo_reset(ctx->cmdfifo);
	}
}

void xui_push_clip(struct xui_context_t * ctx, int x, int y, int w, int h)
{
	struct xui_command_t cmd;
	struct region_t a, b;

	cmd.type = XUI_COMMAND_TYPE_CLIP;
	region_init(&a, x, y, w, h);
	region_init(&b, 0, 0, window_get_width(ctx->w), window_get_height(ctx->w));
	region_intersect(&cmd.u.clip.r, &a, &b);
	__fifo_put(ctx->cmdfifo, (unsigned char *)&cmd, sizeof(struct xui_command_t));
}

void xui_push_line(struct xui_context_t * ctx, int x0, int y0, int x1, int y1, int thickness, struct color_t * c)
{
	struct xui_command_t cmd;

	cmd.type = XUI_COMMAND_TYPE_LINE;
	cmd.u.line.p0.x = x0;
	cmd.u.line.p0.y = y0;
	cmd.u.line.p1.x = x1;
	cmd.u.line.p1.y = y1;
	cmd.u.line.thickness = thickness;
	memcpy(&cmd.u.line.c, c, sizeof(struct color_t));
	__fifo_put(ctx->cmdfifo, (unsigned char *)&cmd, sizeof(struct xui_command_t));
}

void xui_push_triangle(struct xui_context_t * ctx, int x0, int y0, int x1, int y1, int x2, int y2, int thickness, struct color_t * c)
{
	struct xui_command_t cmd;

	cmd.type = XUI_COMMAND_TYPE_TRIANGLE;
	cmd.u.triangle.p0.x = x0;
	cmd.u.triangle.p0.y = y0;
	cmd.u.triangle.p1.x = x1;
	cmd.u.triangle.p1.y = y1;
	cmd.u.triangle.p2.x = x2;
	cmd.u.triangle.p2.y = y2;
	cmd.u.triangle.thickness = thickness;
	memcpy(&cmd.u.triangle.c, c, sizeof(struct color_t));
	__fifo_put(ctx->cmdfifo, (unsigned char *)&cmd, sizeof(struct xui_command_t));
}

void xui_push_rectangle(struct xui_context_t * ctx, int x, int y, int w, int h, int radius, int thickness, struct color_t * c)
{
	struct xui_command_t cmd;

	cmd.type = XUI_COMMAND_TYPE_RECTANGLE;
	cmd.u.rectangle.x = x;
	cmd.u.rectangle.y = y;
	cmd.u.rectangle.w = w;
	cmd.u.rectangle.h = h;
	cmd.u.rectangle.radius = radius;
	cmd.u.rectangle.thickness = thickness;
	memcpy(&cmd.u.rectangle.c, c, sizeof(struct color_t));
	__fifo_put(ctx->cmdfifo, (unsigned char *)&cmd, sizeof(struct xui_command_t));
}

void xui_push_circle(struct xui_context_t * ctx, int x, int y, int radius, int thickness, struct color_t * c)
{
	struct xui_command_t cmd;

	cmd.type = XUI_COMMAND_TYPE_CIRCLE;
	cmd.u.circle.x = x;
	cmd.u.circle.y = y;
	cmd.u.circle.radius = radius;
	cmd.u.circle.thickness = thickness;
	memcpy(&cmd.u.circle.c, c, sizeof(struct color_t));
	__fifo_put(ctx->cmdfifo, (unsigned char *)&cmd, sizeof(struct xui_command_t));
}

void xui_push_ellipse(struct xui_context_t * ctx, int x, int y, int w, int h, int thickness, struct color_t * c)
{
	struct xui_command_t cmd;

	cmd.type = XUI_COMMAND_TYPE_ELLIPSE;
	cmd.u.ellipse.x = x;
	cmd.u.ellipse.y = y;
	cmd.u.ellipse.w = w;
	cmd.u.ellipse.h = h;
	cmd.u.ellipse.thickness = thickness;
	memcpy(&cmd.u.ellipse.c, c, sizeof(struct color_t));
	__fifo_put(ctx->cmdfifo, (unsigned char *)&cmd, sizeof(struct xui_command_t));
}

void xui_push_arc(struct xui_context_t * ctx, int x, int y, int radius, int a1, int a2, int thickness, struct color_t * c)
{
	struct xui_command_t cmd;

	cmd.type = XUI_COMMAND_TYPE_ARC;
	cmd.u.arc.x = x;
	cmd.u.arc.y = y;
	cmd.u.arc.radius = radius;
	cmd.u.arc.a1 = a1;
	cmd.u.arc.a2 = a2;
	cmd.u.arc.thickness = thickness;
	memcpy(&cmd.u.arc.c, c, sizeof(struct color_t));
	__fifo_put(ctx->cmdfifo, (unsigned char *)&cmd, sizeof(struct xui_command_t));
}

static void xui_draw(struct window_t * w, void * o)
{
	struct xui_context_t * ctx = (struct xui_context_t *)o;
	struct surface_t * s = ctx->w->s;
	struct region_t * clip = &ctx->clip;
	struct xui_command_t cmd;

	while(__fifo_get(ctx->cmdfifo, (unsigned char *)&cmd, sizeof(struct xui_command_t)) == sizeof(struct xui_command_t))
	{
		switch(cmd.type)
		{
		case XUI_COMMAND_TYPE_CLIP:
			region_clone(clip, &cmd.u.clip.r);
			break;
		case XUI_COMMAND_TYPE_LINE:
			surface_shape_line(s, clip, &cmd.u.line.p0, &cmd.u.line.p1, cmd.u.line.thickness, &cmd.u.line.c);
			break;
		case XUI_COMMAND_TYPE_TRIANGLE:
			surface_shape_triangle(s, clip, &cmd.u.triangle.p0, &cmd.u.triangle.p1, &cmd.u.triangle.p2, cmd.u.triangle.thickness, &cmd.u.triangle.c);
			break;
		case XUI_COMMAND_TYPE_RECTANGLE:
			surface_shape_rectangle(s, clip, cmd.u.rectangle.x, cmd.u.rectangle.y, cmd.u.rectangle.w, cmd.u.rectangle.h, cmd.u.rectangle.radius, cmd.u.rectangle.thickness, &cmd.u.rectangle.c);
			break;
		case XUI_COMMAND_TYPE_CIRCLE:
			surface_shape_circle(s, clip, cmd.u.circle.x, cmd.u.circle.y, cmd.u.circle.radius, cmd.u.circle.thickness, &cmd.u.circle.c);
			break;
		case XUI_COMMAND_TYPE_ELLIPSE:
			surface_shape_ellipse(s, clip, cmd.u.ellipse.x, cmd.u.ellipse.y, cmd.u.ellipse.w, cmd.u.ellipse.h, cmd.u.ellipse.thickness, &cmd.u.ellipse.c);
			break;
		case XUI_COMMAND_TYPE_ARC:
			surface_shape_arc(s, clip, cmd.u.arc.x, cmd.u.arc.y, cmd.u.arc.radius, cmd.u.arc.a1, cmd.u.arc.a2, cmd.u.arc.thickness, &cmd.u.arc.c);
			break;
		default:
			break;
		}
	}
}

void xui_loop(struct xui_context_t * ctx, void (*func)(struct xui_context_t *))
{
	while(1)
	{
		if(func)
			func(ctx);
		if(window_is_active(ctx->w))
		{
			ctx->w->wm->refresh = 1;
			window_present(ctx->w, &ctx->bgcolor, ctx, xui_draw);
		}
		xui_context_clear(ctx);
		task_yield();
	}
}

#endif
