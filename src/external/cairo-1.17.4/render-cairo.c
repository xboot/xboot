/*
 * render-cairo.c
 */

#include <xboot.h>
#include <cairo.h>
#include <cairoint.h>
#include <cairo-ft.h>
#include <graphic/surface.h>

struct render_cairo_context_t {
	cairo_surface_t * cs;
	cairo_t * cr;
};

static void * render_cairo_create(struct surface_t * s)
{
	struct render_cairo_context_t * ctx;

	ctx = malloc(sizeof(struct render_cairo_context_t));
	if(!ctx)
		return NULL;
	ctx->cs = cairo_image_surface_create_for_data((unsigned char *)s->pixels, CAIRO_FORMAT_ARGB32, s->width, s->height, s->stride);
	ctx->cr = cairo_create(ctx->cs);
	return ctx;
}

static void render_cairo_destroy(void * rctx)
{
	struct render_cairo_context_t * ctx = (struct render_cairo_context_t *)rctx;
	cairo_destroy(ctx->cr);
	cairo_surface_destroy(ctx->cs);
	free(ctx);
}

static void render_cairo_blit(struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct surface_t * src)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	struct region_t r;

	cairo_save(cr);
	if(clip)
	{
		region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
		if(region_intersect(&r, &r, clip))
		{
			cairo_rectangle(cr, r.x, r.y, r.w, r.h);
			cairo_clip(cr);
		}
		else
		{
			cairo_restore(cr);
			return;
		}
	}
	if(m)
	{
		cairo_set_matrix(cr, (cairo_matrix_t *)m);
	}
	cairo_set_source_surface(cr, ((struct render_cairo_context_t *)src->rctx)->cs, 0, 0);
	cairo_paint(cr);
	cairo_restore(cr);
}

static void render_cairo_fill(struct surface_t * s, struct region_t * clip, struct matrix_t * m, int w, int h, struct color_t * c)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	struct region_t r;

	cairo_save(cr);
	if(clip)
	{
		region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
		if(region_intersect(&r, &r, clip))
		{
			cairo_rectangle(cr, r.x, r.y, r.w, r.h);
			cairo_clip(cr);
		}
		else
		{
			cairo_restore(cr);
			return;
		}
	}
	if(m)
	{
		cairo_set_matrix(cr, (cairo_matrix_t *)m);
	}
	cairo_rectangle(cr, 0, 0, w, h);
	cairo_set_source_rgba(cr, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
	cairo_fill(cr);
	cairo_restore(cr);
}


static void render_cairo_shape_save(struct surface_t * s)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_save(cr);
}

static void render_cairo_shape_restore(struct surface_t * s)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_restore(cr);
}

static void render_cairo_shape_set_source(struct surface_t * s, struct surface_t * o, double x, double y)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_set_source_surface(cr, ((struct render_cairo_context_t *)o->rctx)->cs, x, y);
}

static void render_cairo_shape_set_source_color(struct surface_t * s, struct color_t * c)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_set_source_rgba(cr, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
}

static void render_cairo_shape_set_line_width(struct surface_t * s, double w)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_set_line_width(cr, w);
}

static void render_cairo_shape_translate(struct surface_t * s, double tx, double ty)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_translate(cr, tx, ty);
}

static void render_cairo_shape_scale(struct surface_t * s, double sx, double sy)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_scale(cr, sx, sy);
}

static void render_cairo_shape_rotate(struct surface_t * s, double r)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_rotate(cr, r);
}

static void render_cairo_shape_transform(struct surface_t * s, struct matrix_t * m)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_transform(cr, (cairo_matrix_t *)m);
}

static void render_cairo_shape_set_matrix(struct surface_t * s, struct matrix_t * m)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_set_matrix(cr, (cairo_matrix_t *)m);
}

static void render_cairo_shape_identity_matrix(struct surface_t * s)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_identity_matrix(cr);
}

static void render_cairo_shape_new_path(struct surface_t * s)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_new_path(cr);
}

static void render_cairo_shape_close_path(struct surface_t * s)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_close_path(cr);
}

static void render_cairo_shape_move_to(struct surface_t * s, double x, double y)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_move_to(cr, x, y);
}

static void render_cairo_shape_line_to(struct surface_t * s, double x, double y)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_line_to(cr, x, y);
}

