/*
 * kernel/graphic/surface-cairo.c
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

struct surface_cairo_context_t {
	cairo_surface_t * cs;
	cairo_t * cr;
};

static void * surface_cairo_create(struct surface_t * s)
{
	struct surface_cairo_context_t * ctx;

	ctx = malloc(sizeof(struct surface_cairo_context_t));
	if(!ctx)
		return NULL;
	ctx->cs = cairo_image_surface_create_for_data((unsigned char *)s->pixels, CAIRO_FORMAT_ARGB32, s->width, s->height, s->stride);
	ctx->cr = cairo_create(ctx->cs);
	return ctx;
}

static void surface_cairo_destroy(void * pctx)
{
	struct surface_cairo_context_t * ctx = (struct surface_cairo_context_t *)pctx;
	cairo_destroy(ctx->cr);
	cairo_surface_destroy(ctx->cs);
	free(ctx);
}

static void surface_cairo_blit(struct surface_t * s, struct matrix_t * m, struct surface_t * src, double alpha)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, (cairo_matrix_t *)m);
	cairo_set_source_surface(cr, ((struct surface_cairo_context_t *)src->pctx)->cs, 0, 0);
	cairo_paint_with_alpha(cr, alpha);
	cairo_restore(cr);
}

static void surface_cairo_mask(struct surface_t * s, struct matrix_t * m, struct surface_t * src, struct surface_t * mask)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, (cairo_matrix_t *)m);
	cairo_set_source_surface(cr, ((struct surface_cairo_context_t *)src->pctx)->cs, 0, 0);
	cairo_mask_surface(cr, ((struct surface_cairo_context_t *)mask->pctx)->cs, 0, 0);
	cairo_restore(cr);
}

static void surface_cairo_fill(struct surface_t * s, struct matrix_t * m, double x, double y, double w, double h, struct color_t * c)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, (cairo_matrix_t *)m);
	cairo_rectangle(cr, x, y, w, h);
	cairo_set_source_rgba(cr, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
	cairo_fill(cr);
	cairo_restore(cr);
}

static void surface_cairo_text(struct surface_t * s, struct matrix_t * m, const char * utf8, struct color_t * c, void * sfont, double size)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_text_extents_t e;
	cairo_save(cr);
	cairo_set_font_face(cr, (cairo_font_face_t *)sfont);
	cairo_set_font_size(cr, size);
	cairo_move_to(cr, m->tx, m->ty);
	cairo_set_matrix(cr, (cairo_matrix_t *)m);
	cairo_text_extents(cr, utf8, &e);
	cairo_move_to(cr, e.x_bearing, -e.y_bearing);
	cairo_set_source_rgba(cr, c->r / 255.0, c->g / 255.0, c->b / 255.0, c->a / 255.0);
	cairo_show_text(cr, utf8);
	cairo_restore(cr);
}

static void surface_cairo_extent(struct surface_t * s, const char * utf8, void * sfont, double size, double * x, double * y, double * w, double * h)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_text_extents_t e;
	cairo_save(cr);
	cairo_set_font_face(cr, (cairo_font_face_t *)sfont);
	cairo_set_font_size(cr, size);
	cairo_text_extents(cr, utf8, &e);
	cairo_restore(cr);
	*x = e.x_bearing;
	*y = -e.y_bearing;
	*w = e.width;
	*h = e.height;
}

static void surface_cairo_filter_grayscale(struct surface_t * s)
{
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	int stride = surface_get_stride(s);
	unsigned char * p, * q = surface_get_pixels(s);
	unsigned char gray;
	int r, g, b;
	int x, y;

	for(y = 0; y < height; y++, q += stride)
	{
		for(x = 0, p = q; x < width; x++, p += 4)
		{
			b = p[0];
			g = p[1];
			r = p[2];
			gray = (r * 19595 + g * 38469 + b * 7472) >> 16;
			p[0] = gray;
			p[1] = gray;
			p[2] = gray;
		}
	}
}

static void surface_cairo_filter_sepia(struct surface_t * s)
{
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	int stride = surface_get_stride(s);
	unsigned char * p, *q = surface_get_pixels(s);
	int r, g, b;
	int tr, tg, tb;
	int x, y;

	for(y = 0; y < height; y++, q += stride)
	{
		for(x = 0, p = q; x < width; x++, p += 4)
		{
			b = p[0];
			g = p[1];
			r = p[2];
			tb = (r * 17826 + g * 34996 + b * 8585) >> 16;
			tg = (r * 22872 + g * 44958 + b * 11010) >> 16;
			tr = (r * 25756 + g * 50397 + b * 12386) >> 16;
			p[0] = min(tb, 255);
			p[1] = min(tg, 255);
			p[2] = min(tr, 255);
		}
	}
}

static void surface_cairo_filter_invert(struct surface_t * s)
{
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	int stride = surface_get_stride(s);
	unsigned char * p, *q = surface_get_pixels(s);
	int x, y;
	for(y = 0; y < height; y++, q += stride)
	{
		for(x = 0, p = q; x < width; x++, p += 4)
		{
			p[0] = 255 - p[0];
			p[1] = 255 - p[1];
			p[2] = 255 - p[2];
		}
	}
}

static void surface_cairo_filter_threshold(struct surface_t * s, const char * type, int threshold, int value)
{
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	int stride = surface_get_stride(s);
	unsigned char * p, *q = surface_get_pixels(s);
	int x, y;
	threshold = clamp(threshold, 0, 255);
	value = clamp(value, 0, 255);

	switch(shash(type))
	{
	case 0xf4229cca: /* "binary" */
		for(y = 0; y < height; y++, q += stride)
		{
			for(x = 0, p = q; x < width; x++, p += 4)
			{
				p[2] = p[1] = p[0] = (p[0] > threshold) ? value : 0;
			}
		}
		break;
	case 0xc880666f: /* "binary-invert" */
		for(y = 0; y < height; y++, q += stride)
		{
			for(x = 0, p = q; x < width; x++, p += 4)
			{
				p[2] = p[1] = p[0] = (p[0] > threshold) ? 0 : value;
			}
		}
		break;
	case 0x1e92b0a8: /* "tozero" */
		for(y = 0; y < height; y++, q += stride)
		{
			for(x = 0, p = q; x < width; x++, p += 4)
			{
				p[2] = p[1] = p[0] = (p[0] > threshold) ? p[0] : 0;
			}
		}
		break;
	case 0x98d3b48d: /* "tozero-invert" */
		for(y = 0; y < height; y++, q += stride)
		{
			for(x = 0, p = q; x < width; x++, p += 4)
			{
				p[2] = p[1] = p[0] = (p[0] > threshold) ? 0 : p[0];
			}
		}
		break;
	case 0x10729e11: /* "trunc" */
		for(y = 0; y < height; y++, q += stride)
		{
			for(x = 0, p = q; x < width; x++, p += 4)
			{
				p[2] = p[1] = p[0] = (p[0] > threshold) ? threshold : value;
			}
		}
		break;
	default:
		break;
	}
}

