/*
 * kernel/xui/xui-slider.c
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
#include <xui/slider.h>

int xui_slider_ex(struct xui_context_t * ctx, double * value, double low, double high, double step, int opt)
{
	unsigned int id = xui_get_id(ctx, &value, sizeof(double *));
	struct region_t region, * r = xui_layout_next(ctx);
	struct xui_widget_color_t * wc;
	struct color_t * bg, * bc, * c;
	double v = *value;
	int radius, width;
	int l, x, y;

	xui_control_update(ctx, id, r, opt);
	radius = min(r->w, r->h) / 2;
	width = ctx->style.slider.border_width;
	if((ctx->active == id) && ((ctx->mouse.state & XUI_MOUSE_LEFT) || (ctx->mouse.down & XUI_MOUSE_LEFT)))
	{
		if(opt & XUI_SLIDER_VERTICAL)
			v = high - (ctx->mouse.y - (r->y + radius)) * (high - low) / (r->h - radius * 2);
		else
			v = low + (ctx->mouse.x - (r->x + radius)) * (high - low) / (r->w - radius * 2);
	}
	if(step > 0)
		v = ((int)((v + step / 2) / step)) * step;
	v = clamp(v, low, high);
	switch(opt & (0x7 << 8))
	{
	case XUI_SLIDER_PRIMARY:
		wc = &ctx->style.primary;
		break;
	case XUI_SLIDER_SECONDARY:
		wc = &ctx->style.secondary;
		break;
	case XUI_SLIDER_SUCCESS:
		wc = &ctx->style.success;
		break;
	case XUI_SLIDER_INFO:
		wc = &ctx->style.info;
		break;
	case XUI_SLIDER_WARNING:
		wc = &ctx->style.warning;
		break;
	case XUI_SLIDER_DANGER:
		wc = &ctx->style.danger;
		break;
	case XUI_SLIDER_LIGHT:
		wc = &ctx->style.light;
		break;
	case XUI_SLIDER_DARK:
		wc = &ctx->style.dark;
		break;
	default:
		wc = &ctx->style.primary;
		break;
	}
	if(ctx->active == id)
	{
		bg = &wc->active.background;
		bc = &wc->active.border;
	}
	else if(ctx->hover == id)
	{
		bg = &wc->hover.background;
		bc = &wc->hover.border;
	}
	else
	{
		bg = &wc->normal.background;
		bc = &wc->normal.border;
	}
	c = &ctx->style.slider.invalid_color;
	if(opt & XUI_SLIDER_VERTICAL)
	{
		region_init(&region, r->x + (r->w - radius) / 2, r->y + radius, radius, r->h - radius * 2);
		l = region.h * (v - low) / (high - low);
		x = region.x + region.w / 2;
		y = region.y + region.h - l;
		if(c->a)
			xui_draw_rectangle(ctx, region.x, region.y, region.w, region.h, region.w / 2, 0, c);
		if(bg->a)
			xui_draw_rectangle(ctx, region.x, region.y + region.h - l, region.w, l, (0x3 << 16) | (region.w / 2), 0, bg);
		if(bc->a && (width > 0))
			xui_draw_circle(ctx, x, y, radius, width, bc);
		if(bg->a)
			xui_draw_circle(ctx, x, y, radius, 0, bg);
	}
	else
	{
		region_init(&region, r->x + radius, r->y + (r->h - radius) / 2, r->w - radius * 2, radius);
		l = region.w * (v - low) / (high - low);
		x = region.x + l;
		y = region.y + region.h / 2;
		if(c->a)
			xui_draw_rectangle(ctx, region.x, region.y, region.w, region.h, region.h / 2, 0, c);
		if(bg->a)
			xui_draw_rectangle(ctx, region.x, region.y, l, region.h, (0x6 << 16) | (region.h / 2), 0, bg);
		if(bc->a && (width > 0))
			xui_draw_circle(ctx, x, y, radius, width, bc);
		if(bg->a)
			xui_draw_circle(ctx, x, y, radius, 0, bg);
	}
	if(*value != v)
	{
		*value = v;
		return 1;
	}
	return 0;
}