static void render_cairo_shape_curve_to(struct surface_t * s, double x1, double y1, double x2, double y2, double x3, double y3)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_curve_to(cr, x1, y1, x2, y2, x3, y3);
}

static void render_cairo_shape_rectangle(struct surface_t * s, double x, double y, double w, double h)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_rectangle(cr, x, y, w, h);
}

static void render_cairo_shape_arc(struct surface_t * s, double cx, double cy, double r, double a0, double a1)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_arc(cr, cx, cy, r, a0, a1);
}

static void render_cairo_shape_arc_negative(struct surface_t * s, double cx, double cy, double r, double a0, double a1)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_arc_negative(cr, cx, cy, r, a0, a1);
}

static void render_cairo_shape_circle(struct surface_t * s, double cx, double cy, double r)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_move_to(cr, cx + r, cy);
	cairo_arc(cr, cx, cy, r, 0, M_PI * 2);
}

static void render_cairo_shape_ellipse(struct surface_t * s, double cx, double cy, double rx, double ry)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_matrix_t m;

	cairo_get_matrix(cr, &m);
	cairo_translate(cr, cx, cy);
	cairo_scale(cr, 1, (double)ry / (double)rx);
	cairo_translate(cr, -cx, -cy);
	cairo_move_to(cr, cx + rx, cy);
	cairo_arc(cr, cx, cy, rx, 0, M_PI * 2);
	cairo_close_path(cr);
	cairo_set_matrix(cr, &m);
}

static void render_cairo_shape_clip(struct surface_t * s)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_clip(cr);
}

static void render_cairo_shape_clip_preserve(struct surface_t * s)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_clip_preserve(cr);
}

static void render_cairo_shape_fill(struct surface_t * s)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_fill(cr);
}

static void render_cairo_shape_fill_preserve(struct surface_t * s)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_fill_preserve(cr);
}

static void render_cairo_shape_stroke(struct surface_t * s)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_stroke(cr);
}

static void render_cairo_shape_stroke_preserve(struct surface_t * s)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_stroke_preserve(cr);
}

static void render_cairo_shape_paint(struct surface_t * s)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_paint(cr);
}

static struct render_t render_cairo = {
	.name	 				= "cairo",

	.create					= render_cairo_create,
	.destroy				= render_cairo_destroy,

	.blit					= render_cairo_blit,
	.fill					= render_cairo_fill,

	.shape_save				= render_cairo_shape_save,
	.shape_restore			= render_cairo_shape_restore,
	.shape_set_source		= render_cairo_shape_set_source,
	.shape_set_source_color	= render_cairo_shape_set_source_color,
	.shape_set_line_width	= render_cairo_shape_set_line_width,
	.shape_translate		= render_cairo_shape_translate,
	.shape_scale			= render_cairo_shape_scale,
	.shape_rotate			= render_cairo_shape_rotate,
	.shape_transform		= render_cairo_shape_transform,
	.shape_set_matrix		= render_cairo_shape_set_matrix,
	.shape_identity_matrix	= render_cairo_shape_identity_matrix,
	.shape_new_path			= render_cairo_shape_new_path,
	.shape_close_path		= render_cairo_shape_close_path,
	.shape_move_to			= render_cairo_shape_move_to,
	.shape_line_to			= render_cairo_shape_line_to,
	.shape_curve_to			= render_cairo_shape_curve_to,
	.shape_rectangle		= render_cairo_shape_rectangle,
	.shape_arc				= render_cairo_shape_arc,
	.shape_arc_negative		= render_cairo_shape_arc_negative,
	.shape_circle			= render_cairo_shape_circle,
	.shape_ellipse			= render_cairo_shape_ellipse,
	.shape_clip				= render_cairo_shape_clip,
	.shape_clip_preserve	= render_cairo_shape_clip_preserve,
	.shape_fill				= render_cairo_shape_fill,
	.shape_fill_preserve	= render_cairo_shape_fill_preserve,
	.shape_stroke			= render_cairo_shape_stroke,
	.shape_stroke_preserve	= render_cairo_shape_stroke_preserve,
	.shape_paint			= render_cairo_shape_paint,
};

static __init void render_cairo_init(void)
{
	register_render(&render_cairo);
}

static __exit void render_cairo_exit(void)
{
	unregister_render(&render_cairo);
}

core_initcall(render_cairo_init);
core_exitcall(render_cairo_exit);
