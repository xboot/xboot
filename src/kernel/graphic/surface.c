/*
 * kernel/graphic/surface.c
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
#include <png.h>
#include <pngstruct.h>
#include <jpeglib.h>
#include <jerror.h>
#include <graphic/surface.h>

static struct list_head __render_list = {
	.next = &__render_list,
	.prev = &__render_list,
};
static spinlock_t __render_lock = SPIN_LOCK_INIT();

static struct render_t render_default = {
	.name	 			= "default",

	.create				= render_default_create,
	.destroy			= render_default_destroy,

	.blit				= render_default_blit,
	.fill				= render_default_fill,
	.text				= render_default_text,

	.shape_line			= render_default_shape_line,
	.shape_polyline		= render_default_shape_polyline,
	.shape_curve		= render_default_shape_curve,
	.shape_triangle		= render_default_shape_triangle,
	.shape_rectangle	= render_default_shape_rectangle,
	.shape_polygon		= render_default_shape_polygon,
	.shape_circle		= render_default_shape_circle,
	.shape_ellipse		= render_default_shape_ellipse,
	.shape_arc			= render_default_shape_arc,
	.shape_raster		= render_default_shape_raster,

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

inline __attribute__((always_inline)) struct render_t * search_render(void)
{
	struct render_t * r = (struct render_t *)list_first_entry_or_null(&__render_list, struct render_t, list);
	if(!r)
		r = &render_default;
	return r;
}

bool_t register_render(struct render_t * r)
{
	irq_flags_t flags;

	if(!r || !r->name)
		return FALSE;

	init_list_head(&r->list);
	spin_lock_irqsave(&__render_lock, flags);
	list_add(&r->list, &__render_list);
	spin_unlock_irqrestore(&__render_lock, flags);

	return TRUE;
}

bool_t unregister_render(struct render_t * r)
{
	irq_flags_t flags;

	if(!r || !r->name)
		return FALSE;

	spin_lock_irqsave(&__render_lock, flags);
	list_del(&r->list);
	spin_unlock_irqrestore(&__render_lock, flags);

	return TRUE;
}

struct surface_t * surface_alloc(int width, int height, void * priv)
{
	struct surface_t * s;
	void * pixels;
	int stride, pixlen;

	if(width < 0 || height < 0)
		return NULL;

	s = malloc(sizeof(struct surface_t));
	if(!s)
		return NULL;

	stride = width << 2;
	pixlen = height * stride;
	pixels = memalign(4, pixlen);
	if(!pixels)
	{
		free(s);
		return NULL;
	}
	memset(pixels, 0, pixlen);

	s->width = width;
	s->height = height;
	s->stride = stride;
	s->pixlen = pixlen;
	s->pixels = pixels;
	s->r = search_render();
	s->pctx = s->r->create(s);
	s->priv = priv;
	return s;
}

void surface_free(struct surface_t * s)
{
	if(s)
	{
		if(s->r)
			s->r->destroy(s->pctx);
		free(s->pixels);
		free(s);
	}
}

static inline void blend_edge(uint32_t * d, uint32_t * s, int l)
{
	uint32_t v = *s;
	int a = (v >> 24) & 0xff;
	int r = (v >> 16) & 0xff;
	int g = (v >> 8) & 0xff;
	int b = (v >> 0) & 0xff;

	a = a * (32 - l) >> 5;
	r = r * (32 - l) >> 5;
	g = g * (32 - l) >> 5;
	b = b * (32 - l) >> 5;

	*d = (a << 24) | (r << 16) | (g << 8) | (b << 0);
}

struct surface_t * surface_clone(struct surface_t * s, int x, int y, int w, int h, int r)
{
	struct surface_t * o;
	uint32_t * dp, * sp;
	unsigned char * p, * q;
	void * pixels;
	int width, height, stride, pixlen;
	int swidth, sstride;
	int x1, y1, x2, y2;
	int r2, n, l;
	int i, j;

	if(!s)
		return NULL;

	if((w <= 0) || (h <= 0))
	{
		width = s->width;
		height = s->height;
		stride = s->stride;
		pixlen = s->pixlen;

		o = malloc(sizeof(struct surface_t));
		if(!o)
			return NULL;
		pixels = memalign(4, pixlen);
		if(!pixels)
		{
			free(o);
			return NULL;
		}
		memcpy(pixels, s->pixels, pixlen);
	}
	else
	{
		x1 = max(0, x);
		x2 = min(s->width, x + w);
		if(x1 <= x2)
		{
			y1 = max(0, y);
			y2 = min(s->height, y + h);
			if(y1 <= y2)
			{
				width = x2 - x1;
				height = y2 - y1;
				stride = width << 2;
				pixlen = height * stride;

				o = malloc(sizeof(struct surface_t));
				if(!o)
					return NULL;
				pixels = memalign(4, pixlen);
				if(!pixels)
				{
					free(o);
					return NULL;
				}
				if(r <= 0)
				{
					sstride = s->stride;
					p = (unsigned char *)pixels;
					q = (unsigned char *)s->pixels + y1 * sstride + (x1 << 2);
					for(i = 0; i < height; i++, p += stride, q += sstride)
						memcpy(p, q, stride);
				}
				else
				{
					swidth = s->width;
					sstride = s->stride;
					r = min(r, min(width >> 1, height >> 1));
					r2 = r * r;

					p = (unsigned char *)pixels + (r << 2);
					q = (unsigned char *)s->pixels + y1 * sstride + ((x1 + r) << 2);
					l = stride - (r << (1 + 2));
					for(i = 0; i < r; i++, p += stride, q += sstride)
						memcpy(p, q, l);

					p = (unsigned char *)pixels + r * stride;
					q = (unsigned char *)s->pixels + (y1 + r) * sstride + (x1 << 2);
					l = stride;
					for(i = 0; i < y2 - y1 - (r << 1); i++, p += stride, q += sstride)
						memcpy(p, q, l);

					p = (unsigned char *)pixels + (y2 - y1 - r) * stride + (r << 2);
					q = (unsigned char *)s->pixels + (y2 - r) * sstride + ((x1 + r) << 2);
					l = stride - (r << (1 + 2));
					for(i = 0; i < r; i++, p += stride, q += sstride)
						memcpy(p, q, l);

					for(i = 0; i < r; i++)
					{
						dp = (uint32_t *)pixels + i * width;
						sp = (uint32_t *)s->pixels + (y1 + i) * swidth + x1;
						n = (r - i) * (r - i);
						for(j = 0; j < r; j++, dp++, sp++)
						{
							l = n + (r - j) * (r - j);
							if(l < r2)
								*dp = *sp;
							else if(l < r2 + 32)
								blend_edge(dp, sp, l - r2);
							else
								*dp = 0;
						}
					}

					for(i = 0; i < r; i++)
					{
						dp = (uint32_t *)pixels + i * width + (width - r);
						sp = (uint32_t *)s->pixels + (y1 + i) * swidth + (x2 - r);
						n = (r - i) * (r - i);
						for(j = 0; j < r; j++, dp++, sp++)
						{
							l = n + j * j;
							if(l < r2)
								*dp = *sp;
							else if(l < r2 + 32)
								blend_edge(dp, sp, l - r2);
							else
								*dp = 0;
						}
					}

					for(i = 0; i < r; i++)
					{
						dp = (uint32_t *)pixels + (height - r + i) * width;
						sp = (uint32_t *)s->pixels + (y2 - r + i) * swidth + x1;
						n = i * i;
						for(j = 0; j < r; j++, dp++, sp++)
						{
							l = n + (r - j) * (r - j);
							if(l < r2)
								*dp = *sp;
							else if(l < r2 + 32)
								blend_edge(dp, sp, l - r2);
							else
								*dp = 0;
						}
					}

					for(i = 0; i < r; i++)
					{
						dp = (uint32_t *)pixels + (height - r + i) * width + (width - r);
						sp = (uint32_t *)s->pixels + (y2 - r + i) * swidth + (x2 - r);
						n = i * i;
						for(j = 0; j < r; j++, dp++, sp++)
						{
							l = n + j * j;
							if(l < r2)
								*dp = *sp;
							else if(l < r2 + 32)
								blend_edge(dp, sp, l - r2);
							else
								*dp = 0;
						}
					}
				}
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
	}

	o->width = width;
	o->height = height;
	o->stride = stride;
	o->pixlen = pixlen;
	o->pixels = pixels;
	o->r = s->r;
	o->pctx = o->r->create(o);
	o->priv = NULL;
	return o;
}

struct surface_t * surface_extend(struct surface_t * s, int width, int height, const char * type)
{
	struct surface_t * o;
	uint32_t * dp, * sp;
	void * pixels, * spixels;
	int stride, pixlen;
	int sw, sh, x, y;

	if(!s || (width <= 0) || (height <= 0))
		return NULL;

	o = malloc(sizeof(struct surface_t));
	if(!o)
		return NULL;

	stride = width << 2;
	pixlen = height * stride;
	pixels = memalign(4, pixlen);
	if(!pixels)
	{
		free(s);
		return NULL;
	}
	spixels = s->pixels;
	sw = s->width;
	sh = s->height;

	switch(shash(type))
	{
	case 0x192dec66: /* "repeat" */
		for(y = 0, dp = (uint32_t *)pixels; y < height; y++)
		{
			for(x = 0, sp = (uint32_t *)spixels + (y % sh) * sw; x < width; x++)
			{
				*dp++ = *(sp + (x % sw));
			}
		}
		break;
	case 0x3e3a6a0a: /* "reflect" */
		for(y = 0, dp = (uint32_t *)pixels; y < height; y++)
		{
			for(x = 0, sp = (uint32_t *)spixels + (((y / sh) & 0x1) ? (sh - 1 - (y % sh)) : (y % sh)) * sw; x < width; x++)
			{
				*dp++ = *(sp + (((x / sw) & 0x1) ? (sw - 1 - (x % sw)) : (x % sw)));
			}
		}
		break;
	case 0x0b889c3a: /* "pad" */
		for(y = 0, dp = (uint32_t *)pixels; y < height; y++)
		{
			for(x = 0, sp = (uint32_t *)spixels + ((y < sh) ? y : sh - 1) * sw; x < width; x++)
			{
				*dp++ = *(sp + ((x < sw) ? x : sw - 1));
			}
		}
		break;
	default:
		for(y = 0, dp = (uint32_t *)pixels; y < height; y++)
		{
			if(y < sh)
			{
				for(x = 0, sp = (uint32_t *)spixels + y * sw; x < width; x++)
				{
					if(x < sw)
						*dp++ = *(sp + x);
					else
						*dp++ = 0;
				}
			}
			else
			{
				memset(dp, 0, stride);
				dp += width;
			}
		}
		break;
	}

	o->width = width;
	o->height = height;
	o->stride = stride;
	o->pixlen = pixlen;
	o->pixels = pixels;
	o->r = s->r;
	o->pctx = o->r->create(o);
	o->priv = NULL;
	return o;
}

