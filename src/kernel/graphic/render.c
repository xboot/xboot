/*
 * kernel/graphic/render.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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
#include <cg.h>
#include <graphic/surface.h>

struct render_cg_ctx_t {
	struct cg_surface_t * cs;
	struct cg_ctx_t * cg;
};

static void * render_cg_create(struct surface_t * s)
{
	struct render_cg_ctx_t * ctx;

	ctx = malloc(sizeof(struct render_cg_ctx_t));
	if(!ctx)
		return NULL;
	ctx->cs = cg_surface_create_for_data(s->width, s->height, s->pixels);
	ctx->cg = cg_create(ctx->cs);
	return ctx;
}

static void render_cg_destroy(void * rctx)
{
	struct render_cg_ctx_t * ctx = (struct render_cg_ctx_t *)rctx;
	cg_destroy(ctx->cg);
	cg_surface_destroy(ctx->cs);
	free(ctx);
}

static void render_cg_blit(struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct surface_t * src)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	struct region_t r;

	cg_save(cg);
	if(clip)
	{
		region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
		if(region_intersect(&r, &r, clip))
		{
			cg_rectangle(cg, r.x, r.y, r.w, r.h);
			cg_clip(cg);
		}
		else
		{
			cg_restore(cg);
			return;
		}
	}
	if(m)
	{
		cg_set_matrix(cg, (struct cg_matrix_t *)m);
	}
	cg_rectangle(cg, 0, 0, src->width, src->height);
	cg_clip(cg);
	cg_set_source_surface(cg, ((struct render_cg_ctx_t *)src->rctx)->cs, 0, 0);
	cg_paint(cg);
	cg_restore(cg);
}

static void render_cg_fill(struct surface_t * s, struct region_t * clip, struct matrix_t * m, int w, int h, struct color_t * c)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	struct region_t r;

	cg_save(cg);
	if(clip)
	{
		region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
		if(region_intersect(&r, &r, clip))
		{
			cg_rectangle(cg, r.x, r.y, r.w, r.h);
			cg_clip(cg);
		}
		else
		{
			cg_restore(cg);
			return;
		}
	}
	if(m)
	{
		cg_set_matrix(cg, (struct cg_matrix_t *)m);
	}
	cg_rectangle(cg, 0, 0, w, h);
	cg_set_source_rgba(cg, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
	cg_fill(cg);
	cg_restore(cg);
}

static void render_cg_shape_save(struct surface_t * s)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_save(cg);
}

static void render_cg_shape_restore(struct surface_t * s)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_restore(cg);
}

static void render_cg_shape_set_source(struct surface_t * s, struct surface_t * o, double x, double y)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_rectangle(cg, 0, 0, surface_get_width(s), surface_get_height(s));
	cg_clip(cg);
	cg_set_source_surface(cg, ((struct render_cg_ctx_t *)o->rctx)->cs, x, y);
}

static void render_cg_shape_set_source_color(struct surface_t * s, struct color_t * c)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_set_source_rgba(cg, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
}

static void render_cg_shape_set_line_width(struct surface_t * s, double w)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_set_line_width(cg, w);
}

static void render_cg_shape_set_matrix(struct surface_t * s, struct matrix_t * m)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_set_matrix(cg, (struct cg_matrix_t *)m);
}

static void render_cg_shape_new_path(struct surface_t * s)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_new_path(cg);
}

static void render_cg_shape_close_path(struct surface_t * s)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_close_path(cg);
}

static void render_cg_shape_move_to(struct surface_t * s, double x, double y)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_move_to(cg, x, y);
}

static void render_cg_shape_line_to(struct surface_t * s, double x, double y)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_line_to(cg, x, y);
}

static void render_cg_shape_curve_to(struct surface_t * s, double x1, double y1, double x2, double y2, double x3, double y3)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_curve_to(cg, x1, y1, x2, y2, x3, y3);
}

static void render_cg_shape_rectangle(struct surface_t * s, double x, double y, double w, double h)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_rectangle(cg, x, y, w, h);
}

static void render_cg_shape_arc(struct surface_t * s, double cx, double cy, double r, double a0, double a1)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_arc(cg, cx, cy, r, a0, a1);
}

static void render_cg_shape_arc_negative(struct surface_t * s, double cx, double cy, double r, double a0, double a1)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_arc_negative(cg, cx, cy, r, a0, a1);
}

static void render_cg_shape_circle(struct surface_t * s, double cx, double cy, double r)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_circle(cg, cx, cy, r);
}

static void render_cg_shape_ellipse(struct surface_t * s, double cx, double cy, double rx, double ry)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_ellipse(cg, cx, cy, rx, ry);
}

static void render_cg_shape_clip(struct surface_t * s)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_clip(cg);
}

static void render_cg_shape_clip_preserve(struct surface_t * s)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_clip_preserve(cg);
}

static void render_cg_shape_fill(struct surface_t * s)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_fill(cg);
}

static void render_cg_shape_fill_preserve(struct surface_t * s)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_fill_preserve(cg);
}

static void render_cg_shape_stroke(struct surface_t * s)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_stroke(cg);
}

static void render_cg_shape_stroke_preserve(struct surface_t * s)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_stroke_preserve(cg);
}

static void render_cg_shape_paint(struct surface_t * s)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	cg_paint(cg);
}

struct render_t render_cg = {
	.name	 				= "cg",

	.create					= render_cg_create,
	.destroy				= render_cg_destroy,

	.blit					= render_cg_blit,
	.fill					= render_cg_fill,

	.shape_save				= render_cg_shape_save,
	.shape_restore			= render_cg_shape_restore,
	.shape_set_source		= render_cg_shape_set_source,
	.shape_set_source_color	= render_cg_shape_set_source_color,
	.shape_set_line_width	= render_cg_shape_set_line_width,
	.shape_set_matrix		= render_cg_shape_set_matrix,
	.shape_new_path			= render_cg_shape_new_path,
	.shape_close_path		= render_cg_shape_close_path,
	.shape_move_to			= render_cg_shape_move_to,
	.shape_line_to			= render_cg_shape_line_to,
	.shape_curve_to			= render_cg_shape_curve_to,
	.shape_rectangle		= render_cg_shape_rectangle,
	.shape_arc				= render_cg_shape_arc,
	.shape_arc_negative		= render_cg_shape_arc_negative,
	.shape_circle			= render_cg_shape_circle,
	.shape_ellipse			= render_cg_shape_ellipse,
	.shape_clip				= render_cg_shape_clip,
	.shape_clip_preserve	= render_cg_shape_clip_preserve,
	.shape_fill				= render_cg_shape_fill,
	.shape_fill_preserve	= render_cg_shape_fill_preserve,
	.shape_stroke			= render_cg_shape_stroke,
	.shape_stroke_preserve	= render_cg_shape_stroke_preserve,
	.shape_paint			= render_cg_shape_paint,
};
