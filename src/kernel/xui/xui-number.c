/*
 * kernel/xui/xui-number.c
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
#include <xui/number.h>

int xui_number_ex(struct xui_context_t * ctx, double * value, double low, double high, double step, const char * fmt, int opt)
{
	unsigned int id = xui_get_id(ctx, &value, sizeof(double *));
	struct region_t * r = xui_layout_next(ctx);
	struct xui_widget_color_t * wc;
	struct color_t * fc, * bc, * tc;
	double v = *value;
	int radius, width;

	xui_control_update(ctx, id, r, opt);
	if((ctx->focus == id) && ((ctx->mouse.state & XUI_MOUSE_LEFT) || (ctx->mouse.down & XUI_MOUSE_LEFT)))
		v += ctx->mouse.dx * step;
	v = clamp(v, low, high);
	if(opt & XUI_NUMBER_ROUNDED)
		radius = r->h / 2;
	else
		radius = ctx->style.number.border_radius;
	width = ctx->style.number.border_width;
	switch(opt & (0x7 << 8))
	{
	case XUI_NUMBER_PRIMARY:
		wc = &ctx->style.primary;
		break;
	case XUI_NUMBER_SECONDARY:
		wc = &ctx->style.secondary;
		break;
	case XUI_NUMBER_SUCCESS:
		wc = &ctx->style.success;
		break;
	case XUI_NUMBER_INFO:
		wc = &ctx->style.info;
		break;
	case XUI_NUMBER_WARNING:
		wc = &ctx->style.warning;
		break;
	case XUI_NUMBER_DANGER:
		wc = &ctx->style.danger;
		break;
	case XUI_NUMBER_LIGHT:
		wc = &ctx->style.light;
		break;
	case XUI_NUMBER_DARK:
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
			xui_control_draw_text(ctx, xui_format(ctx, fmt, v), r, tc, opt);
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
			xui_control_draw_text(ctx, xui_format(ctx, fmt, v), r, tc, opt);
	}
	else
	{
		fc = &wc->normal.face;
		bc = &wc->normal.border;
		tc = &wc->normal.text;
		if(opt & XUI_NUMBER_OUTLINE)
		{
			if(fc->a)
			{
				xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, ctx->style.number.outline_width, fc);
				xui_control_draw_text(ctx, xui_format(ctx, fmt, v), r, fc, opt);
			}
		}
		else
		{
			if(bc->a && (width > 0))
				xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, width, bc);
			if(fc->a)
				xui_draw_rectangle(ctx, r->x, r->y, r->w, r->h, radius, 0, fc);
			if(tc->a)
				xui_control_draw_text(ctx, xui_format(ctx, fmt, v), r, tc, opt);
		}
	}
	if(*value != v)
	{
		*value = v;
		return 1;
	}
	return 0;
}
