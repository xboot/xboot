/*
 * kernel/xui/xui-chart.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
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
#include <xui/chart.h>

void xui_chart_ex(struct xui_context_t * ctx, double * values, int len, double ymin, double ymax, struct color_t * c, int opt)
{
	if(len > 2)
	{
		struct region_t * r = xui_layout_next(ctx);
		if(opt & XUI_CHART_AREA)
		{
			struct point_t p[len + 2];
			p[0].x = r->x;
			p[0].y = r->y + r->h;
			for(int i = 0; i < len; i++)
			{
				p[i + 1].x = r->x + r->w * i / (len - 1);
				p[i + 1].y = r->y + (ymax - values[i]) * r->h / (ymax - ymin);
			}
			p[len + 1].x = r->x + r->w;
			p[len + 1].y = r->y + r->h;
			struct color_t lc;
			color_level(&lc, c, -2);
			xui_push_clip(ctx, r);
			xui_draw_polygon(ctx, p, len + 2, 0, &lc);
			xui_draw_polyline(ctx, &p[1], len, 2, c);
			if(opt & XUI_CHART_DOTS)
			{
				for(int i = 1; i < len + 1; i++)
					xui_draw_circle(ctx, p[i].x, p[i].y, 2, 0, c);
			}
			xui_pop_clip(ctx);
		}
		else
		{
			struct point_t p[len];
			for(int i = 0; i < len; i++)
			{
				p[i].x = r->x + r->w * i / (len - 1);
				p[i].y = r->y + (ymax - values[i]) * r->h / (ymax - ymin);
			}
			xui_push_clip(ctx, r);
			xui_draw_polyline(ctx, p, len, 2, c);
			if(opt & XUI_CHART_DOTS)
			{
				for(int i = 0; i < len; i++)
					xui_draw_circle(ctx, p[i].x, p[i].y, 2, 0, c);
			}
			xui_pop_clip(ctx);
		}
	}
}

void xui_chart_float_ex(struct xui_context_t * ctx, float * values, int len, float ymin, float ymax, struct color_t * c, int opt)
{
	if(len > 2)
	{
		struct region_t * r = xui_layout_next(ctx);
		if(opt & XUI_CHART_AREA)
		{
			struct point_t p[len + 2];
			p[0].x = r->x;
			p[0].y = r->y + r->h;
			for(int i = 0; i < len; i++)
			{
				p[i + 1].x = r->x + r->w * i / (len - 1);
				p[i + 1].y = r->y + (ymax - values[i]) * r->h / (ymax - ymin);
			}
			p[len + 1].x = r->x + r->w;
			p[len + 1].y = r->y + r->h;
			struct color_t lc;
			color_level(&lc, c, -2);
			xui_push_clip(ctx, r);
			xui_draw_polygon(ctx, p, len + 2, 0, &lc);
			xui_draw_polyline(ctx, &p[1], len, 2, c);
			if(opt & XUI_CHART_DOTS)
			{
				for(int i = 1; i < len + 1; i++)
					xui_draw_circle(ctx, p[i].x, p[i].y, 2, 0, c);
			}
			xui_pop_clip(ctx);
		}
		else
		{
			struct point_t p[len];
			for(int i = 0; i < len; i++)
			{
				p[i].x = r->x + r->w * i / (len - 1);
				p[i].y = r->y + (ymax - values[i]) * r->h / (ymax - ymin);
			}
			xui_push_clip(ctx, r);
			xui_draw_polyline(ctx, p, len, 2, c);
			if(opt & XUI_CHART_DOTS)
			{
				for(int i = 0; i < len; i++)
					xui_draw_circle(ctx, p[i].x, p[i].y, 2, 0, c);
			}
			xui_pop_clip(ctx);
		}
	}
}

void xui_chart_int_ex(struct xui_context_t * ctx, int * values, int len, int ymin, int ymax, struct color_t * c, int opt)
{
	if(len > 2)
	{
		struct region_t * r = xui_layout_next(ctx);
		if(opt & XUI_CHART_AREA)
		{
			struct point_t p[len + 2];
			p[0].x = r->x;
			p[0].y = r->y + r->h;
			for(int i = 0; i < len; i++)
			{
				p[i + 1].x = r->x + r->w * i / (len - 1);
				p[i + 1].y = r->y + (ymax - values[i]) * r->h / (ymax - ymin);
			}
			p[len + 1].x = r->x + r->w;
			p[len + 1].y = r->y + r->h;
			struct color_t lc;
			color_level(&lc, c, -2);
			xui_push_clip(ctx, r);
			xui_draw_polygon(ctx, p, len + 2, 0, &lc);
			xui_draw_polyline(ctx, &p[1], len, 2, c);
			if(opt & XUI_CHART_DOTS)
			{
				for(int i = 1; i < len + 1; i++)
					xui_draw_circle(ctx, p[i].x, p[i].y, 2, 0, c);
			}
			xui_pop_clip(ctx);
		}
		else
		{
			struct point_t p[len];
			for(int i = 0; i < len; i++)
			{
				p[i].x = r->x + r->w * i / (len - 1);
				p[i].y = r->y + (ymax - values[i]) * r->h / (ymax - ymin);
			}
			xui_push_clip(ctx, r);
			xui_draw_polyline(ctx, p, len, 2, c);
			if(opt & XUI_CHART_DOTS)
			{
				for(int i = 0; i < len; i++)
					xui_draw_circle(ctx, p[i].x, p[i].y, 2, 0, c);
			}
			xui_pop_clip(ctx);
		}
	}
}