static const unsigned char colormap_parula[256][3] = {
	{ 0x44, 0x08, 0x9d }, { 0x43, 0x0c, 0x9e }, { 0x42, 0x0f, 0x9f }, { 0x42, 0x12, 0xa0 }, { 0x41, 0x15, 0xa1 }, { 0x40, 0x17, 0xa2 }, { 0x40, 0x1a, 0xa2 }, { 0x3f, 0x1c, 0xa3 },
	{ 0x3e, 0x1f, 0xa3 }, { 0x3d, 0x21, 0xa3 }, { 0x3c, 0x23, 0xa4 }, { 0x3b, 0x25, 0xa4 }, { 0x3a, 0x28, 0xa4 }, { 0x39, 0x2a, 0xa4 }, { 0x38, 0x2c, 0xa4 }, { 0x37, 0x2e, 0xa3 },
	{ 0x35, 0x30, 0xa3 }, { 0x34, 0x32, 0xa3 }, { 0x33, 0x34, 0xa2 }, { 0x32, 0x35, 0xa2 }, { 0x31, 0x37, 0xa1 }, { 0x30, 0x39, 0xa0 }, { 0x2f, 0x3b, 0x9f }, { 0x2d, 0x3c, 0x9f },
	{ 0x2c, 0x3e, 0x9e }, { 0x2b, 0x3f, 0x9d }, { 0x2a, 0x41, 0x9c }, { 0x29, 0x42, 0x9b }, { 0x29, 0x44, 0x9a }, { 0x28, 0x45, 0x99 }, { 0x27, 0x47, 0x98 }, { 0x26, 0x48, 0x97 },
	{ 0x26, 0x49, 0x96 }, { 0x25, 0x4a, 0x95 }, { 0x25, 0x4c, 0x94 }, { 0x24, 0x4d, 0x94 }, { 0x24, 0x4e, 0x93 }, { 0x24, 0x4f, 0x92 }, { 0x23, 0x50, 0x91 }, { 0x23, 0x51, 0x90 },
	{ 0x23, 0x52, 0x8f }, { 0x23, 0x53, 0x8e }, { 0x23, 0x54, 0x8e }, { 0x23, 0x55, 0x8d }, { 0x23, 0x56, 0x8c }, { 0x23, 0x57, 0x8b }, { 0x23, 0x58, 0x8b }, { 0x23, 0x59, 0x8a },
	{ 0x24, 0x5a, 0x89 }, { 0x24, 0x5b, 0x89 }, { 0x24, 0x5c, 0x88 }, { 0x25, 0x5d, 0x88 }, { 0x25, 0x5e, 0x87 }, { 0x25, 0x5f, 0x86 }, { 0x26, 0x60, 0x86 }, { 0x26, 0x61, 0x85 },
	{ 0x26, 0x61, 0x85 }, { 0x27, 0x62, 0x84 }, { 0x27, 0x63, 0x84 }, { 0x28, 0x64, 0x83 }, { 0x28, 0x65, 0x83 }, { 0x29, 0x66, 0x83 }, { 0x29, 0x66, 0x82 }, { 0x2a, 0x67, 0x82 },
	{ 0x2a, 0x68, 0x81 }, { 0x2b, 0x69, 0x81 }, { 0x2b, 0x6a, 0x81 }, { 0x2b, 0x6b, 0x80 }, { 0x2c, 0x6b, 0x80 }, { 0x2c, 0x6c, 0x80 }, { 0x2d, 0x6d, 0x80 }, { 0x2d, 0x6e, 0x7f },
	{ 0x2d, 0x6f, 0x7f }, { 0x2e, 0x70, 0x7f }, { 0x2e, 0x70, 0x7e }, { 0x2e, 0x71, 0x7e }, { 0x2f, 0x72, 0x7e }, { 0x2f, 0x73, 0x7e }, { 0x2f, 0x74, 0x7d }, { 0x30, 0x75, 0x7d },
	{ 0x30, 0x75, 0x7d }, { 0x30, 0x76, 0x7d }, { 0x30, 0x77, 0x7d }, { 0x30, 0x78, 0x7c }, { 0x30, 0x79, 0x7c }, { 0x31, 0x7a, 0x7c }, { 0x31, 0x7a, 0x7c }, { 0x31, 0x7b, 0x7b },
	{ 0x31, 0x7c, 0x7b }, { 0x31, 0x7d, 0x7b }, { 0x31, 0x7e, 0x7b }, { 0x31, 0x7f, 0x7a }, { 0x31, 0x80, 0x7a }, { 0x31, 0x81, 0x7a }, { 0x31, 0x81, 0x79 }, { 0x30, 0x82, 0x79 },
	{ 0x30, 0x83, 0x79 }, { 0x30, 0x84, 0x79 }, { 0x30, 0x85, 0x78 }, { 0x30, 0x86, 0x78 }, { 0x30, 0x87, 0x77 }, { 0x2f, 0x88, 0x77 }, { 0x2f, 0x89, 0x77 }, { 0x2f, 0x89, 0x76 },
	{ 0x2f, 0x8a, 0x76 }, { 0x2e, 0x8b, 0x75 }, { 0x2e, 0x8c, 0x75 }, { 0x2e, 0x8d, 0x74 }, { 0x2d, 0x8e, 0x74 }, { 0x2d, 0x8f, 0x73 }, { 0x2d, 0x90, 0x73 }, { 0x2c, 0x91, 0x72 },
	{ 0x2c, 0x92, 0x72 }, { 0x2c, 0x93, 0x71 }, { 0x2b, 0x94, 0x70 }, { 0x2b, 0x95, 0x70 }, { 0x2b, 0x95, 0x6f }, { 0x2b, 0x96, 0x6e }, { 0x2a, 0x97, 0x6d }, { 0x2a, 0x98, 0x6d },
	{ 0x2a, 0x99, 0x6c }, { 0x2a, 0x9a, 0x6b }, { 0x29, 0x9b, 0x6a }, { 0x29, 0x9c, 0x69 }, { 0x29, 0x9d, 0x68 }, { 0x29, 0x9e, 0x67 }, { 0x29, 0x9f, 0x66 }, { 0x29, 0xa0, 0x65 },
	{ 0x29, 0xa1, 0x64 }, { 0x2a, 0xa1, 0x63 }, { 0x2a, 0xa2, 0x62 }, { 0x2a, 0xa3, 0x61 }, { 0x2b, 0xa4, 0x60 }, { 0x2b, 0xa5, 0x5e }, { 0x2c, 0xa6, 0x5d }, { 0x2c, 0xa7, 0x5c },
	{ 0x2d, 0xa8, 0x5a }, { 0x2e, 0xa8, 0x59 }, { 0x2f, 0xa9, 0x58 }, { 0x30, 0xaa, 0x56 }, { 0x31, 0xab, 0x55 }, { 0x33, 0xac, 0x53 }, { 0x34, 0xad, 0x52 }, { 0x35, 0xad, 0x50 },
	{ 0x37, 0xae, 0x4e }, { 0x39, 0xaf, 0x4d }, { 0x3b, 0xb0, 0x4b }, { 0x3c, 0xb1, 0x49 }, { 0x3e, 0xb1, 0x47 }, { 0x41, 0xb2, 0x46 }, { 0x43, 0xb3, 0x44 }, { 0x45, 0xb3, 0x42 },
	{ 0x48, 0xb4, 0x40 }, { 0x4a, 0xb5, 0x3e }, { 0x4d, 0xb5, 0x3c }, { 0x4f, 0xb6, 0x3a }, { 0x52, 0xb6, 0x37 }, { 0x55, 0xb7, 0x35 }, { 0x58, 0xb7, 0x33 }, { 0x5b, 0xb8, 0x31 },
	{ 0x5e, 0xb8, 0x2f }, { 0x61, 0xb9, 0x2d }, { 0x64, 0xb9, 0x2b }, { 0x67, 0xba, 0x29 }, { 0x6b, 0xba, 0x27 }, { 0x6e, 0xba, 0x25 }, { 0x71, 0xbb, 0x23 }, { 0x74, 0xbb, 0x22 },
	{ 0x77, 0xbb, 0x20 }, { 0x7b, 0xbc, 0x1f }, { 0x7e, 0xbc, 0x1e }, { 0x81, 0xbc, 0x1d }, { 0x84, 0xbc, 0x1c }, { 0x87, 0xbd, 0x1c }, { 0x8a, 0xbd, 0x1b }, { 0x8d, 0xbd, 0x1b },
	{ 0x90, 0xbd, 0x1b }, { 0x93, 0xbd, 0x1b }, { 0x95, 0xbe, 0x1c }, { 0x98, 0xbe, 0x1c }, { 0x9b, 0xbe, 0x1d }, { 0x9e, 0xbe, 0x1e }, { 0xa0, 0xbe, 0x1f }, { 0xa3, 0xbf, 0x20 },
	{ 0xa5, 0xbf, 0x21 }, { 0xa8, 0xbf, 0x22 }, { 0xaa, 0xbf, 0x23 }, { 0xad, 0xbf, 0x24 }, { 0xaf, 0xc0, 0x25 }, { 0xb2, 0xc0, 0x27 }, { 0xb4, 0xc0, 0x28 }, { 0xb7, 0xc0, 0x29 },
	{ 0xb9, 0xc1, 0x2b }, { 0xbb, 0xc1, 0x2c }, { 0xbe, 0xc1, 0x2d }, { 0xc0, 0xc1, 0x2f }, { 0xc2, 0xc1, 0x30 }, { 0xc4, 0xc2, 0x32 }, { 0xc7, 0xc2, 0x33 }, { 0xc9, 0xc2, 0x34 },
	{ 0xcb, 0xc2, 0x36 }, { 0xcd, 0xc2, 0x37 }, { 0xcf, 0xc3, 0x39 }, { 0xd2, 0xc3, 0x3a }, { 0xd4, 0xc3, 0x3b }, { 0xd6, 0xc3, 0x3d }, { 0xd8, 0xc4, 0x3e }, { 0xda, 0xc4, 0x40 },
	{ 0xdc, 0xc4, 0x41 }, { 0xde, 0xc4, 0x42 }, { 0xe1, 0xc4, 0x44 }, { 0xe3, 0xc5, 0x45 }, { 0xe5, 0xc5, 0x46 }, { 0xe7, 0xc5, 0x48 }, { 0xe9, 0xc5, 0x49 }, { 0xeb, 0xc5, 0x4a },
	{ 0xed, 0xc6, 0x4c }, { 0xef, 0xc6, 0x4d }, { 0xf1, 0xc6, 0x4e }, { 0xf3, 0xc6, 0x50 }, { 0xf6, 0xc6, 0x51 }, { 0xf8, 0xc7, 0x52 }, { 0xfa, 0xc7, 0x53 }, { 0xfc, 0xc7, 0x53 },
	{ 0xfd, 0xc8, 0x51 }, { 0xfd, 0xc9, 0x50 }, { 0xfe, 0xca, 0x4f }, { 0xfe, 0xcc, 0x4e }, { 0xfe, 0xcd, 0x4d }, { 0xfe, 0xce, 0x4c }, { 0xfe, 0xd0, 0x4b }, { 0xfe, 0xd1, 0x4a },
	{ 0xfe, 0xd3, 0x49 }, { 0xfe, 0xd4, 0x48 }, { 0xfe, 0xd5, 0x47 }, { 0xfe, 0xd7, 0x46 }, { 0xfe, 0xd8, 0x45 }, { 0xfe, 0xda, 0x44 }, { 0xfe, 0xdb, 0x43 }, { 0xfe, 0xdc, 0x41 },
	{ 0xfe, 0xde, 0x40 }, { 0xfd, 0xdf, 0x3f }, { 0xfd, 0xe1, 0x3d }, { 0xfd, 0xe2, 0x3c }, { 0xfd, 0xe3, 0x3a }, { 0xfd, 0xe5, 0x38 }, { 0xfd, 0xe6, 0x37 }, { 0xfd, 0xe8, 0x35 },
	{ 0xfd, 0xe9, 0x33 }, { 0xfc, 0xeb, 0x31 }, { 0xfc, 0xec, 0x2e }, { 0xfc, 0xee, 0x2c }, { 0xfc, 0xef, 0x29 }, { 0xfc, 0xf1, 0x27 }, { 0xfb, 0xf2, 0x23 }, { 0xfb, 0xf4, 0x20 },
};