void surface_clear(struct surface_t * s, struct color_t * c, int x, int y, int w, int h)
{
	uint32_t * q, * p, v;
	int x1, y1, x2, y2;
	int i, j, l;

	if(s)
	{
		v = c ? color_get_premult(c) : 0;
		if((w <= 0) || (h <= 0))
		{
			if(v)
			{
				if(v == 0xffffffff)
				{
					memset(s->pixels, 0xff, s->pixlen);
				}
				else
				{
					p = (uint32_t * )s->pixels;
					l = s->width * s->height;
					while(l--)
						*p++ = v;
				}
			}
			else
			{
				memset(s->pixels, 0, s->pixlen);
			}
		}
		else
		{
			x1 = max(0, x);
			x2 = min(s->width, x + w);
			if(x1 <= x2)
			{
				y1 = max(0, y);
				y2 = min(s->height, y + h);
				if(y1 <= y2)
				{
					l = s->stride >> 2;
					q = (uint32_t *)s->pixels + y1 * l + x1;
					for(j = y1; j < y2; j++, q += l)
					{
						for(i = x1, p = q; i < x2; i++, p++)
							*p = v;
					}
				}
			}
		}
	}
}

void surface_set_pixel(struct surface_t * s, int x, int y, struct color_t * c)
{
	if(c && s && (x < s->width) && (y < s->height))
	{
		uint32_t * p = (uint32_t *)s->pixels + y * (s->stride >> 2) + x;
		*p = color_get_premult(c);
	}
}

