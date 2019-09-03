/*
 * kernel/graphic/render-cairo.c
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

static void render_cairo_destroy(void * pctx)
{
	struct render_cairo_context_t * ctx = (struct render_cairo_context_t *)pctx;
	cairo_destroy(ctx->cr);
	cairo_surface_destroy(ctx->cs);
	free(ctx);
}

static void render_cairo_blit(struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct surface_t * src, enum render_type_t type)
{
	cairo_t * cr = ((struct render_cairo_context_t *)s->pctx)->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, (cairo_matrix_t *)m);
	cairo_set_source_surface(cr, ((struct render_cairo_context_t *)src->pctx)->cs, 0, 0);
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
	cairo_t * cr = ((struct render_cairo_context_t *)s->pctx)->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, (cairo_matrix_t *)m);
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

static struct render_t render_cairo = {
	.name	 			= "cairo",

	.create				= render_cairo_create,
	.destroy			= render_cairo_destroy,

	.blit				= render_cairo_blit,
	.fill				= render_cairo_fill,
	.text				= render_default_text,
	.raster				= render_default_raster,

	.filter_haldclut	= render_default_filter_haldclut,
	.filter_grayscale	= render_default_filter_grayscale,
	.filter_sepia		= render_default_filter_sepia,
	.filter_invert		= render_default_filter_invert,
	.filter_threshold	= render_default_filter_threshold,
	.filter_colorize	= render_default_filter_colorize,
	.filter_hue			= render_default_filter_hue,
	.filter_saturate	= render_default_filter_saturate,
	.filter_brightness	= render_default_filter_brightness,
	.filter_contrast	= render_default_filter_contrast,
	.filter_opacity		= render_default_filter_opacity,
	.filter_blur		= render_default_filter_blur,
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