static const unsigned char colormap_jet[256][3] = {
	{ 0x00, 0x00, 0x7f }, { 0x00, 0x00, 0x83 }, { 0x00, 0x00, 0x87 }, { 0x00, 0x00, 0x8b }, { 0x00, 0x00, 0x8f }, { 0x00, 0x00, 0x93 }, { 0x00, 0x00, 0x97 }, { 0x00, 0x00, 0x9b },
	{ 0x00, 0x00, 0x9f }, { 0x00, 0x00, 0xa3 }, { 0x00, 0x00, 0xa7 }, { 0x00, 0x00, 0xab }, { 0x00, 0x00, 0xaf }, { 0x00, 0x00, 0xb3 }, { 0x00, 0x00, 0xb7 }, { 0x00, 0x00, 0xbb },
	{ 0x00, 0x00, 0xbf }, { 0x00, 0x00, 0xc3 }, { 0x00, 0x00, 0xc7 }, { 0x00, 0x00, 0xcb }, { 0x00, 0x00, 0xcf }, { 0x00, 0x00, 0xd3 }, { 0x00, 0x00, 0xd7 }, { 0x00, 0x00, 0xdb },
	{ 0x00, 0x00, 0xdf }, { 0x00, 0x00, 0xe3 }, { 0x00, 0x00, 0xe7 }, { 0x00, 0x00, 0xeb }, { 0x00, 0x00, 0xef }, { 0x00, 0x00, 0xf3 }, { 0x00, 0x00, 0xf7 }, { 0x00, 0x00, 0xfb },
	{ 0x00, 0x00, 0xff }, { 0x00, 0x04, 0xff }, { 0x00, 0x08, 0xff }, { 0x00, 0x0c, 0xff }, { 0x00, 0x10, 0xff }, { 0x00, 0x14, 0xff }, { 0x00, 0x18, 0xff }, { 0x00, 0x1c, 0xff },
	{ 0x00, 0x20, 0xff }, { 0x00, 0x24, 0xff }, { 0x00, 0x28, 0xff }, { 0x00, 0x2c, 0xff }, { 0x00, 0x30, 0xff }, { 0x00, 0x34, 0xff }, { 0x00, 0x38, 0xff }, { 0x00, 0x3c, 0xff },
	{ 0x00, 0x40, 0xff }, { 0x00, 0x44, 0xff }, { 0x00, 0x48, 0xff }, { 0x00, 0x4c, 0xff }, { 0x00, 0x50, 0xff }, { 0x00, 0x54, 0xff }, { 0x00, 0x58, 0xff }, { 0x00, 0x5c, 0xff },
	{ 0x00, 0x60, 0xff }, { 0x00, 0x64, 0xff }, { 0x00, 0x68, 0xff }, { 0x00, 0x6c, 0xff }, { 0x00, 0x70, 0xff }, { 0x00, 0x74, 0xff }, { 0x00, 0x78, 0xff }, { 0x00, 0x7c, 0xff },
	{ 0x00, 0x80, 0xff }, { 0x00, 0x84, 0xff }, { 0x00, 0x88, 0xff }, { 0x00, 0x8c, 0xff }, { 0x00, 0x90, 0xff }, { 0x00, 0x94, 0xff }, { 0x00, 0x98, 0xff }, { 0x00, 0x9c, 0xff },
	{ 0x00, 0xa0, 0xff }, { 0x00, 0xa4, 0xff }, { 0x00, 0xa8, 0xff }, { 0x00, 0xac, 0xff }, { 0x00, 0xb0, 0xff }, { 0x00, 0xb4, 0xff }, { 0x00, 0xb8, 0xff }, { 0x00, 0xbc, 0xff },
	{ 0x00, 0xc0, 0xff }, { 0x00, 0xc4, 0xff }, { 0x00, 0xc8, 0xff }, { 0x00, 0xcc, 0xff }, { 0x00, 0xd0, 0xff }, { 0x00, 0xd4, 0xff }, { 0x00, 0xd8, 0xff }, { 0x00, 0xdc, 0xff },
	{ 0x00, 0xe0, 0xff }, { 0x00, 0xe4, 0xff }, { 0x00, 0xe8, 0xff }, { 0x00, 0xec, 0xff }, { 0x00, 0xf0, 0xff }, { 0x00, 0xf4, 0xff }, { 0x00, 0xf8, 0xff }, { 0x00, 0xfc, 0xff },
	{ 0x01, 0xff, 0xfd }, { 0x05, 0xff, 0xf9 }, { 0x09, 0xff, 0xf5 }, { 0x0d, 0xff, 0xf1 }, { 0x11, 0xff, 0xed }, { 0x15, 0xff, 0xe9 }, { 0x19, 0xff, 0xe5 }, { 0x1d, 0xff, 0xe1 },
	{ 0x21, 0xff, 0xdd }, { 0x25, 0xff, 0xd9 }, { 0x29, 0xff, 0xd5 }, { 0x2d, 0xff, 0xd1 }, { 0x31, 0xff, 0xcd }, { 0x35, 0xff, 0xc9 }, { 0x39, 0xff, 0xc5 }, { 0x3d, 0xff, 0xc1 },
	{ 0x41, 0xff, 0xbd }, { 0x45, 0xff, 0xb9 }, { 0x49, 0xff, 0xb5 }, { 0x4d, 0xff, 0xb1 }, { 0x51, 0xff, 0xad }, { 0x55, 0xff, 0xa9 }, { 0x59, 0xff, 0xa5 }, { 0x5d, 0xff, 0xa1 },
	{ 0x61, 0xff, 0x9d }, { 0x65, 0xff, 0x99 }, { 0x69, 0xff, 0x95 }, { 0x6d, 0xff, 0x91 }, { 0x71, 0xff, 0x8d }, { 0x75, 0xff, 0x89 }, { 0x79, 0xff, 0x85 }, { 0x7d, 0xff, 0x81 },
	{ 0x81, 0xff, 0x7d }, { 0x85, 0xff, 0x79 }, { 0x89, 0xff, 0x75 }, { 0x8d, 0xff, 0x71 }, { 0x91, 0xff, 0x6d }, { 0x95, 0xff, 0x69 }, { 0x99, 0xff, 0x65 }, { 0x9d, 0xff, 0x61 },
	{ 0xa1, 0xff, 0x5d }, { 0xa5, 0xff, 0x59 }, { 0xa9, 0xff, 0x55 }, { 0xad, 0xff, 0x51 }, { 0xb1, 0xff, 0x4d }, { 0xb5, 0xff, 0x49 }, { 0xb9, 0xff, 0x45 }, { 0xbd, 0xff, 0x41 },
	{ 0xc1, 0xff, 0x3d }, { 0xc5, 0xff, 0x39 }, { 0xc9, 0xff, 0x35 }, { 0xcd, 0xff, 0x31 }, { 0xd1, 0xff, 0x2d }, { 0xd5, 0xff, 0x29 }, { 0xd9, 0xff, 0x25 }, { 0xdd, 0xff, 0x21 },
	{ 0xe1, 0xff, 0x1d }, { 0xe5, 0xff, 0x19 }, { 0xe9, 0xff, 0x15 }, { 0xed, 0xff, 0x11 }, { 0xf1, 0xff, 0x0d }, { 0xf5, 0xff, 0x09 }, { 0xf9, 0xff, 0x05 }, { 0xfd, 0xff, 0x01 },
	{ 0xff, 0xfc, 0x00 }, { 0xff, 0xf8, 0x00 }, { 0xff, 0xf4, 0x00 }, { 0xff, 0xf0, 0x00 }, { 0xff, 0xec, 0x00 }, { 0xff, 0xe8, 0x00 }, { 0xff, 0xe4, 0x00 }, { 0xff, 0xe0, 0x00 },
	{ 0xff, 0xdc, 0x00 }, { 0xff, 0xd8, 0x00 }, { 0xff, 0xd4, 0x00 }, { 0xff, 0xd0, 0x00 }, { 0xff, 0xcc, 0x00 }, { 0xff, 0xc8, 0x00 }, { 0xff, 0xc4, 0x00 }, { 0xff, 0xc0, 0x00 },
	{ 0xff, 0xbc, 0x00 }, { 0xff, 0xb8, 0x00 }, { 0xff, 0xb4, 0x00 }, { 0xff, 0xb0, 0x00 }, { 0xff, 0xac, 0x00 }, { 0xff, 0xa8, 0x00 }, { 0xff, 0xa4, 0x00 }, { 0xff, 0xa0, 0x00 },
	{ 0xff, 0x9c, 0x00 }, { 0xff, 0x98, 0x00 }, { 0xff, 0x94, 0x00 }, { 0xff, 0x90, 0x00 }, { 0xff, 0x8c, 0x00 }, { 0xff, 0x88, 0x00 }, { 0xff, 0x84, 0x00 }, { 0xff, 0x80, 0x00 },
	{ 0xff, 0x7c, 0x00 }, { 0xff, 0x78, 0x00 }, { 0xff, 0x74, 0x00 }, { 0xff, 0x70, 0x00 }, { 0xff, 0x6c, 0x00 }, { 0xff, 0x68, 0x00 }, { 0xff, 0x64, 0x00 }, { 0xff, 0x60, 0x00 },
	{ 0xff, 0x5c, 0x00 }, { 0xff, 0x58, 0x00 }, { 0xff, 0x54, 0x00 }, { 0xff, 0x50, 0x00 }, { 0xff, 0x4c, 0x00 }, { 0xff, 0x48, 0x00 }, { 0xff, 0x44, 0x00 }, { 0xff, 0x40, 0x00 },
	{ 0xff, 0x3c, 0x00 }, { 0xff, 0x38, 0x00 }, { 0xff, 0x34, 0x00 }, { 0xff, 0x30, 0x00 }, { 0xff, 0x2c, 0x00 }, { 0xff, 0x28, 0x00 }, { 0xff, 0x24, 0x00 }, { 0xff, 0x20, 0x00 },
	{ 0xff, 0x1c, 0x00 }, { 0xff, 0x18, 0x00 }, { 0xff, 0x14, 0x00 }, { 0xff, 0x10, 0x00 }, { 0xff, 0x0c, 0x00 }, { 0xff, 0x08, 0x00 }, { 0xff, 0x04, 0x00 }, { 0xff, 0x00, 0x00 },
	{ 0xfb, 0x00, 0x00 }, { 0xf7, 0x00, 0x00 }, { 0xf3, 0x00, 0x00 }, { 0xef, 0x00, 0x00 }, { 0xeb, 0x00, 0x00 }, { 0xe7, 0x00, 0x00 }, { 0xe3, 0x00, 0x00 }, { 0xdf, 0x00, 0x00 },
	{ 0xdb, 0x00, 0x00 }, { 0xd7, 0x00, 0x00 }, { 0xd3, 0x00, 0x00 }, { 0xcf, 0x00, 0x00 }, { 0xcb, 0x00, 0x00 }, { 0xc7, 0x00, 0x00 }, { 0xc3, 0x00, 0x00 }, { 0xbf, 0x00, 0x00 },
	{ 0xbb, 0x00, 0x00 }, { 0xb7, 0x00, 0x00 }, { 0xb3, 0x00, 0x00 }, { 0xaf, 0x00, 0x00 }, { 0xab, 0x00, 0x00 }, { 0xa7, 0x00, 0x00 }, { 0xa3, 0x00, 0x00 }, { 0x9f, 0x00, 0x00 },
	{ 0x9b, 0x00, 0x00 }, { 0x97, 0x00, 0x00 }, { 0x93, 0x00, 0x00 }, { 0x8f, 0x00, 0x00 }, { 0x8b, 0x00, 0x00 }, { 0x87, 0x00, 0x00 }, { 0x83, 0x00, 0x00 }, { 0x7f, 0x00, 0x00 },
};

