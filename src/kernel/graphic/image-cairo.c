/*
 * kernel/graphic/image-backend-cairo.c
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
#include <graphic/image.h>

struct image_cairo_context_t {
	cairo_surface_t * cs;
	cairo_t * cr;
};

static void * image_cairo_create(struct image_t * img)
{
	struct image_cairo_context_t * ctx;

	ctx = malloc(sizeof(struct image_cairo_context_t));
	if(!ctx)
		return NULL;
	ctx->cs = cairo_image_surface_create_for_data((unsigned char *)img->pixels, CAIRO_FORMAT_ARGB32, img->width, img->height, img->stride);
	ctx->cr = cairo_create(ctx->cs);
	return ctx;
}

static void image_cairo_destroy(void * priv)
{
	struct image_cairo_context_t * ctx = (struct image_cairo_context_t *)priv;
	cairo_destroy(ctx->cr);
	cairo_surface_destroy(ctx->cs);
	free(ctx);
}

static void image_cairo_blit(struct image_t * img, struct matrix_t * m, struct image_t * src, double alpha)
{
	cairo_t * cr = ((struct image_cairo_context_t *)img->priv)->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, (cairo_matrix_t *)m);
	cairo_set_source_surface(cr, ((struct image_cairo_context_t *)src->priv)->cs, 0, 0);
	cairo_paint_with_alpha(cr, alpha);
	cairo_restore(cr);
}

static void image_cairo_mask(struct image_t * img, struct matrix_t * m, struct image_t * src, struct image_t * mask)
{
	cairo_t * cr = ((struct image_cairo_context_t *)img->priv)->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, (cairo_matrix_t *)m);
	cairo_set_source_surface(cr, ((struct image_cairo_context_t *)src->priv)->cs, 0, 0);
	cairo_mask_surface(cr, ((struct image_cairo_context_t *)mask->priv)->cs, 0, 0);
	cairo_restore(cr);
}

static void image_cairo_fill(struct image_t * img, struct matrix_t * m, double x, double y, double w, double h, double r, double g, double b, double a)
{
	cairo_t * cr = ((struct image_cairo_context_t *)img->priv)->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, (cairo_matrix_t *)m);
	cairo_rectangle(cr, x, y, w, h);
	cairo_set_source_rgba(cr, r, g, b, a);
	cairo_fill(cr);
	cairo_restore(cr);
}

static void image_cairo_shape_save(struct image_t * img)
{
	cairo_t * cr = ((struct image_cairo_context_t *)img->priv)->cr;
	cairo_save(cr);
}

static void image_cairo_shape_restore(struct image_t * img)
{
	cairo_t * cr = ((struct image_cairo_context_t *)img->priv)->cr;
	cairo_restore(cr);
}

struct image_operate_t image_operate_cairo = {
	.create			= image_cairo_create,
	.destroy		= image_cairo_destroy,

	.blit			= image_cairo_blit,
	.mask			= image_cairo_mask,
	.fill			= image_cairo_fill,

	.shape_save		= image_cairo_shape_save,
	.shape_restore	= image_cairo_shape_restore,
};