void surface_get_pixel(struct surface_t * s, int x, int y, struct color_t * c)
{
	if(c)
	{
		if(s && (x < s->width) && (y < s->height))
		{
			uint32_t * p = (uint32_t *)s->pixels + y * (s->stride >> 2) + x;
			color_set_premult(c, *p);
		}
		else
		{
			memset(&c, 0, sizeof(struct color_t));
		}
	}
}

static void png_xfs_read_data(png_structp png, png_bytep data, size_t length)
{
	size_t check;

	if(png == NULL)
		return;
	check = xfs_read((struct xfs_file_t *)png->io_ptr, data, length);
	if(check != length)
		png_error(png, "Read Error");
}

static inline int multiply_alpha(int alpha, int color)
{
	int temp = (alpha * color) + 0x80;
	return ((temp + (temp >> 8)) >> 8);
}

static void premultiply_data(png_structp png, png_row_infop row_info, png_bytep data)
{
	unsigned int i;

	for(i = 0; i < row_info->rowbytes; i += 4)
	{
		uint8_t * base = &data[i];
		uint8_t alpha = base[3];
		uint32_t p;

		if(alpha == 0)
		{
			p = 0;
		}
		else
		{
			uint8_t red = base[0];
			uint8_t green = base[1];
			uint8_t blue = base[2];

			if(alpha != 0xff)
			{
				red = multiply_alpha(alpha, red);
				green = multiply_alpha(alpha, green);
				blue = multiply_alpha(alpha, blue);
			}
			p = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
		}
		memcpy(base, &p, sizeof(uint32_t));
	}
}