static const unsigned char colormap_rainbow[256][3] = {
	{ 0x26, 0x00, 0x32 }, { 0x26, 0x00, 0x33 }, { 0x26, 0x00, 0x33 }, { 0x26, 0x00, 0x33 }, { 0x27, 0x00, 0x34 }, { 0x27, 0x00, 0x34 }, { 0x27, 0x00, 0x35 }, { 0x28, 0x00, 0x36 },
	{ 0x29, 0x00, 0x37 }, { 0x29, 0x00, 0x39 }, { 0x2a, 0x00, 0x3a }, { 0x2b, 0x00, 0x3c }, { 0x2c, 0x00, 0x3d }, { 0x2d, 0x00, 0x3f }, { 0x2e, 0x00, 0x41 }, { 0x2f, 0x00, 0x43 },
	{ 0x30, 0x00, 0x46 }, { 0x32, 0x00, 0x48 }, { 0x33, 0x00, 0x4b }, { 0x34, 0x00, 0x4d }, { 0x35, 0x00, 0x50 }, { 0x36, 0x00, 0x53 }, { 0x38, 0x00, 0x56 }, { 0x39, 0x00, 0x59 },
	{ 0x3a, 0x00, 0x5c }, { 0x3b, 0x00, 0x60 }, { 0x3c, 0x00, 0x63 }, { 0x3d, 0x00, 0x67 }, { 0x3e, 0x00, 0x6a }, { 0x3f, 0x00, 0x6e }, { 0x40, 0x00, 0x72 }, { 0x41, 0x00, 0x75 },
	{ 0x41, 0x00, 0x79 }, { 0x42, 0x00, 0x7d }, { 0x42, 0x00, 0x81 }, { 0x42, 0x00, 0x85 }, { 0x42, 0x00, 0x89 }, { 0x42, 0x00, 0x8d }, { 0x42, 0x00, 0x91 }, { 0x41, 0x00, 0x96 },
	{ 0x41, 0x00, 0x9a }, { 0x40, 0x00, 0x9e }, { 0x3f, 0x00, 0xa2 }, { 0x3e, 0x00, 0xa6 }, { 0x3c, 0x00, 0xaa }, { 0x3b, 0x00, 0xae }, { 0x39, 0x00, 0xb3 }, { 0x37, 0x00, 0xb7 },
	{ 0x34, 0x00, 0xbb }, { 0x32, 0x00, 0xbf }, { 0x2f, 0x00, 0xc3 }, { 0x2c, 0x00, 0xc6 }, { 0x29, 0x00, 0xca }, { 0x26, 0x00, 0xce }, { 0x22, 0x00, 0xd2 }, { 0x1e, 0x00, 0xd5 },
	{ 0x1a, 0x00, 0xd9 }, { 0x15, 0x00, 0xdc }, { 0x11, 0x00, 0xdf }, { 0x0c, 0x00, 0xe2 }, { 0x07, 0x00, 0xe5 }, { 0x02, 0x00, 0xe8 }, { 0x00, 0x03, 0xeb }, { 0x00, 0x08, 0xed },
	{ 0x00, 0x0e, 0xf0 }, { 0x00, 0x14, 0xf2 }, { 0x00, 0x1a, 0xf4 }, { 0x00, 0x21, 0xf6 }, { 0x00, 0x27, 0xf7 }, { 0x00, 0x2e, 0xf9 }, { 0x00, 0x34, 0xfa }, { 0x00, 0x3b, 0xfb },
	{ 0x00, 0x42, 0xfc }, { 0x00, 0x49, 0xfd }, { 0x00, 0x50, 0xfd }, { 0x00, 0x56, 0xfd }, { 0x00, 0x5d, 0xfd }, { 0x00, 0x64, 0xfd }, { 0x00, 0x6b, 0xfd }, { 0x00, 0x72, 0xfd },
	{ 0x00, 0x79, 0xfd }, { 0x00, 0x80, 0xfd }, { 0x00, 0x88, 0xfc }, { 0x00, 0x8f, 0xfc }, { 0x00, 0x96, 0xfc }, { 0x00, 0x9d, 0xfc }, { 0x00, 0xa5, 0xfc }, { 0x00, 0xac, 0xfc },
	{ 0x00, 0xb4, 0xfc }, { 0x00, 0xbb, 0xfc }, { 0x00, 0xc2, 0xfb }, { 0x00, 0xca, 0xfb }, { 0x00, 0xd2, 0xfb }, { 0x00, 0xd9, 0xfb }, { 0x00, 0xe1, 0xfa }, { 0x00, 0xe8, 0xfa },
	{ 0x00, 0xf0, 0xfa }, { 0x00, 0xf7, 0xfa }, { 0x00, 0xf9, 0xf4 }, { 0x00, 0xf9, 0xeb }, { 0x00, 0xf8, 0xe3 }, { 0x00, 0xf8, 0xdb }, { 0x00, 0xf7, 0xd2 }, { 0x00, 0xf7, 0xca },
	{ 0x00, 0xf6, 0xc1 }, { 0x00, 0xf5, 0xb9 }, { 0x00, 0xf5, 0xb0 }, { 0x00, 0xf4, 0xa7 }, { 0x00, 0xf3, 0x9f }, { 0x00, 0xf1, 0x96 }, { 0x00, 0xf0, 0x8d }, { 0x00, 0xee, 0x84 },
	{ 0x00, 0xed, 0x7b }, { 0x00, 0xeb, 0x72 }, { 0x00, 0xe8, 0x69 }, { 0x00, 0xe5, 0x60 }, { 0x00, 0xe2, 0x57 }, { 0x00, 0xde, 0x4e }, { 0x00, 0xd9, 0x45 }, { 0x00, 0xd4, 0x3d },
	{ 0x00, 0xce, 0x34 }, { 0x00, 0xc6, 0x2b }, { 0x00, 0xbe, 0x23 }, { 0x00, 0xb5, 0x1b }, { 0x00, 0xac, 0x14 }, { 0x00, 0xa3, 0x0d }, { 0x00, 0x9d, 0x07 }, { 0x00, 0x99, 0x02 },
	{ 0x02, 0x99, 0x00 }, { 0x07, 0x9d, 0x00 }, { 0x0d, 0xa3, 0x00 }, { 0x14, 0xac, 0x00 }, { 0x1b, 0xb5, 0x00 }, { 0x23, 0xbe, 0x00 }, { 0x2b, 0xc6, 0x00 }, { 0x34, 0xce, 0x00 },
	{ 0x3d, 0xd4, 0x00 }, { 0x45, 0xd9, 0x00 }, { 0x4e, 0xde, 0x00 }, { 0x57, 0xe2, 0x00 }, { 0x60, 0xe5, 0x00 }, { 0x69, 0xe8, 0x00 }, { 0x72, 0xeb, 0x00 }, { 0x7b, 0xed, 0x00 },
	{ 0x84, 0xee, 0x00 }, { 0x8d, 0xf0, 0x00 }, { 0x96, 0xf1, 0x00 }, { 0x9f, 0xf3, 0x00 }, { 0xa7, 0xf4, 0x00 }, { 0xb0, 0xf5, 0x00 }, { 0xb9, 0xf5, 0x00 }, { 0xc1, 0xf6, 0x00 },
	{ 0xca, 0xf7, 0x00 }, { 0xd2, 0xf7, 0x00 }, { 0xdb, 0xf8, 0x00 }, { 0xe3, 0xf8, 0x00 }, { 0xeb, 0xf9, 0x00 }, { 0xf4, 0xf9, 0x00 }, { 0xfa, 0xf7, 0x00 }, { 0xfa, 0xf0, 0x00 },
	{ 0xfa, 0xe8, 0x00 }, { 0xfa, 0xe1, 0x00 }, { 0xfb, 0xd9, 0x00 }, { 0xfb, 0xd2, 0x00 }, { 0xfb, 0xca, 0x00 }, { 0xfb, 0xc2, 0x00 }, { 0xfc, 0xbb, 0x00 }, { 0xfc, 0xb4, 0x00 },
	{ 0xfc, 0xac, 0x00 }, { 0xfc, 0xa5, 0x00 }, { 0xfc, 0x9d, 0x00 }, { 0xfc, 0x96, 0x00 }, { 0xfc, 0x8f, 0x00 }, { 0xfc, 0x88, 0x00 }, { 0xfd, 0x80, 0x00 }, { 0xfd, 0x79, 0x00 },
	{ 0xfd, 0x72, 0x00 }, { 0xfd, 0x6b, 0x00 }, { 0xfd, 0x64, 0x00 }, { 0xfd, 0x5d, 0x00 }, { 0xfd, 0x56, 0x00 }, { 0xfd, 0x50, 0x00 }, { 0xfd, 0x49, 0x00 }, { 0xfc, 0x42, 0x00 },
	{ 0xfb, 0x3b, 0x00 }, { 0xfa, 0x34, 0x00 }, { 0xf9, 0x2e, 0x00 }, { 0xf7, 0x27, 0x00 }, { 0xf6, 0x21, 0x00 }, { 0xf4, 0x1a, 0x00 }, { 0xf2, 0x14, 0x00 }, { 0xf0, 0x0e, 0x00 },
	{ 0xed, 0x08, 0x00 }, { 0xeb, 0x03, 0x00 }, { 0xe8, 0x00, 0x02 }, { 0xe5, 0x00, 0x07 }, { 0xe2, 0x00, 0x0c }, { 0xdf, 0x00, 0x11 }, { 0xdc, 0x00, 0x15 }, { 0xd9, 0x00, 0x1a },
	{ 0xd5, 0x00, 0x1e }, { 0xd2, 0x00, 0x22 }, { 0xce, 0x00, 0x26 }, { 0xca, 0x00, 0x29 }, { 0xc6, 0x00, 0x2c }, { 0xc3, 0x00, 0x2f }, { 0xbf, 0x00, 0x32 }, { 0xbb, 0x00, 0x34 },
	{ 0xb7, 0x00, 0x37 }, { 0xb3, 0x00, 0x39 }, { 0xae, 0x00, 0x3b }, { 0xaa, 0x00, 0x3c }, { 0xa6, 0x00, 0x3e }, { 0xa2, 0x00, 0x3f }, { 0x9e, 0x00, 0x40 }, { 0x9a, 0x00, 0x41 },
	{ 0x96, 0x00, 0x41 }, { 0x91, 0x00, 0x42 }, { 0x8d, 0x00, 0x42 }, { 0x89, 0x00, 0x42 }, { 0x85, 0x00, 0x42 }, { 0x81, 0x00, 0x42 }, { 0x7d, 0x00, 0x42 }, { 0x79, 0x00, 0x41 },
	{ 0x75, 0x00, 0x41 }, { 0x72, 0x00, 0x40 }, { 0x6e, 0x00, 0x3f }, { 0x6a, 0x00, 0x3e }, { 0x67, 0x00, 0x3d }, { 0x63, 0x00, 0x3c }, { 0x60, 0x00, 0x3b }, { 0x5c, 0x00, 0x3a },
	{ 0x59, 0x00, 0x39 }, { 0x56, 0x00, 0x38 }, { 0x53, 0x00, 0x36 }, { 0x50, 0x00, 0x35 }, { 0x4d, 0x00, 0x34 }, { 0x4b, 0x00, 0x33 }, { 0x48, 0x00, 0x32 }, { 0x46, 0x00, 0x30 },
	{ 0x43, 0x00, 0x2f }, { 0x41, 0x00, 0x2e }, { 0x3f, 0x00, 0x2d }, { 0x3d, 0x00, 0x2c }, { 0x3c, 0x00, 0x2b }, { 0x3a, 0x00, 0x2a }, { 0x39, 0x00, 0x29 }, { 0x37, 0x00, 0x29 },
	{ 0x36, 0x00, 0x28 }, { 0x35, 0x00, 0x27 }, { 0x34, 0x00, 0x27 }, { 0x34, 0x00, 0x27 }, { 0x33, 0x00, 0x26 }, { 0x33, 0x00, 0x26 }, { 0x33, 0x00, 0x26 }, { 0x32, 0x00, 0x26 },
};

