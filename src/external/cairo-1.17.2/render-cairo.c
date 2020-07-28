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

static void render_cairo_blit(struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct surface_t * src, enum render_type_t type)
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
	switch(type)
	{
	case RENDER_TYPE_FAST:
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		break;
	case RENDER_TYPE_GOOD:
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_GOOD);
		break;
	case RENDER_TYPE_BEST:
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_BEST);
		break;
	default:
		break;
	}
	cairo_paint(cr);
	cairo_restore(cr);
}

static void render_cairo_fill(struct surface_t * s, struct region_t * clip, struct matrix_t * m, int w, int h, struct color_t * c, enum render_type_t type)
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
	switch(type)
	{
	case RENDER_TYPE_FAST:
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		break;
	case RENDER_TYPE_GOOD:
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_GOOD);
		break;
	case RENDER_TYPE_BEST:
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_BEST);
		break;
	default:
		break;
	}
	cairo_fill(cr);
	cairo_restore(cr);
}

static void render_cairo_shape_line(struct surface_t * s, struct region_t * clip, struct point_t * p0, struct point_t * p1, int thickness, struct color_t * c)
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
	cairo_move_to(cr, p0->x, p0->y);
	cairo_line_to(cr, p1->x, p1->y);
	cairo_set_source_rgba(cr, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
	cairo_set_line_width(cr, thickness > 0 ? thickness : 1);
	cairo_stroke(cr);
	cairo_restore(cr);
}