static void convert_bytes_to_data(png_structp png, png_row_infop row_info, png_bytep data)
{
	unsigned int i;

	for(i = 0; i < row_info->rowbytes; i += 4)
	{
		uint8_t * base = &data[i];
		uint8_t red = base[0];
		uint8_t green = base[1];
		uint8_t blue = base[2];
		uint32_t pixel;

		pixel = (0xff << 24) | (red << 16) | (green << 8) | (blue << 0);
		memcpy(base, &pixel, sizeof(uint32_t));
	}
}

static inline struct surface_t * surface_alloc_from_xfs_png(struct xfs_context_t * ctx, const char * filename)
{
	struct surface_t * s;
	png_struct * png;
	png_info * info;
	png_byte * data = NULL;
	png_byte ** row_pointers = NULL;
	png_uint_32 png_width, png_height;
	int depth, color_type, interlace, stride;
	unsigned int i;
	struct xfs_file_t * file;

	if(!(file = xfs_open_read(ctx, filename)))
		return NULL;

	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png)
	{
		xfs_close(file);
		return NULL;
	}

	info = png_create_info_struct(png);
	if(!info)
	{
		xfs_close(file);
		png_destroy_read_struct(&png, NULL, NULL);
		return NULL;
	}

	png_set_read_fn(png, file, png_xfs_read_data);

#ifdef PNG_SETJMP_SUPPORTED
	if(setjmp(png_jmpbuf(png)))
	{
		png_destroy_read_struct(&png, &info, NULL);
		xfs_close(file);
		return NULL;
	}
#endif

	png_read_info(png, info);
	png_get_IHDR(png, info, &png_width, &png_height, &depth, &color_type, &interlace, NULL, NULL);

	if(color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);
	if(color_type == PNG_COLOR_TYPE_GRAY)
	{
#if PNG_LIBPNG_VER >= 10209
		png_set_expand_gray_1_2_4_to_8(png);
#else
		png_set_gray_1_2_4_to_8(png);
#endif
	}
	if(png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);
	if(depth == 16)
		png_set_strip_16(png);
	if(depth < 8)
		png_set_packing(png);
	if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);
	if(interlace != PNG_INTERLACE_NONE)
		png_set_interlace_handling(png);

	png_set_filler(png, 0xff, PNG_FILLER_AFTER);
	png_read_update_info(png, info);
	png_get_IHDR(png, info, &png_width, &png_height, &depth, &color_type, &interlace, NULL, NULL);

	if(depth != 8 || !(color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA))
	{
		png_destroy_read_struct(&png, &info, NULL);
		xfs_close(file);
		return NULL;
	}

	switch(color_type)
	{
	case PNG_COLOR_TYPE_RGB_ALPHA:
		png_set_read_user_transform_fn(png, premultiply_data);
		break;
	case PNG_COLOR_TYPE_RGB:
		png_set_read_user_transform_fn(png, convert_bytes_to_data);
		break;
	default:
		break;
	}

	s = surface_alloc(png_width, png_height, NULL);
	data = surface_get_pixels(s);

	row_pointers = (png_byte **)malloc(png_height * sizeof(char *));
	stride = png_width * 4;

	for(i = 0; i < png_height; i++)
		row_pointers[i] = &data[i * stride];

	png_read_image(png, row_pointers);
	png_read_end(png, info);
	free(row_pointers);
	png_destroy_read_struct(&png, &info, NULL);
	xfs_close(file);

	return s;
}