static void surface_cairo_filter_colorize(struct surface_t * s, const char * type)
{
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	int stride = surface_get_stride(s);
	unsigned char * p, *q = surface_get_pixels(s);
	const unsigned char (*cm)[3];
	int x, y;

	switch(shash(type))
	{
	case 0x143c974a: /* "parula" */
		cm = colormap_parula;
		break;
	case 0x0b888348: /* "jet" */
		cm = colormap_jet;
		break;
	case 0x351d1e97: /* "rainbow" */
		cm = colormap_rainbow;
		break;
	default:
		cm = colormap_parula;
		break;
	}
	for(y = 0; y < height; y++, q += stride)
	{
		for(x = 0, p = q; x < width; x++, p += 4)
		{
			p[0] = cm[p[0]][0];
			p[1] = cm[p[1]][1];
			p[2] = cm[p[2]][2];
		}
	}
}

static void surface_cairo_filter_gamma(struct surface_t * s, double gamma)
{
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	int stride = surface_get_stride(s);
	unsigned char * p, *q = surface_get_pixels(s);
	unsigned char lut[256];
	int x, y, i, t;

	for(i = 0; i < 256; i++)
	{
		t = powf((float)(i / 255.0), (float)gamma) * 255.0;
		lut[i] = min(t, 255);
	}
	for(y = 0; y < height; y++, q += stride)
	{
		for(x = 0, p = q; x < width; x++, p += 4)
		{
			p[0] = lut[p[0]];
			p[1] = lut[p[1]];
			p[2] = lut[p[2]];
		}
	}
}

