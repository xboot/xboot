/*
 * render-cg.c
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

static void render_cg_blit(struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct surface_t * src, enum render_type_t type)
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

static void render_cg_fill(struct surface_t * s, struct region_t * clip, struct matrix_t * m, int w, int h, struct color_t * c, enum render_type_t type)
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

static void render_cg_shape_line(struct surface_t * s, struct region_t * clip, struct point_t * p0, struct point_t * p1, int thickness, struct color_t * c)
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
	cg_move_to(cg, p0->x, p0->y);
	cg_line_to(cg, p1->x, p1->y);
	cg_set_source_rgba(cg, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
	cg_set_line_width(cg, thickness > 0 ? thickness : 1);
	cg_stroke(cg);
	cg_restore(cg);
}

static void render_cg_shape_polyline(struct surface_t * s, struct region_t * clip, struct point_t * p, int n, int thickness, struct color_t * c)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	struct region_t r;
	int i;

	if(p && (n > 0))
	{
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
		cg_move_to(cg, p[0].x, p[0].y);
		for(i = 1; i < n; i++)
			cg_line_to(cg, p[i].x, p[i].y);
		cg_set_source_rgba(cg, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
		cg_set_line_width(cg, thickness > 0 ? thickness : 1);
		cg_stroke(cg);
		cg_restore(cg);
	}
}

static void render_cg_shape_curve(struct surface_t * s, struct region_t * clip, struct point_t * p, int n, int thickness, struct color_t * c)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	struct region_t r;
	int i;

	if(p && (n >= 4))
	{
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
		cg_move_to(cg, p[0].x, p[0].y);
		for(i = 1; i <= n - 3; i += 3)
			cg_curve_to(cg, p[i].x, p[i].y, p[i + 1].x, p[i + 1].y, p[i + 2].x, p[i + 2].y);
		cg_set_source_rgba(cg, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
		cg_set_line_width(cg, thickness > 0 ? thickness : 1);
		cg_stroke(cg);
		cg_restore(cg);
	}
}

static void render_cg_shape_triangle(struct surface_t * s, struct region_t * clip, struct point_t * p0, struct point_t * p1, struct point_t * p2, int thickness, struct color_t * c)
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
	cg_move_to(cg, p0->x, p0->y);
	cg_line_to(cg, p1->x, p1->y);
	cg_line_to(cg, p2->x, p2->y);
	cg_close_path(cg);
	cg_set_source_rgba(cg, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
	if(thickness > 0)
	{
		cg_set_line_width(cg, thickness);
		cg_stroke(cg);
	}
	else
	{
		cg_fill(cg);
	}
	cg_restore(cg);
}

static void render_cg_shape_rectangle(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h, int radius, int thickness, struct color_t * c)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	struct region_t r;
	int corner;

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
	corner = (radius >> 16) & 0xf;
	radius &= 0xffff;
	if(radius > 0)
	{
		cg_move_to(cg, x + radius, y);
		cg_line_to(cg, x + w - radius, y);
		if(corner & (1 << 1))
		{
			cg_line_to(cg, x + w, y);
			cg_line_to(cg, x + w, y + radius);
		}
		else
		{
			cg_arc(cg, x + w - radius, y + radius, radius, - M_PI_2, 0);
		}
		cg_line_to(cg, x + w, y + h - radius);
		if(corner & (1 << 2))
		{
			cg_line_to(cg, x + w, y + h);
			cg_line_to(cg, w - radius, y + h);
		}
		else
		{
			cg_arc(cg, x + w - radius, y + h - radius, radius, 0, M_PI_2);
		}
		cg_line_to(cg, x + radius, y + h);
		if(corner & (1 << 3))
		{
			cg_line_to(cg, x, y + h);
		}
		else
		{
			cg_arc(cg, x + radius, y + h - radius, radius, M_PI_2, M_PI);
		}
		if(corner & (1 << 0))
		{
			cg_line_to(cg, x, y);
			cg_line_to(cg, x + radius, y);
		}
		else
		{
			cg_arc(cg, x + radius, y + radius, radius, M_PI, M_PI + M_PI_2);
		}
	}
	else
	{
		cg_rectangle(cg, x, y, w, h);
	}
	cg_set_source_rgba(cg, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
	if(thickness > 0)
	{
		cg_set_line_width(cg, thickness);
		cg_stroke(cg);
	}
	else
	{
		cg_fill(cg);
	}
	cg_restore(cg);
}

static void render_cg_shape_polygon(struct surface_t * s, struct region_t * clip, struct point_t * p, int n, int thickness, struct color_t * c)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	struct region_t r;
	int i;

	if(p && (n > 0))
	{
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
		cg_move_to(cg, p[0].x, p[0].y);
		for(i = 1; i < n; i++)
			cg_line_to(cg, p[i].x, p[i].y);
		cg_close_path(cg);
		cg_set_source_rgba(cg, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
		if(thickness > 0)
		{
			cg_set_line_width(cg, thickness);
			cg_stroke(cg);
		}
		else
		{
			cg_fill(cg);
		}
		cg_restore(cg);
	}
}

static void render_cg_shape_circle(struct surface_t * s, struct region_t * clip, int x, int y, int radius, int thickness, struct color_t * c)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	struct region_t r;

	if(radius > 0)
	{
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
		cg_circle(cg, x, y, radius);
		cg_set_source_rgba(cg, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
		if(thickness > 0)
		{
			cg_set_line_width(cg, thickness);
			cg_stroke(cg);
		}
		else
		{
			cg_fill(cg);
		}
		cg_restore(cg);
	}
}

static void render_cg_shape_ellipse(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h, int thickness, struct color_t * c)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	struct region_t r;

	if((w > 0) && (h > 0))
	{
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
		cg_ellipse(cg, x, y, w, h);
		cg_set_source_rgba(cg, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
		if(thickness > 0)
		{
			cg_set_line_width(cg, thickness);
			cg_stroke(cg);
		}
		else
		{
			cg_fill(cg);
		}
		cg_restore(cg);
	}
}

void render_cg_shape_arc(struct surface_t * s, struct region_t * clip, int x, int y, int radius, int a1, int a2, int thickness, struct color_t * c)
{
	struct cg_ctx_t * cg = ((struct render_cg_ctx_t *)s->rctx)->cg;
	struct region_t r;

	if(radius > 0)
	{
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
		cg_arc(cg, x, y, radius, a1 * (M_PI / 180.0), a2 * (M_PI / 180.0));
		cg_set_source_rgba(cg, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
		if(thickness > 0)
		{
			cg_set_line_width(cg, thickness);
			cg_stroke(cg);
		}
		else
		{
			cg_fill(cg);
		}
		cg_restore(cg);
	}
}

static struct render_t render_cg = {
	.name	 				= "cg",

	.create					= render_cg_create,
	.destroy				= render_cg_destroy,

	.blit					= render_cg_blit,
	.fill					= render_cg_fill,
	.text					= render_default_text,
	.icon					= render_default_icon,

	.shape_line				= render_cg_shape_line,
	.shape_polyline			= render_cg_shape_polyline,
	.shape_curve			= render_cg_shape_curve,
	.shape_triangle			= render_cg_shape_triangle,
	.shape_rectangle		= render_cg_shape_rectangle,
	.shape_polygon			= render_cg_shape_polygon,
	.shape_circle			= render_cg_shape_circle,
	.shape_ellipse			= render_cg_shape_ellipse,
	.shape_arc				= render_cg_shape_arc,

	.effect_glass			= render_default_effect_glass,
	.effect_gradient		= render_default_effect_gradient,
	.effect_checkerboard	= render_default_effect_checkerboard,

	.filter_gray			= render_default_filter_gray,
	.filter_sepia			= render_default_filter_sepia,
	.filter_invert			= render_default_filter_invert,
	.filter_coloring		= render_default_filter_coloring,
	.filter_hue				= render_default_filter_hue,
	.filter_saturate		= render_default_filter_saturate,
	.filter_brightness		= render_default_filter_brightness,
	.filter_contrast		= render_default_filter_contrast,
	.filter_opacity			= render_default_filter_opacity,
	.filter_haldclut		= render_default_filter_haldclut,
	.filter_blur			= render_default_filter_blur,
};

static __init void render_cg_init(void)
{
	register_render(&render_cg);
}

static __exit void render_cg_exit(void)
{
	unregister_render(&render_cg);
}

core_initcall(render_cg_init);
core_exitcall(render_cg_exit);