static void render_cairo_shape_polyline(struct surface_t * s, struct region_t * clip, struct point_t * p, int n, int thickness, struct color_t * c)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	struct region_t r;
	int i;

	if(p && (n > 0))
	{
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
		cairo_move_to(cr, p[0].x, p[0].y);
		for(i = 1; i < n; i++)
			cairo_line_to(cr, p[i].x, p[i].y);
		cairo_set_source_rgba(cr, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
		cairo_set_line_width(cr, thickness > 0 ? thickness : 1);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

static void render_cairo_shape_curve(struct surface_t * s, struct region_t * clip, struct point_t * p, int n, int thickness, struct color_t * c)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	struct region_t r;
	int i;

	if(p && (n >= 4))
	{
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
		cairo_move_to(cr, p[0].x, p[0].y);
		for(i = 1; i <= n - 3; i += 3)
			cairo_curve_to(cr, p[i].x, p[i].y, p[i + 1].x, p[i + 1].y, p[i + 2].x, p[i + 2].y);
		cairo_set_source_rgba(cr, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
		cairo_set_line_width(cr, thickness > 0 ? thickness : 1);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

static void render_cairo_shape_triangle(struct surface_t * s, struct region_t * clip, struct point_t * p0, struct point_t * p1, struct point_t * p2, int thickness, struct color_t * c)
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
	cairo_move_to(cr, p0->x, p0->y);
	cairo_line_to(cr, p1->x, p1->y);
	cairo_line_to(cr, p2->x, p2->y);
	cairo_close_path(cr);
	cairo_set_source_rgba(cr, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
	if(thickness > 0)
	{
		cairo_set_line_width(cr, thickness);
		cairo_stroke(cr);
	}
	else
	{
		cairo_fill(cr);
	}
	cairo_restore(cr);
}

static void render_cairo_shape_rectangle(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h, int radius, int thickness, struct color_t * c)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	struct region_t r;
	int corner;

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
	corner = (radius >> 16) & 0xf;
	radius &= 0xffff;
	if(radius > 0)
	{
		cairo_move_to(cr, x + radius, y);
		cairo_line_to(cr, x + w - radius, y);
		if(corner & (1 << 1))
		{
			cairo_line_to(cr, x + w, y);
			cairo_line_to(cr, x + w, y + radius);
		}
		else
		{
			cairo_arc(cr, x + w - radius, y + radius, radius, - M_PI_2, 0);
		}
		cairo_line_to(cr, x + w, y + h - radius);
		if(corner & (1 << 2))
		{
			cairo_line_to(cr, x + w, y + h);
			cairo_line_to(cr, w - radius, y + h);
		}
		else
		{
			cairo_arc(cr, x + w - radius, y + h - radius, radius, 0, M_PI_2);
		}
		cairo_line_to(cr, x + radius, y + h);
		if(corner & (1 << 3))
		{
			cairo_line_to(cr, x, y + h);
		}
		else
		{
			cairo_arc(cr, x + radius, y + h - radius, radius, M_PI_2, M_PI);
		}
		if(corner & (1 << 0))
		{
			cairo_line_to(cr, x, y);
			cairo_line_to(cr, x + radius, y);
		}
		else
		{
			cairo_arc(cr, x + radius, y + radius, radius, M_PI, M_PI + M_PI_2);
		}
	}
	else
	{
		cairo_rectangle(cr, x, y, w, h);
	}
	cairo_set_source_rgba(cr, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
	if(thickness > 0)
	{
		cairo_set_line_width(cr, thickness);
		cairo_stroke(cr);
	}
	else
	{
		cairo_fill(cr);
	}
	cairo_restore(cr);
}

static void render_cairo_shape_polygon(struct surface_t * s, struct region_t * clip, struct point_t * p, int n, int thickness, struct color_t * c)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	struct region_t r;
	int i;

	if(p && (n > 0))
	{
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
		cairo_move_to(cr, p[0].x, p[0].y);
		for(i = 1; i < n; i++)
			cairo_line_to(cr, p[i].x, p[i].y);
		cairo_close_path(cr);
		cairo_set_source_rgba(cr, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
		if(thickness > 0)
		{
			cairo_set_line_width(cr, thickness);
			cairo_stroke(cr);
		}
		else
		{
			cairo_fill(cr);
		}
		cairo_restore(cr);
	}
}

static void render_cairo_shape_circle(struct surface_t * s, struct region_t * clip, int x, int y, int radius, int thickness, struct color_t * c)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	struct region_t r;

	if(radius > 0)
	{
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
		cairo_move_to(cr, x + radius, y);
		cairo_arc(cr, x, y, radius, 0, M_PI * 2);
		cairo_set_source_rgba(cr, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
		if(thickness > 0)
		{
			cairo_set_line_width(cr, thickness);
			cairo_stroke(cr);
		}
		else
		{
			cairo_fill(cr);
		}
		cairo_restore(cr);
	}
}

static void render_cairo_shape_ellipse(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h, int thickness, struct color_t * c)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	cairo_matrix_t m;
	struct region_t r;

	if((w > 0) && (h > 0))
	{
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
		cairo_get_matrix(cr, &m);
		cairo_translate(cr, x, y);
		cairo_scale(cr, 1, (double)h / (double)w);
		cairo_translate(cr, -x, -y);
		cairo_move_to(cr, x + w, y);
		cairo_arc(cr, x, y, w, 0, M_PI * 2);
		cairo_close_path(cr);
		cairo_set_matrix(cr, &m);
		cairo_set_source_rgba(cr, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
		if(thickness > 0)
		{
			cairo_set_line_width(cr, thickness);
			cairo_stroke(cr);
		}
		else
		{
			cairo_fill(cr);
		}
		cairo_restore(cr);
	}
}

void render_cairo_shape_arc(struct surface_t * s, struct region_t * clip, int x, int y, int radius, int a1, int a2, int thickness, struct color_t * c)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->rctx)->cr;
	struct region_t r;

	if(radius > 0)
	{
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
		cairo_arc(cr, x, y, radius, a1 * (M_PI / 180.0), a2 * (M_PI / 180.0));
		cairo_set_source_rgba(cr, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
		if(thickness > 0)
		{
			cairo_set_line_width(cr, thickness);
			cairo_stroke(cr);
		}
		else
		{
			cairo_fill(cr);
		}
		cairo_restore(cr);
	}
}

static struct render_t render_cairo = {
	.name	 			= "cairo",

	.create				= render_cairo_create,
	.destroy			= render_cairo_destroy,

	.blit				= render_cairo_blit,
	.fill				= render_cairo_fill,
	.text				= render_default_text,
	.icon				= render_default_icon,

	.shape_line			= render_cairo_shape_line,
	.shape_polyline		= render_cairo_shape_polyline,
	.shape_curve		= render_cairo_shape_curve,
	.shape_triangle		= render_cairo_shape_triangle,
	.shape_rectangle	= render_cairo_shape_rectangle,
	.shape_polygon		= render_cairo_shape_polygon,
	.shape_circle		= render_cairo_shape_circle,
	.shape_ellipse		= render_cairo_shape_ellipse,
	.shape_arc			= render_cairo_shape_arc,
	.shape_gradient		= render_default_shape_gradient,
	.shape_checkerboard	= render_default_shape_checkerboard,
	.shape_raster		= render_default_shape_raster,

	.filter_grayscale	= render_default_filter_grayscale,
	.filter_sepia		= render_default_filter_sepia,
	.filter_invert		= render_default_filter_invert,
	.filter_threshold	= render_default_filter_threshold,
	.filter_colormap	= render_default_filter_colormap,
	.filter_coloring	= render_default_filter_coloring,
	.filter_hue			= render_default_filter_hue,
	.filter_saturate	= render_default_filter_saturate,
	.filter_brightness	= render_default_filter_brightness,
	.filter_contrast	= render_default_filter_contrast,
	.filter_opacity		= render_default_filter_opacity,
	.filter_haldclut	= render_default_filter_haldclut,
	.filter_blur		= render_default_filter_blur,
	.filter_erode		= render_default_filter_erode,
	.filter_dilate		= render_default_filter_dilate,
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