static void surface_cairo_filter_hue(struct surface_t * s, int angle)
{
	float av = angle * M_PI / 180.0;
	float cv = cosf(av);
	float sv = sinf(av);
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	int stride = surface_get_stride(s);
	unsigned char * p, *q = surface_get_pixels(s);
	int r, g, b;
	int tr, tg, tb;
	int x, y;
	int m[9];

	m[0] = (0.213 + cv * 0.787 - sv * 0.213) * 65536;
	m[1] = (0.715 - cv * 0.715 - sv * 0.715) * 65536;
	m[2] = (0.072 - cv * 0.072 + sv * 0.928) * 65536;
	m[3] = (0.213 - cv * 0.213 + sv * 0.143) * 65536;
	m[4] = (0.715 + cv * 0.285 + sv * 0.140) * 65536;
	m[5] = (0.072 - cv * 0.072 - sv * 0.283) * 65536;
	m[6] = (0.213 - cv * 0.213 - sv * 0.787) * 65536;
	m[7] = (0.715 - cv * 0.715 + sv * 0.715) * 65536;
	m[8] = (0.072 + cv * 0.928 + sv * 0.072) * 65536;
	for(y = 0; y < height; y++, q += stride)
	{
		for(x = 0, p = q; x < width; x++, p += 4)
		{
			b = p[0];
			g = p[1];
			r = p[2];
			tb = (m[6] * r + m[7] * g + m[8] * b) >> 16;
			tg = (m[3] * r + m[4] * g + m[5] * b) >> 16;
			tr = (m[0] * r + m[1] * g + m[2] * b) >> 16;
			p[0] = clamp(tb, 0, 255);
			p[1] = clamp(tg, 0, 255);
			p[2] = clamp(tr, 0, 255);
		}
	}
}

static void surface_cairo_filter_saturate(struct surface_t * s, int saturate)
{
	int k = clamp(saturate, -100, 100) / 100.0 * 128.0;
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	int stride = surface_get_stride(s);
	unsigned char * p, *q = surface_get_pixels(s);
	int r, g, b, min, max;
	int alpha, delta, value, lv, sv;
	int x, y;

	for(y = 0; y < height; y++, q += stride)
	{
		for(x = 0, p = q; x < width; x++, p += 4)
		{
			b = p[0];
			g = p[1];
			r = p[2];
			min = min(min(r, g), b);
			max = max(max(r, g), b);
			delta = max - min;
			value = max + min;
			if(delta == 0)
				continue;
			lv = value >> 1;
			sv = lv < 128 ? (delta << 7) / value : (delta << 7) / (510 - value);
			if(k >= 0)
			{
				alpha = (k + sv >= 128) ? sv : 128 - k;
				if(alpha != 0)
					alpha = 128 * 128 / alpha - 128;
			}
			else
			{
				alpha = k;
			}
			r = r + ((r - lv) * alpha >> 7);
			g = g + ((g - lv) * alpha >> 7);
			b = b + ((b - lv) * alpha >> 7);
			p[0] = clamp(b, 0, 255);
			p[1] = clamp(g, 0, 255);
			p[2] = clamp(r, 0, 255);
		}
	}
}

static void surface_cairo_filter_brightness(struct surface_t * s, int brightness)
{
	int delta = clamp(brightness, -100, 100) / 100.0 * 255.0;
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	int stride = surface_get_stride(s);
	unsigned char * p, *q = surface_get_pixels(s);
	int r, g, b;
	int tr, tg, tb;
	int x, y;

	for(y = 0; y < height; y++, q += stride)
	{
		for(x = 0, p = q; x < width; x++, p += 4)
		{
			b = p[0];
			g = p[1];
			r = p[2];
			tb = b + delta;
			tg = g + delta;
			tr = r + delta;
			p[0] = clamp(tb, 0, 255);
			p[1] = clamp(tg, 0, 255);
			p[2] = clamp(tr, 0, 255);
		}
	}
}

static void surface_cairo_filter_contrast(struct surface_t * s, int contrast)
{
	int k = clamp(contrast, -100, 100) / 100.0 * 128.0;
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	int stride = surface_get_stride(s);
	unsigned char * p, *q = surface_get_pixels(s);
	int r, g, b;
	int tr, tg, tb;
	int x, y;

	for(y = 0; y < height; y++, q += stride)
	{
		for(x = 0, p = q; x < width; x++, p += 4)
		{
			b = p[0];
			g = p[1];
			r = p[2];
			tb = (b << 7) + (b - 128) * k;
			tg = (g << 7) + (g - 128) * k;
			tr = (r << 7) + (r - 128) * k;
			p[0] = clamp(tb, 0, 255 << 7) >> 7;
			p[1] = clamp(tg, 0, 255 << 7) >> 7;
			p[2] = clamp(tr, 0, 255 << 7) >> 7;
		}
	}
}

static inline void blurinner(unsigned char * p, int * zr, int * zg, int * zb, int * za, int alpha)
{
	int r, g, b;
	unsigned char a;

	b = p[0];
	g = p[1];
	r = p[2];
	a = p[3];

	*zb += (alpha * ((b << 7) - *zb)) >> 16;
	*zg += (alpha * ((g << 7) - *zg)) >> 16;
	*zr += (alpha * ((r << 7) - *zr)) >> 16;
	*za += (alpha * ((a << 7) - *za)) >> 16;

	p[0] = *zb >> 7;
	p[1] = *zg >> 7;
	p[2] = *zr >> 7;
	p[3] = *za >> 7;
}

static inline void blurrow(unsigned char * pixel, int width, int height, int channel, int line, int alpha)
{
	unsigned char * p = &(pixel[line * width * channel]);
	int zr, zg, zb, za;
	int i;

	zb = p[0] << 7;
	zg = p[1] << 7;
	zr = p[2] << 7;
	za = p[3] << 7;

	for(i = 0; i < width; i++)
		blurinner(&p[i * channel], &zr, &zg, &zb, &za, alpha);
	for(i = width - 2; i >= 0; i--)
		blurinner(&p[i * channel], &zr, &zg, &zb, &za, alpha);
}

static inline void blurcol(unsigned char * pixel, int width, int height, int channel, int x, int alpha)
{
	unsigned char * p = pixel;
	int zr, zg, zb, za;
	int i;

	p += x * channel;
	zb = p[0] << 7;
	zg = p[1] << 7;
	zr = p[2] << 7;
	za = p[3] << 7;

	for(i = width; i < (height - 1) * width; i += width)
		blurinner(&p[i * channel], &zr, &zg, &zb, &za, alpha);
	for(i = (height - 2) * width; i >= 0; i -= width)
		blurinner(&p[i * channel], &zr, &zg, &zb, &za, alpha);
}

static inline void expblur(unsigned char * pixel, int width, int height, int channel, int radius)
{
	int alpha = (int)((1 << 16) * (1.0 - expf(-2.3 / (radius + 1.0))));
	int row, col;

	for(row = 0; row < height; row++)
		blurrow(pixel, width, height, channel, row, alpha);
	for(col = 0; col < width; col++)
		blurcol(pixel, width, height, channel, col, alpha);
}

static void surface_cairo_filter_blur(struct surface_t * s, int radius)
{
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	unsigned char * pixels = surface_get_pixels(s);

	if(radius > 0)
		expblur(pixels, width, height, 4, radius);
}

static void surface_cairo_shape_save(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_save(cr);
}

static void surface_cairo_shape_restore(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_restore(cr);
}

static void surface_cairo_shape_push_group(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_push_group(cr);
}

static void surface_cairo_shape_pop_group(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_pop_group(cr);
}

static void surface_cairo_shape_pop_group_to_source(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_pop_group_to_source(cr);
}

static void surface_cairo_shape_new_path(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_new_path(cr);
}

static void surface_cairo_shape_new_sub_path(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_new_sub_path(cr);
}

static void surface_cairo_shape_close_path(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_close_path(cr);
}