struct my_error_mgr
{
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

struct my_source_mgr
{
	struct jpeg_source_mgr pub;
	struct xfs_file_t * file;
	JOCTET * buffer;
	int start_of_file;
};

static void my_error_exit(j_common_ptr cinfo)
{
	struct my_error_mgr * myerr = (struct my_error_mgr *)cinfo->err;
	(*cinfo->err->output_message)(cinfo);
	longjmp(myerr->setjmp_buffer, 1);
}

static void init_source(j_decompress_ptr cinfo)
{
	struct my_source_mgr * src = (struct my_source_mgr *)cinfo->src;
	src->start_of_file = 1;
}

static boolean fill_input_buffer(j_decompress_ptr cinfo)
{
	struct my_source_mgr * src = (struct my_source_mgr *)cinfo->src;
	size_t nbytes;

	nbytes = xfs_read(src->file, src->buffer, 4096);
	if(nbytes <= 0)
	{
		if(src->start_of_file)
			ERREXIT(cinfo, JERR_INPUT_EMPTY);
		WARNMS(cinfo, JWRN_JPEG_EOF);
		src->buffer[0] = (JOCTET)0xFF;
		src->buffer[1] = (JOCTET)JPEG_EOI;
		nbytes = 2;
	}
	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_file = 0;
	return 1;
}

static void skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	struct jpeg_source_mgr * src = cinfo->src;

	if(num_bytes > 0)
	{
		while(num_bytes > (long)src->bytes_in_buffer)
		{
			num_bytes -= (long)src->bytes_in_buffer;
			(void)(*src->fill_input_buffer)(cinfo);
		}
		src->next_input_byte += (size_t)num_bytes;
		src->bytes_in_buffer -= (size_t)num_bytes;
	}
}

static void term_source(j_decompress_ptr cinfo)
{
}

static void jpeg_xfs_src(j_decompress_ptr cinfo, struct xfs_file_t * file)
{
	struct my_source_mgr * src;

	if(cinfo->src == NULL)
	{
		cinfo->src = (struct jpeg_source_mgr *)(*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_PERMANENT, sizeof(struct my_source_mgr));
		src = (struct my_source_mgr *)cinfo->src;
		src->buffer = (JOCTET *)(*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_PERMANENT, 4096 * sizeof(JOCTET));
	}

	src = (struct my_source_mgr *)cinfo->src;
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buffer;
	src->pub.skip_input_data = skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart;
	src->pub.term_source = term_source;
	src->file = file;
	src->pub.bytes_in_buffer = 0;
	src->pub.next_input_byte = NULL;
}

static inline struct surface_t * surface_alloc_from_xfs_jpeg(struct xfs_context_t * ctx, const char * filename)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	struct surface_t * s;
	struct xfs_file_t * file;
	JSAMPARRAY buf;
	unsigned char * p;
	int scanline, offset, i;

	if(!(file = xfs_open_read(ctx, filename)))
		return NULL;
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if(setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_decompress(&cinfo);
		xfs_close(file);
		return 0;
	}
	jpeg_create_decompress(&cinfo);
	jpeg_xfs_src(&cinfo, file);
	jpeg_read_header(&cinfo, 1);
	jpeg_start_decompress(&cinfo);
	buf = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, cinfo.output_width * cinfo.output_components, 1);
	s = surface_alloc(cinfo.image_width, cinfo.image_height, NULL);
	p = surface_get_pixels(s);
	while(cinfo.output_scanline < cinfo.output_height)
	{
		scanline = cinfo.output_scanline * surface_get_stride(s);
		jpeg_read_scanlines(&cinfo, buf, 1);
		for(i = 0; i < cinfo.output_width; i++)
		{
			offset = scanline + (i * 4);
			p[offset + 3] = 0xff;
			p[offset + 2] = buf[0][(i * 3) + 0];
			p[offset + 1] = buf[0][(i * 3) + 1];
			p[offset] = buf[0][(i * 3) + 2];
		}
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	xfs_close(file);

	return s;
}

static const char * fileext(const char * filename)
{
	const char * ret = NULL;
	const char * p;

	if(filename != NULL)
	{
		ret = p = strchr(filename, '.');
		while(p != NULL)
		{
			p = strchr(p + 1, '.');
			if(p != NULL)
				ret = p;
		}
		if(ret != NULL)
			ret++;
	}
	return ret;
}

struct surface_t * surface_alloc_from_xfs(struct xfs_context_t * ctx, const char * filename)
{
	const char * ext = fileext(filename);
	if(strcasecmp(ext, "png") == 0)
		return surface_alloc_from_xfs_png(ctx, filename);
	else if((strcasecmp(ext, "jpg") == 0) || (strcasecmp(ext, "jpeg") == 0))
		return surface_alloc_from_xfs_jpeg(ctx, filename);
	return NULL;
}