static void surface_cairo_shape_set_operator(struct surface_t * s, const char * type)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	switch(shash(type))
	{
	case 0x0f3b6d8c: /* "clear" */
		cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
		break;
	case 0x1c3aff76: /* "source" */
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		break;
	case 0x7c9bf101: /* "over" */
		cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
		break;
	case 0x0059783c: /* "in" */
		cairo_set_operator(cr, CAIRO_OPERATOR_IN);
		break;
	case 0x0b889a9d: /* "out" */
		cairo_set_operator(cr, CAIRO_OPERATOR_OUT);
		break;
	case 0x7c943c79: /* "atop" */
		cairo_set_operator(cr, CAIRO_OPERATOR_ATOP);
		break;
	case 0x7c95a255: /* "dest" */
		cairo_set_operator(cr, CAIRO_OPERATOR_DEST);
		break;
	case 0x4f1e8f9e: /* "dest-over" */
		cairo_set_operator(cr, CAIRO_OPERATOR_DEST_OVER);
		break;
	case 0x0979cb99: /* "dest-in" */
		cairo_set_operator(cr, CAIRO_OPERATOR_DEST_IN);
		break;
	case 0x38b3599a: /* "dest-out" */
		cairo_set_operator(cr, CAIRO_OPERATOR_DEST_OUT);
		break;
	case 0x4f16db16: /* "dest-atop" */
		cairo_set_operator(cr, CAIRO_OPERATOR_DEST_ATOP);
		break;
	case 0x0b88c01e: /* "xor" */
		cairo_set_operator(cr, CAIRO_OPERATOR_XOR);
		break;
	case 0x0b885cce: /* "add" */
		cairo_set_operator(cr, CAIRO_OPERATOR_ADD);
		break;
	case 0xdf32bb4e: /* "saturate" */
		cairo_set_operator(cr, CAIRO_OPERATOR_SATURATE);
		break;
	case 0x46746f05: /* "multiply" */
		cairo_set_operator(cr, CAIRO_OPERATOR_MULTIPLY);
		break;
	case 0x1b5ffd45: /* "screen" */
		cairo_set_operator(cr, CAIRO_OPERATOR_SCREEN);
		break;
	case 0x7ee4b5c7: /* "overlay" */
		cairo_set_operator(cr, CAIRO_OPERATOR_OVERLAY);
		break;
	case 0xf83e845a: /* "darken" */
		cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);
		break;
	case 0x79c1c710: /* "lighten" */
		cairo_set_operator(cr, CAIRO_OPERATOR_LIGHTEN);
		break;
	case 0x8b9b2114: /* "color-dodge" */
		cairo_set_operator(cr, CAIRO_OPERATOR_COLOR_DODGE);
		break;
	case 0x61514968: /* "color-burn" */
		cairo_set_operator(cr, CAIRO_OPERATOR_COLOR_BURN);
		break;
	case 0xb5060029: /* "hard-light" */
		cairo_set_operator(cr, CAIRO_OPERATOR_HARD_LIGHT);
		break;
	case 0xfcd63c26: /* "soft-light" */
		cairo_set_operator(cr, CAIRO_OPERATOR_SOFT_LIGHT);
		break;
	case 0x52a92470: /* "difference" */
		cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
		break;
	case 0x6f499bff: /* "exclusion" */
		cairo_set_operator(cr, CAIRO_OPERATOR_EXCLUSION);
		break;
	case 0x5d66597b: /* "hsl-hue" */
		cairo_set_operator(cr, CAIRO_OPERATOR_HSL_HUE);
		break;
	case 0x1f46f0c3: /* "hsl-saturation" */
		cairo_set_operator(cr, CAIRO_OPERATOR_HSL_SATURATION);
		break;
	case 0x50050bf8: /* "hsl-color" */
		cairo_set_operator(cr, CAIRO_OPERATOR_HSL_COLOR);
		break;
	case 0x02d64a96: /* "hsl-luminosity" */
		cairo_set_operator(cr, CAIRO_OPERATOR_HSL_LUMINOSITY);
		break;
	default:
		break;
	}
}

static void surface_cairo_shape_set_source(struct surface_t * s, void * pattern)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_set_source(cr, pattern);
}

static void * surface_cairo_shape_get_source(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	return cairo_get_source(cr);
}

static void surface_cairo_shape_set_source_color(struct surface_t * s, double r, double g, double b, double a)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_set_source_rgba(cr, r, g, b, a);
}

static void surface_cairo_shape_set_source_surface(struct surface_t * s, struct surface_t * o, double x, double y)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_set_source_surface(cr, ((struct surface_cairo_context_t *)o->pctx)->cs, x, y);
}

static void surface_cairo_shape_set_tolerance(struct surface_t * s, double tolerance)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_set_tolerance(cr, tolerance);
}

static void surface_cairo_shape_set_miter_limit(struct surface_t * s, double limit)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_set_miter_limit(cr, limit);
}

static void surface_cairo_shape_set_antialias(struct surface_t * s, const char * type)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	switch(shash(type))
	{
	case 0x0885548a: /* "default" */
		cairo_set_antialias(cr, CAIRO_ANTIALIAS_DEFAULT);
		break;
	case 0x7c9b47f5: /* "none" */
		cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
		break;
	case 0x7c977c78: /* "gray" */
		cairo_set_antialias(cr, CAIRO_ANTIALIAS_GRAY);
		break;
	case 0xb8639511: /* "subpixel" */
		cairo_set_antialias(cr, CAIRO_ANTIALIAS_SUBPIXEL);
		break;
	case 0x7c96aa13: /* "fast" */
		cairo_set_antialias(cr, CAIRO_ANTIALIAS_FAST);
		break;
	case 0x7c97716e: /* "good" */
		cairo_set_antialias(cr, CAIRO_ANTIALIAS_GOOD);
		break;
	case 0x7c948993: /* "best" */
		cairo_set_antialias(cr, CAIRO_ANTIALIAS_BEST);
		break;
	default:
		break;
	}
}

static void surface_cairo_shape_set_fill_rule(struct surface_t * s, const char * type)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	switch(shash(type))
	{
	case 0xc8feafb5: /* "winding" */
		cairo_set_fill_rule(cr, CAIRO_FILL_RULE_WINDING);
		break;
	case 0x207e6937: /* "even-odd" */
		cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);
		break;
	default:
		break;
	}
}

static void surface_cairo_shape_set_line_width(struct surface_t * s, double width)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_set_line_width(cr, width);
}

static void surface_cairo_shape_set_line_cap(struct surface_t * s, const char * type)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	switch(shash(type))
	{
	case 0x7c94cdc4: /* "butt" */
		cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
		break;
	case 0x104cc7ed: /* "round" */
		cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
		break;
	case 0x1c5eea16: /* "square" */
		cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
		break;
	default:
		break;
	}
}

static void surface_cairo_shape_set_line_join(struct surface_t * s, const char * type)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	switch(shash(type))
	{
	case 0x0feefdc6: /* "miter" */
		cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
		break;
	case 0x104cc7ed: /* "round" */
		cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
		break;
	case 0x0f25c733: /* "bevel" */
		cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);
		break;
	default:
		break;
	}
}

static void surface_cairo_shape_set_dash(struct surface_t * s, const double * dashes, int ndashes, double offset)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_set_dash(cr, dashes, ndashes, offset);
}

static void surface_cairo_shape_identity(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_identity_matrix(cr);
}

static void surface_cairo_shape_translate(struct surface_t * s, double tx, double ty)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_translate(cr, tx, ty);
}

static void surface_cairo_shape_scale(struct surface_t * s, double sx, double sy)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_scale(cr, sx, sy);
}

static void surface_cairo_shape_rotate(struct surface_t * s, double angle)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_rotate(cr, angle);
}

static void surface_cairo_shape_transform(struct surface_t * s, struct matrix_t * m)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_transform(cr, (cairo_matrix_t *)m);
}

static void surface_cairo_shape_set_matrix(struct surface_t * s, struct matrix_t * m)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_set_matrix(cr, (cairo_matrix_t *)m);
}

static void surface_cairo_shape_get_matrix(struct surface_t * s, struct matrix_t * m)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_get_matrix(cr, (cairo_matrix_t *)m);
}

static void surface_cairo_shape_move_to(struct surface_t * s, double x, double y)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_move_to(cr, x, y);
}

static void surface_cairo_shape_rel_move_to(struct surface_t * s, double dx, double dy)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_rel_move_to(cr, dx, dy);
}

static void surface_cairo_shape_line_to(struct surface_t * s, double x, double y)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_line_to(cr, x, y);
}

static void surface_cairo_shape_rel_line_to(struct surface_t * s, double dx, double dy)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_rel_line_to(cr, dx, dy);
}

static void surface_cairo_shape_curve_to(struct surface_t * s, double x1, double y1, double x2, double y2, double x3, double y3)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_curve_to(cr, x1, y1, x2, y2, x3, y3);
}

static void surface_cairo_shape_rel_curve_to(struct surface_t * s, double dx1, double dy1, double dx2, double dy2, double dx3, double dy3)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_rel_curve_to(cr, dx1, dy1, dx2, dy2, dx3, dy3);
}

static void surface_cairo_shape_rectangle(struct surface_t * s, double x, double y, double w, double h)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_rectangle(cr, x, y, w, h);
}

static void surface_cairo_shape_rounded_rectangle(struct surface_t * s, double x, double y, double w, double h, double r)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_move_to(cr, x + r, y);
	cairo_line_to(cr, x + w - r, y);
	cairo_arc(cr, x + w - r, y + r, r, - M_PI / 2, 0);
	cairo_line_to(cr, x + w, y + h - r);
	cairo_arc(cr, x + w - r, y + h - r, r, 0, M_PI / 2);
	cairo_line_to(cr, x + r, y + h);
	cairo_arc(cr, x + r, y + h - r, r, M_PI / 2, M_PI);
	cairo_line_to(cr, x, y + r);
	cairo_arc(cr, x + r, y + r, r, M_PI, M_PI + M_PI / 2);
}

static void surface_cairo_shape_arc(struct surface_t * s, double xc, double yc, double r, double a1, double a2)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_arc(cr, xc, yc, r, a1, a2);
}

static void surface_cairo_shape_arc_negative(struct surface_t * s, double xc, double yc, double r, double a1, double a2)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_arc_negative(cr, xc, yc, r, a1, a2);
}

static void surface_cairo_shape_stroke(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_stroke(cr);
}

static void surface_cairo_shape_stroke_preserve(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_stroke_preserve(cr);
}

static void surface_cairo_shape_fill(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_fill(cr);
}

static void surface_cairo_shape_fill_preserve(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_fill_preserve(cr);
}

static void surface_cairo_shape_reset_clip(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_reset_clip(cr);
}

static void surface_cairo_shape_clip(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_clip(cr);
}

static void surface_cairo_shape_clip_preserve(struct surface_t * s)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_clip_preserve(cr);
}

static void surface_cairo_shape_mask(struct surface_t * s, void * pattern)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_mask(cr, pattern);
}

static void surface_cairo_shape_mask_surface(struct surface_t * s, struct surface_t * o, double x, double y)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_mask_surface(cr, ((struct surface_cairo_context_t *)s->pctx)->cs, x, y);
}

static void surface_cairo_shape_paint(struct surface_t * s, double alpha)
{
	cairo_t * cr = ((struct surface_cairo_context_t *)s->pctx)->cr;
	cairo_paint_with_alpha(cr, alpha);
}

static void * surface_cairo_pattern_create_color(double r, double g, double b, double a)
{
	return cairo_pattern_create_rgba(r, g, b, a);
}

static void * surface_cairo_pattern_create_linear(double x0, double y0, double x1, double y1)
{
	return cairo_pattern_create_linear(x0, y0, x1, y1);
}

static void * surface_cairo_pattern_create_radial(double x0, double y0, double r0, double x1, double y1, double r1)
{
	return cairo_pattern_create_radial(x0, y0, r0, x1, y1, r1);
}

static void * surface_cairo_font_create(void * font)
{
	return cairo_ft_font_face_create_for_ft_face((FT_Face)font, 0);
}

static void surface_cairo_font_destroy(void * sfont)
{
	cairo_font_face_destroy((cairo_font_face_t *)sfont);
}

static void * surface_cairo_pattern_create(struct surface_t * s)
{
	return cairo_pattern_create_for_surface(((struct surface_cairo_context_t *)s->pctx)->cs);
}

static void surface_cairo_pattern_destroy(void * pattern)
{
	cairo_pattern_destroy((cairo_pattern_t *)pattern);
}

static void surface_cairo_pattern_add_color_stop(void * pattern, double o, double r, double g, double b, double a)
{
	cairo_pattern_add_color_stop_rgba((cairo_pattern_t *)pattern, o, r, g, b, a);
}

static void surface_cairo_pattern_set_extend(void * pattern, const char * type)
{
	switch(shash(type))
	{
	case 0x7c9b47f5: /* "none" */
		cairo_pattern_set_extend((cairo_pattern_t *)pattern, CAIRO_EXTEND_NONE);
		break;
	case 0x192dec66: /* "repeat" */
		cairo_pattern_set_extend((cairo_pattern_t *)pattern, CAIRO_EXTEND_REPEAT);
		break;
	case 0x3e3a6a0a: /* "reflect" */
		cairo_pattern_set_extend((cairo_pattern_t *)pattern, CAIRO_EXTEND_REFLECT);
		break;
	case 0x0b889c3a: /* "pad" */
		cairo_pattern_set_extend((cairo_pattern_t *)pattern, CAIRO_EXTEND_PAD);
		break;
	default:
		break;
	}
}

static void surface_cairo_pattern_set_filter(void * pattern, const char * type)
{
	switch(shash(type))
	{
	case 0x7c96aa13: /* "fast" */
		cairo_pattern_set_filter((cairo_pattern_t *)pattern, CAIRO_FILTER_FAST);
		break;
	case 0x7c97716e: /* "good" */
		cairo_pattern_set_filter((cairo_pattern_t *)pattern, CAIRO_FILTER_GOOD);
		break;
	case 0x7c948993: /* "best" */
		cairo_pattern_set_filter((cairo_pattern_t *)pattern, CAIRO_FILTER_BEST);
		break;
	case 0x09fa48d7: /* "nearest" */
		cairo_pattern_set_filter((cairo_pattern_t *)pattern, CAIRO_FILTER_NEAREST);
		break;
	case 0x8320f06b: /* "bilinear" */
		cairo_pattern_set_filter((cairo_pattern_t *)pattern, CAIRO_FILTER_BILINEAR);
		break;
	case 0xce4e6460: /* "gaussian" */
		cairo_pattern_set_filter((cairo_pattern_t *)pattern, CAIRO_FILTER_GAUSSIAN);
		break;
	default:
		break;
	}
}

static void surface_cairo_pattern_set_matrix(void * pattern, struct matrix_t * m)
{
	cairo_pattern_set_matrix((cairo_pattern_t *)pattern, (cairo_matrix_t *)m);
}

struct surface_operate_t surface_operate_cairo = {
	.create						= surface_cairo_create,
	.destroy					= surface_cairo_destroy,

	.blit						= surface_cairo_blit,
	.mask						= surface_cairo_mask,
	.fill						= surface_cairo_fill,
	.text						= surface_cairo_text,
	.extent						= surface_cairo_extent,

	.filter_grayscale			= surface_cairo_filter_grayscale,
	.filter_sepia				= surface_cairo_filter_sepia,
	.filter_invert				= surface_cairo_filter_invert,
	.filter_threshold			= surface_cairo_filter_threshold,
	.filter_colorize			= surface_cairo_filter_colorize,
	.filter_gamma				= surface_cairo_filter_gamma,
	.filter_hue					= surface_cairo_filter_hue,
	.filter_saturate			= surface_cairo_filter_saturate,
	.filter_brightness			= surface_cairo_filter_brightness,
	.filter_contrast			= surface_cairo_filter_contrast,
	.filter_blur				= surface_cairo_filter_blur,

	.shape_save					= surface_cairo_shape_save,
	.shape_restore				= surface_cairo_shape_restore,
	.shape_push_group			= surface_cairo_shape_push_group,
	.shape_pop_group			= surface_cairo_shape_pop_group,
	.shape_pop_group_to_source	= surface_cairo_shape_pop_group_to_source,
	.shape_new_path				= surface_cairo_shape_new_path,
	.shape_new_sub_path			= surface_cairo_shape_new_sub_path,
	.shape_close_path			= surface_cairo_shape_close_path,
	.shape_set_operator			= surface_cairo_shape_set_operator,
	.shape_set_source			= surface_cairo_shape_set_source,
	.shape_get_source			= surface_cairo_shape_get_source,
	.shape_set_source_color		= surface_cairo_shape_set_source_color,
	.shape_set_source_surface	= surface_cairo_shape_set_source_surface,
	.shape_set_tolerance		= surface_cairo_shape_set_tolerance,
	.shape_set_miter_limit		= surface_cairo_shape_set_miter_limit,
	.shape_set_antialias		= surface_cairo_shape_set_antialias,
	.shape_set_fill_rule		= surface_cairo_shape_set_fill_rule,
	.shape_set_line_width		= surface_cairo_shape_set_line_width,
	.shape_set_line_cap			= surface_cairo_shape_set_line_cap,
	.shape_set_line_join		= surface_cairo_shape_set_line_join,
	.shape_set_dash				= surface_cairo_shape_set_dash,
	.shape_identity				= surface_cairo_shape_identity,
	.shape_translate			= surface_cairo_shape_translate,
	.shape_scale				= surface_cairo_shape_scale,
	.shape_rotate				= surface_cairo_shape_rotate,
	.shape_transform			= surface_cairo_shape_transform,
	.shape_set_matrix			= surface_cairo_shape_set_matrix,
	.shape_get_matrix			= surface_cairo_shape_get_matrix,
	.shape_move_to				= surface_cairo_shape_move_to,
	.shape_rel_move_to			= surface_cairo_shape_rel_move_to,
	.shape_line_to				= surface_cairo_shape_line_to,
	.shape_rel_line_to			= surface_cairo_shape_rel_line_to,
	.shape_curve_to				= surface_cairo_shape_curve_to,
	.shape_rel_curve_to			= surface_cairo_shape_rel_curve_to,
	.shape_rectangle			= surface_cairo_shape_rectangle,
	.shape_rounded_rectangle	= surface_cairo_shape_rounded_rectangle,
	.shape_arc					= surface_cairo_shape_arc,
	.shape_arc_negative			= surface_cairo_shape_arc_negative,
	.shape_stroke				= surface_cairo_shape_stroke,
	.shape_stroke_preserve		= surface_cairo_shape_stroke_preserve,
	.shape_fill					= surface_cairo_shape_fill,
	.shape_fill_preserve		= surface_cairo_shape_fill_preserve,
	.shape_reset_clip			= surface_cairo_shape_reset_clip,
	.shape_clip					= surface_cairo_shape_clip,
	.shape_clip_preserve		= surface_cairo_shape_clip_preserve,
	.shape_mask					= surface_cairo_shape_mask,
	.shape_mask_surface			= surface_cairo_shape_mask_surface,
	.shape_paint				= surface_cairo_shape_paint,

	.font_create				= surface_cairo_font_create,
	.font_destroy				= surface_cairo_font_destroy,

	.pattern_create				= surface_cairo_pattern_create,
	.pattern_create_color		= surface_cairo_pattern_create_color,
	.pattern_create_linear		= surface_cairo_pattern_create_linear,
	.pattern_create_radial		= surface_cairo_pattern_create_radial,
	.pattern_destroy			= surface_cairo_pattern_destroy,
	.pattern_add_color_stop		= surface_cairo_pattern_add_color_stop,
	.pattern_set_extend			= surface_cairo_pattern_set_extend,
	.pattern_set_filter			= surface_cairo_pattern_set_filter,
	.pattern_set_matrix			= surface_cairo_pattern_set_matrix,
};
