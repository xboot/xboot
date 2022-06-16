/*
 * kernel/graphic/surface.c
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
#include <png.h>
#include <pngstruct.h>
#include <jpeglib.h>
#include <jerror.h>
#include <qrcgen.h>
#include <graphic/surface.h>

static struct list_head __render_list = {
	.next = &__render_list,
	.prev = &__render_list,
};
static spinlock_t __render_lock = SPIN_LOCK_INIT();

static struct render_t render_default = {
	.name	 				= "default",

	.create					= render_default_create,
	.destroy				= render_default_destroy,

	.blit					= render_default_blit,
	.fill					= render_default_fill,
	.text					= render_default_text,
	.icon					= render_default_icon,

	.shape_line				= render_default_shape_line,
	.shape_polyline			= render_default_shape_polyline,
	.shape_curve			= render_default_shape_curve,
	.shape_triangle			= render_default_shape_triangle,
	.shape_rectangle		= render_default_shape_rectangle,
	.shape_polygon			= render_default_shape_polygon,
	.shape_circle			= render_default_shape_circle,
	.shape_ellipse			= render_default_shape_ellipse,
	.shape_arc				= render_default_shape_arc,
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
	pixels = malloc(pixlen);
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
	s->rctx = s->r->create(s);
	s->priv = priv;
	return s;
}

void surface_free(struct surface_t * s)
{
	if(s)
	{
		if(s->r)
			s->r->destroy(s->rctx);
		free(s->pixels);
		free(s);
	}
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

static void png_xfs_read_data(png_structp png, png_bytep data, size_t length)
{
	size_t check;

	if(png == NULL)
		return;
	check = xfs_read((struct xfs_file_t *)png->io_ptr, data, length);
	if(check != length)
		png_error(png, "Read Error");
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

struct png_source_t {
	unsigned char * data;
	int size;
	int offset;
};

static void png_source_read_data(png_structp png, png_bytep data, png_size_t length)
{
	struct png_source_t * src = (struct png_source_t *)png_get_io_ptr(png);

	if(src->offset + length <= src->size)
	{
		memcpy(data, src->data + src->offset, length);
		src->offset += length;
	}
	else
		png_error(png, "read error");
}

static inline struct surface_t * surface_alloc_from_buf_png(const void * buf, int len)
{
	struct surface_t * s;
	png_struct * png;
	png_info * info;
	png_byte * data = NULL;
	png_byte ** row_pointers = NULL;
	png_uint_32 png_width, png_height;
	int depth, color_type, interlace, stride;
	unsigned int i;

	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png)
		return NULL;

	info = png_create_info_struct(png);
	if(!info)
	{
		png_destroy_read_struct(&png, NULL, NULL);
		return NULL;
	}

	struct png_source_t src;
	src.data = (unsigned char *)buf;
	src.size = len;
	src.offset = 0;
	png_set_read_fn(png, &src, png_source_read_data);

#ifdef PNG_SETJMP_SUPPORTED
	if(setjmp(png_jmpbuf(png)))
	{
		png_destroy_read_struct(&png, &info, NULL);
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

	return s;
}

struct x_error_mgr
{
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

struct x_source_mgr
{
	struct jpeg_source_mgr pub;
	struct xfs_file_t * file;
	JOCTET * buffer;
	int start_of_file;
};

static void x_error_exit(j_common_ptr dinfo)
{
	struct x_error_mgr * err = (struct x_error_mgr *)dinfo->err;
	(*dinfo->err->output_message)(dinfo);
	longjmp(err->setjmp_buffer, 1);
}

static void x_emit_message(j_common_ptr dinfo, int msg_level)
{
	struct jpeg_error_mgr * err = dinfo->err;
	if(msg_level < 0)
		err->num_warnings++;
}

static void init_source(j_decompress_ptr dinfo)
{
	struct x_source_mgr * src = (struct x_source_mgr *)dinfo->src;
	src->start_of_file = 1;
}

static boolean fill_input_buffer(j_decompress_ptr dinfo)
{
	struct x_source_mgr * src = (struct x_source_mgr *)dinfo->src;
	size_t nbytes;

	nbytes = xfs_read(src->file, src->buffer, 4096);
	if(nbytes <= 0)
	{
		if(src->start_of_file)
			ERREXIT(dinfo, JERR_INPUT_EMPTY);
		WARNMS(dinfo, JWRN_JPEG_EOF);
		src->buffer[0] = (JOCTET)0xFF;
		src->buffer[1] = (JOCTET)JPEG_EOI;
		nbytes = 2;
	}
	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_file = 0;
	return 1;
}

static void skip_input_data(j_decompress_ptr dinfo, long num_bytes)
{
	struct jpeg_source_mgr * src = dinfo->src;

	if(num_bytes > 0)
	{
		while(num_bytes > (long)src->bytes_in_buffer)
		{
			num_bytes -= (long)src->bytes_in_buffer;
			(void)(*src->fill_input_buffer)(dinfo);
		}
		src->next_input_byte += (size_t)num_bytes;
		src->bytes_in_buffer -= (size_t)num_bytes;
	}
}

static void term_source(j_decompress_ptr dinfo)
{
}

static void jpeg_xfs_src(j_decompress_ptr dinfo, struct xfs_file_t * file)
{
	struct x_source_mgr * src;

	if(dinfo->src == NULL)
	{
		dinfo->src = (struct jpeg_source_mgr *)(*dinfo->mem->alloc_small)((j_common_ptr)dinfo, JPOOL_PERMANENT, sizeof(struct x_source_mgr));
		src = (struct x_source_mgr *)dinfo->src;
		src->buffer = (JOCTET *)(*dinfo->mem->alloc_small)((j_common_ptr)dinfo, JPOOL_PERMANENT, 4096 * sizeof(JOCTET));
	}

	src = (struct x_source_mgr *)dinfo->src;
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buffer;
	src->pub.skip_input_data = skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart;
	src->pub.term_source = term_source;
	src->file = file;
	src->pub.bytes_in_buffer = 0;
	src->pub.next_input_byte = NULL;
}

static inline struct surface_t * surface_alloc_from_xfs_jpg(struct xfs_context_t * ctx, const char * filename)
{
	struct jpeg_decompress_struct dinfo;
	struct x_error_mgr jerr;
	struct surface_t * s;
	struct xfs_file_t * file;
	JSAMPARRAY tmp;
	unsigned char * p;
	int scanline, offset, i;

	if(!(file = xfs_open_read(ctx, filename)))
		return NULL;
	dinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = x_error_exit;
	jerr.pub.emit_message = x_emit_message;
	if(setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_decompress(&dinfo);
		xfs_close(file);
		return 0;
	}
	jpeg_create_decompress(&dinfo);
	jpeg_xfs_src(&dinfo, file);
	jpeg_read_header(&dinfo, 1);
	jpeg_start_decompress(&dinfo);
	tmp = (*dinfo.mem->alloc_sarray)((j_common_ptr)&dinfo, JPOOL_IMAGE, dinfo.output_width * dinfo.output_components, 1);
	s = surface_alloc(dinfo.image_width, dinfo.image_height, NULL);
	p = surface_get_pixels(s);
	while(dinfo.output_scanline < dinfo.output_height)
	{
		scanline = dinfo.output_scanline * surface_get_stride(s);
		jpeg_read_scanlines(&dinfo, tmp, 1);
		for(i = 0; i < dinfo.output_width; i++)
		{
			offset = scanline + (i * 4);
			p[offset + 3] = 0xff;
			p[offset + 2] = tmp[0][(i * 3) + 0];
			p[offset + 1] = tmp[0][(i * 3) + 1];
			p[offset + 0] = tmp[0][(i * 3) + 2];
		}
	}
	jpeg_finish_decompress(&dinfo);
	jpeg_destroy_decompress(&dinfo);
	xfs_close(file);

	return s;
}

static inline struct surface_t * surface_alloc_from_buf_jpg(const void * buf, int len)
{
	struct jpeg_decompress_struct dinfo;
	struct x_error_mgr jerr;
	struct surface_t * s;
	JSAMPARRAY tmp;
	unsigned char * p;
	int scanline, offset, i;

	dinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = x_error_exit;
	jerr.pub.emit_message = x_emit_message;
	if(setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_decompress(&dinfo);
		return NULL;
	}
	jpeg_create_decompress(&dinfo);
	jpeg_mem_src(&dinfo, buf, len);
	jpeg_read_header(&dinfo, 1);
	jpeg_start_decompress(&dinfo);
	tmp = (*dinfo.mem->alloc_sarray)((j_common_ptr)&dinfo, JPOOL_IMAGE, dinfo.output_width * dinfo.output_components, 1);
	s = surface_alloc(dinfo.image_width, dinfo.image_height, NULL);
	p = surface_get_pixels(s);
	while(dinfo.output_scanline < dinfo.output_height)
	{
		scanline = dinfo.output_scanline * surface_get_stride(s);
		jpeg_read_scanlines(&dinfo, tmp, 1);
		for(i = 0; i < dinfo.output_width; i++)
		{
			offset = scanline + (i * 4);
			p[offset + 3] = 0xff;
			p[offset + 2] = tmp[0][(i * 3) + 0];
			p[offset + 1] = tmp[0][(i * 3) + 1];
			p[offset + 0] = tmp[0][(i * 3) + 2];
		}
	}
	jpeg_finish_decompress(&dinfo);
	jpeg_destroy_decompress(&dinfo);

	return s;
}

struct surface_t * surface_alloc_from_xfs(struct xfs_context_t * ctx, const char * filename)
{
	const char * ext = fileext(filename);
	if(strcasecmp(ext, "png") == 0)
		return surface_alloc_from_xfs_png(ctx, filename);
	else if((strcasecmp(ext, "jpg") == 0) || (strcasecmp(ext, "jpeg") == 0))
		return surface_alloc_from_xfs_jpg(ctx, filename);
	return NULL;
}

struct surface_t * surface_alloc_from_buf(const void * buf, int len)
{
	struct surface_t * s = NULL;

	if(buf && (len > 0))
	{
		s = surface_alloc_from_buf_png(buf, len);
		if(!s)
			s = surface_alloc_from_buf_jpg(buf, len);
	}
	return s;
}

struct surface_t * surface_alloc_qrcode(const char * txt, int pixsz)
{
	struct surface_t * s;
	uint32_t * p, * q;
	uint8_t qrc[QRCGEN_BUFFER_LEN_MAX];
	uint8_t tmp[QRCGEN_BUFFER_LEN_MAX];
	int qrs, i, j;
	int x1, y1, x2, y2;
	int l, x, y;

	if(qrcgen_encode_text(txt, tmp, qrc, QRCGEN_ECC_MEDIUM, QRCGEN_VERSION_MIN, QRCGEN_VERSION_MAX, QRCGEN_MASK_AUTO, 1))
	{
		qrs = qrcgen_get_size(qrc);
		if(qrs > 0)
		{
			if(pixsz < 0)
				pixsz = 1;
			s = surface_alloc((qrs + 4) * pixsz, (qrs + 4) * pixsz, NULL);
			if(s)
			{
				memset(s->pixels, 0xff, s->pixlen);
				l = s->stride >> 2;
				for(j = 0; j < qrs; j++)
				{
					for(i = 0; i < qrs; i++)
					{
						if(qrcgen_get_pixel(qrc, i, j))
						{
							x1 = (i + 2) * pixsz;
							y1 = (j + 2) * pixsz;
							x2 = x1 + pixsz;
							y2 = y1 + pixsz;
							q = (uint32_t *)s->pixels + y1 * l + x1;
							for(y = y1; y < y2; y++, q += l)
							{
								for(x = x1, p = q; x < x2; x++, p++)
									*p = 0xff000000;
							}
						}
					}
				}
				return s;
			}
		}
	}
	return NULL;
}

static inline void blend(uint32_t * d, uint32_t * s)
{
	uint32_t dv, sv = *s;
	uint8_t da, dr, dg, db;
	uint8_t sa, sr, sg, sb;
	uint8_t a, r, g, b;
	int t;

	sa = (sv >> 24) & 0xff;
	if(sa == 255)
	{
		*d = sv;
	}
	else if(sa != 0)
	{
		sr = (sv >> 16) & 0xff;
		sg = (sv >> 8) & 0xff;
		sb = (sv >> 0) & 0xff;
		dv = *d;
		da = (dv >> 24) & 0xff;
		dr = (dv >> 16) & 0xff;
		dg = (dv >> 8) & 0xff;
		db = (dv >> 0) & 0xff;
		t = sa + (sa >> 8);
		a = (((sa + da) << 8) - da * t) >> 8;
		r = (((sr + dr) << 8) - dr * t) >> 8;
		g = (((sg + dg) << 8) - dg * t) >> 8;
		b = (((sb + db) << 8) - db * t) >> 8;
		*d = (a << 24) | (r << 16) | (g << 8) | (b << 0);
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
		pixels = malloc(pixlen);
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
				pixels = malloc(pixlen);
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
	o->rctx = o->r->create(o);
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
	pixels = malloc(pixlen);
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
	o->rctx = o->r->create(o);
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

static inline void blurrow(unsigned char * pixel, int width, int height, int x, int y, int w, int index, int alpha)
{
	unsigned char * p = pixel + (y + index) * (width << 2) + (x << 2);
	int zr, zg, zb, za;
	int i;

	zb = p[0] << 7;
	zg = p[1] << 7;
	zr = p[2] << 7;
	za = p[3] << 7;
	for(i = 0; i < w; i++, p += 4)
		blurinner(p, &zr, &zg, &zb, &za, alpha);
	for(i = w - 2, p -= 8; i >= 0; i--, p -= 4)
		blurinner(p, &zr, &zg, &zb, &za, alpha);
}

static inline void blurcol(unsigned char * pixel, int width, int height, int x, int y, int h, int index, int alpha)
{
	int stride = width << 2;
	unsigned char * p = pixel + (y + 1) * stride + ((x + index) << 2);
	int zr, zg, zb, za;
	int i;

	zb = p[0] << 7;
	zg = p[1] << 7;
	zr = p[2] << 7;
	za = p[3] << 7;
	for(i = 1; i < h; i++, p += stride)
		blurinner(p, &zr, &zg, &zb, &za, alpha);
	for(i = h - 2, p -= (stride << 1); i >= 0; i--, p -= stride)
		blurinner(p, &zr, &zg, &zb, &za, alpha);
}

static void expblur(unsigned char * pixel, int width, int height, int x, int y, int w, int h, int radius)
{
	int alpha = (int)((1 << 16) * (1.0 - expf(-2.3 / (radius + 1.0))));
	int i;

	for(i = 0; i < h; i++)
		blurrow(pixel, width, height, x, y, w, i, alpha);
	for(i = 0; i < w; i++)
		blurcol(pixel, width, height, x, y, h, i, alpha);
}

void surface_effect_glass(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h, int radius)
{
	struct region_t region, r;

	if(radius > 0)
	{
		region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
		if(clip)
		{
			if(!region_intersect(&r, &r, clip))
				return;
		}
		region_init(&region, x, y, w, h);
		if(!region_intersect(&r, &r, &region))
			return;
		expblur(surface_get_pixels(s), surface_get_width(s), surface_get_height(s), r.x, r.y, r.w, r.h, radius);
	}
}

void surface_effect_shadow(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h, int radius, struct color_t * c)
{
	struct region_t region, r;
	uint32_t * p, * q;
	int i, j;

	if(radius > 0)
	{
		region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
		if(clip)
		{
			if(!region_intersect(&r, &r, clip))
				return;
		}
		int r2 = radius << 1;
		int r4 = radius << 2;
		region_init(&region, x - r2, y - r2, w + r4, h + r4);
		if(!region_intersect(&r, &r, &region))
			return;
		int tw = w + r4;
		int th = h + r4;
		int pixlen = th * (tw << 2);
		void * pixels = malloc(pixlen);
		if(pixels)
		{
			memset(pixels, 0, pixlen);
			int x1 = r2;
			int x2 = r2 + w;
			int y1 = r2;
			int y2 = r2 + h;
			uint32_t v = color_get_premult(c);
			q = (uint32_t *)pixels + y1 * tw + x1;
			for(j = y1; j < y2; j++, q += tw)
			{
				for(i = x1, p = q; i < x2; i++, p++)
					*p = v;
			}
			expblur(pixels, tw, th, 0, 0, tw, th, radius);
			p = (uint32_t *)s->pixels + r.y * s->width + r.x;
			q = (uint32_t *)pixels + (max(region.y, r.y) - region.y) * tw + (max(region.x, r.x) - region.x);
			for(j = 0; j < r.h; j++, p += s->width, q += tw)
			{
				for(i = 0; i < r.w; i++)
				{
					blend(p + i, q + i);
				}
			}
			free(pixels);
		}
	}
}

void surface_effect_gradient(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h, struct color_t * lt, struct color_t * rt, struct color_t * rb, struct color_t * lb)
{
	struct region_t region, r;
	struct color_t cl, cr;
	unsigned char * p, * q;
	int stride;
	int x0, y0, x1, y1;
	int i, j, u, v, t;
	uint8_t da, dr, dg, db;
	uint8_t sa, sr, sg, sb;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return;
	}
	region_init(&region, x, y, w, h);
	if(!region_intersect(&r, &r, &region))
		return;
	stride = surface_get_stride(s);
	q = (unsigned char *)surface_get_pixels(s) + y * stride + (x << 2);
	x0 = r.x - x;
	y0 = r.y - y;
	x1 = x0 + r.w;
	y1 = y0 + r.h;
	for(j = 0; j < h; j++, q += stride)
	{
		if((j >= y0) && (j < y1))
		{
			if(h > 1)
				u = (j << 8) / (h - 1);
			else
				u = 0;
			v = 256 - u;
			cl.b = (lt->b * v + lb->b * u) >> 8;
			cl.g = (lt->g * v + lb->g * u) >> 8;
			cl.r = (lt->r * v + lb->r * u) >> 8;
			cl.a = (lt->a * v + lb->a * u) >> 8;
			cr.b = (rt->b * v + rb->b * u) >> 8;
			cr.g = (rt->g * v + rb->g * u) >> 8;
			cr.r = (rt->r * v + rb->r * u) >> 8;
			cr.a = (rt->a * v + rb->a * u) >> 8;
			for(i = 0, p = q; i < w; i++, p += 4)
			{
				if((i >= x0) && (i < x1))
				{
					if(w > 1)
						u = (i << 8) / (w - 1);
					else
						u = 0;
					v = 256 - u;
					sa = (cl.a * v + cr.a * u) >> 8;
					if(sa != 0)
					{
						if(sa == 255)
						{
							p[0] = (cl.b * v + cr.b * u) >> 8;
							p[1] = (cl.g * v + cr.g * u) >> 8;
							p[2] = (cl.r * v + cr.r * u) >> 8;
							p[3] = sa;
						}
						else
						{
							sr = idiv255(((cl.r * v + cr.r * u) >> 8) * sa);
							sg = idiv255(((cl.g * v + cr.g * u) >> 8) * sa);
							sb = idiv255(((cl.b * v + cr.b * u) >> 8) * sa);
							db = p[0];
							dg = p[1];
							dr = p[2];
							da = p[3];
							t = sa + (sa >> 8);
							p[3] = (((sa + da) << 8) - da * t) >> 8;
							p[2] = (((sr + dr) << 8) - dr * t) >> 8;
							p[1] = (((sg + dg) << 8) - dg * t) >> 8;
							p[0] = (((sb + db) << 8) - db * t) >> 8;
						}
					}
				}
			}
		}
	}
}

void surface_effect_checkerboard(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h)
{
	struct region_t region, r;
	uint32_t * q, * p;
	int x1, y1, x2, y2;
	int i, j, l;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return;
	}
	region_init(&region, x, y, w, h);
	if(!region_intersect(&r, &r, &region))
		return;

	x1 = r.x;
	y1 = r.y;
	x2 = r.x + r.w;
	y2 = r.y + r.h;
	l = s->stride >> 2;
	q = (uint32_t *)s->pixels + y1 * l + x1;

	for(j = y1; j < y2; j++, q += l)
	{
		for(i = x1, p = q; i < x2; i++, p++)
		{
			if((i ^ j) & (1 << 3))
				*p = 0xffabb9bd;
			else
				*p = 0xff899598;
		}
	}
}

void surface_filter_gray(struct surface_t * s)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	unsigned char gray;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			gray = (p[2] * 19595L + p[1] * 38469L + p[0] * 7472L) >> 16;
			p[0] = gray;
			p[1] = gray;
			p[2] = gray;
		}
	}
}

void surface_filter_sepia(struct surface_t * s)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	int r, g, b;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			b = (p[2] * 17826L + p[1] * 34996L + p[0] * 8585L) >> 16;
			g = (p[2] * 22872L + p[1] * 44958L + p[0] * 11010L) >> 16;
			r = (p[2] * 25756L + p[1] * 50397L + p[0] * 12386L) >> 16;
			p[0] = min(b, 255);
			p[1] = min(g, 255);
			p[2] = min(r, 255);
		}
	}
}

void surface_filter_invert(struct surface_t * s)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			p[0] = p[3] - p[0];
			p[1] = p[3] - p[1];
			p[2] = p[3] - p[2];
		}
	}
}

void surface_filter_coloring(struct surface_t * s, struct color_t * c)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	unsigned char r = c->r;
	unsigned char g = c->g;
	unsigned char b = c->b;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] == 255)
		{
			p[0] = b;
			p[1] = g;
			p[2] = r;
		}
		else
		{
			p[0] = idiv255(b * p[3]);
			p[1] = idiv255(g * p[3]);
			p[2] = idiv255(r * p[3]);
		}
	}
}

void surface_filter_hue(struct surface_t * s, int angle)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	float av = angle * M_PI / 180.0;
	float cv = cosf(av);
	float sv = sinf(av);
	int r, g, b;
	int tr, tg, tb;
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
	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			if(p[3] == 255)
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
			else
			{
				b = p[0] * 255 / p[3];
				g = p[1] * 255 / p[3];
				r = p[2] * 255 / p[3];
				tb = (m[6] * r + m[7] * g + m[8] * b) >> 16;
				tg = (m[3] * r + m[4] * g + m[5] * b) >> 16;
				tr = (m[0] * r + m[1] * g + m[2] * b) >> 16;
				p[0] = clamp(idiv255(tb * p[3]), 0, 255);
				p[1] = clamp(idiv255(tg * p[3]), 0, 255);
				p[2] = clamp(idiv255(tr * p[3]), 0, 255);
			}
		}
	}
}

void surface_filter_saturate(struct surface_t * s, int saturate)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	int v = clamp(saturate, -100, 100) * 128 / 100;
	int r, g, b, vmin, vmax;
	int alpha, delta, value, lv, sv;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			if(p[3] == 255)
			{
				b = p[0];
				g = p[1];
				r = p[2];
				vmin = min(min(r, g), b);
				vmax = max(max(r, g), b);
				delta = vmax - vmin;
				value = vmax + vmin;
				if(delta == 0)
					continue;
				lv = value >> 1;
				sv = lv < 128 ? (delta << 7) / value : (delta << 7) / (510 - value);
				if(v >= 0)
				{
					alpha = (v + sv >= 128) ? sv : 128 - v;
					if(alpha != 0)
						alpha = 128 * 128 / alpha - 128;
				}
				else
				{
					alpha = v;
				}
				r = r + ((r - lv) * alpha >> 7);
				g = g + ((g - lv) * alpha >> 7);
				b = b + ((b - lv) * alpha >> 7);
				p[0] = clamp(b, 0, 255);
				p[1] = clamp(g, 0, 255);
				p[2] = clamp(r, 0, 255);
			}
			else
			{
				b = p[0] * 255 / p[3];
				g = p[1] * 255 / p[3];
				r = p[2] * 255 / p[3];
				vmin = min(min(r, g), b);
				vmax = max(max(r, g), b);
				delta = vmax - vmin;
				value = vmax + vmin;
				if(delta == 0)
					continue;
				lv = value >> 1;
				sv = lv < 128 ? (delta << 7) / value : (delta << 7) / (510 - value);
				if(v >= 0)
				{
					alpha = (v + sv >= 128) ? sv : 128 - v;
					if(alpha != 0)
						alpha = 128 * 128 / alpha - 128;
				}
				else
				{
					alpha = v;
				}
				r = r + ((r - lv) * alpha >> 7);
				g = g + ((g - lv) * alpha >> 7);
				b = b + ((b - lv) * alpha >> 7);
				p[0] = clamp(idiv255(b * p[3]), 0, 255);
				p[1] = clamp(idiv255(g * p[3]), 0, 255);
				p[2] = clamp(idiv255(r * p[3]), 0, 255);
			}
		}
	}
}

void surface_filter_brightness(struct surface_t * s, int brightness)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	int t, v = clamp(brightness, -100, 100) * 255 / 100;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			if(p[3] == 255)
			{
				p[0] = clamp(p[0] + v, 0, 255);
				p[1] = clamp(p[1] + v, 0, 255);
				p[2] = clamp(p[2] + v, 0, 255);
			}
			else
			{
				t = idiv255(v * p[3]);
				p[0] = clamp(p[0] + t, 0, 255);
				p[1] = clamp(p[1] + t, 0, 255);
				p[2] = clamp(p[2] + t, 0, 255);
			}
		}
	}
}

void surface_filter_contrast(struct surface_t * s, int contrast)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	int v = clamp(contrast, -100, 100) * 128 / 100;
	int r, g, b;
	int tr, tg, tb;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			if(p[3] == 255)
			{
				b = p[0];
				g = p[1];
				r = p[2];
				tb = ((b << 7) + (b - 128) * v) >> 7;
				tg = ((g << 7) + (g - 128) * v) >> 7;
				tr = ((r << 7) + (r - 128) * v) >> 7;
				p[0] = clamp(tb, 0, 255);
				p[1] = clamp(tg, 0, 255);
				p[2] = clamp(tr, 0, 255);
			}
			else
			{
				b = p[0] * 255 / p[3];
				g = p[1] * 255 / p[3];
				r = p[2] * 255 / p[3];
				tb = ((b << 7) + (b - 128) * v) >> 7;
				tg = ((g << 7) + (g - 128) * v) >> 7;
				tr = ((r << 7) + (r - 128) * v) >> 7;
				p[0] = clamp(idiv255(tb * p[3]), 0, 255);
				p[1] = clamp(idiv255(tg * p[3]), 0, 255);
				p[2] = clamp(idiv255(tr * p[3]), 0, 255);
			}
		}
	}
}

void surface_filter_opacity(struct surface_t * s, int alpha)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	int v = clamp(alpha, 0, 100) * 256 / 100;

	switch(v)
	{
	case 0:
		memset(s->pixels, 0, s->pixlen);
		break;
	case 256:
		break;
	default:
		for(i = 0; i < len; i++, p += 4)
		{
			if(p[3] != 0)
			{
				p[0] = (p[0] * v) >> 8;
				p[1] = (p[1] * v) >> 8;
				p[2] = (p[2] * v) >> 8;
				p[3] = (p[3] * v) >> 8;
			}
		}
		break;
	}
}

void surface_filter_haldclut(struct surface_t * s, struct surface_t * clut, const char * type)
{
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	int stride = surface_get_stride(s);
	unsigned char * p, * q = surface_get_pixels(s);
	int cw = surface_get_width(clut);
	int ch = surface_get_height(clut);
	unsigned char * t, * cp, * cq = surface_get_pixels(clut);
	double sum[9];
	double dr, dg, db, xdr, xdg, xdb;
	int ri, gi, bi;
	int x, y, v;
	int level, level2, level_1, level_2;

	if(cw == ch)
	{
		switch(cw)
		{
		case 8:    level =  2 *  2; break;
		case 27:   level =  3 *  3; break;
		case 64:   level =  4 *  4; break;
		case 125:  level =  5 *  5; break;
		case 216:  level =  6 *  6; break;
		case 343:  level =  7 *  7; break;
		case 512:  level =  8 *  8; break;
		case 729:  level =  9 *  9; break;
		case 1000: level = 10 * 10; break;
		case 1331: level = 11 * 11; break;
		case 1728: level = 12 * 12; break;
		case 2197: level = 13 * 13; break;
		case 2744: level = 14 * 14; break;
		case 3375: level = 15 * 15; break;
		case 4096: level = 16 * 16; break;
		default:
			return;
		}
		level2 = level * level;
		level_1 = level - 1;
		level_2 = level - 2;
		switch(shash(type))
		{
		case 0x09fa48d7: /* "nearest" */
			for(y = 0; y < height; y++, q += stride)
			{
				for(x = 0, p = q; x < width; x++, p += 4)
				{
					if(p[3] != 0)
					{
						if(p[3] == 255)
						{
							bi = idiv255(p[0] * level_1);
							if(bi > level_2)
								bi = level_2;
							gi = idiv255(p[1] * level_1);
							if(gi > level_2)
								gi = level_2;
							ri = idiv255(p[2] * level_1);
							if(ri > level_2)
								ri = level_2;
							cp = cq + ((bi * level2 + gi * level + ri) << 2);
							p[0] = cp[0];
							p[1] = cp[1];
							p[2] = cp[2];
						}
						else
						{
							bi = p[0] * level_1 / p[3];
							if(bi > level_2)
								bi = level_2;
							gi = p[1] * level_1 / p[3];
							if(gi > level_2)
								gi = level_2;
							ri = p[2] * level_1 / p[3];
							if(ri > level_2)
								ri = level_2;
							cp = cq + ((bi * level2 + gi * level + ri) << 2);
							p[0] = idiv255(cp[0] * p[3]);
							p[1] = idiv255(cp[1] * p[3]);
							p[2] = idiv255(cp[2] * p[3]);
						}
					}
				}
			}
			break;
		case 0x860ab38f: /* "trilinear" */
			for(y = 0; y < height; y++, q += stride)
			{
				for(x = 0, p = q; x < width; x++, p += 4)
				{
					if(p[3] != 0)
					{
						if(p[3] == 255)
						{
							bi = idiv255(p[0] * level_1);
							if(bi > level_2)
								bi = level_2;
							gi = idiv255(p[1] * level_1);
							if(gi > level_2)
								gi = level_2;
							ri = idiv255(p[2] * level_1);
							if(ri > level_2)
								ri = level_2;
							db = (double)p[0] * level_1 / 255 - bi;
							dg = (double)p[1] * level_1 / 255 - gi;
							dr = (double)p[2] * level_1 / 255 - ri;
							xdb = 1 - db;
							xdg = 1 - dg;
							xdr = 1 - dr;
							cp = cq + ((bi * level2 + gi * level + ri) << 2);
							t = cp;
							sum[0] = (double)t[0] * xdr;
							sum[1] = (double)t[1] * xdr;
							sum[2] = (double)t[2] * xdr;
							t += 4;
							sum[0] += (double)t[0] * dr;
							sum[1] += (double)t[1] * dr;
							sum[2] += (double)t[2] * dr;
							t = cp + (level << 2);
							sum[3] = (double)t[0] * xdr;
							sum[4] = (double)t[1] * xdr;
							sum[5] = (double)t[2] * xdr;
							t += 4;
							sum[3] += (double)t[0] * dr;
							sum[4] += (double)t[1] * dr;
							sum[5] += (double)t[2] * dr;
							sum[6] = sum[0] * xdg + sum[3] * dg;
							sum[7] = sum[1] * xdg + sum[4] * dg;
							sum[8] = sum[2] * xdg + sum[5] * dg;
							t = cp + (level2 << 2);
							sum[0] = (double)t[0] * xdr;
							sum[1] = (double)t[1] * xdr;
							sum[2] = (double)t[2] * xdr;
							t += 4;
							sum[0] += (double)t[0] * dr;
							sum[1] += (double)t[1] * dr;
							sum[2] += (double)t[2] * dr;
							t = cp + ((level2 + level) << 2);
							sum[3] = (double)t[0] * xdr;
							sum[4] = (double)t[1] * xdr;
							sum[5] = (double)t[2] * xdr;
							t += 4;
							sum[3] += (double)t[0] * dr;
							sum[4] += (double)t[1] * dr;
							sum[5] += (double)t[2] * dr;
							sum[0] = sum[0] * xdg + sum[3] * dg;
							sum[1] = sum[1] * xdg + sum[4] * dg;
							sum[2] = sum[2] * xdg + sum[5] * dg;
							v = sum[6] * xdb + sum[0] * db;
							p[0] = clamp(v, 0, 255);
							v = sum[7] * xdb + sum[1] * db;
							p[1] = clamp(v, 0, 255);
							v = sum[8] * xdb + sum[2] * db;
							p[2] = clamp(v, 0, 255);
						}
						else
						{
							bi = p[0] * level_1 / p[3];
							if(bi > level_2)
								bi = level_2;
							gi = p[1] * level_1 / p[3];
							if(gi > level_2)
								gi = level_2;
							ri = p[2] * level_1 / p[3];
							if(ri > level_2)
								ri = level_2;
							db = (double)p[0] * level_1 / p[3] - bi;
							dg = (double)p[1] * level_1 / p[3] - gi;
							dr = (double)p[2] * level_1 / p[3] - ri;
							xdb = 1 - db;
							xdg = 1 - dg;
							xdr = 1 - dr;
							cp = cq + ((bi * level2 + gi * level + ri) << 2);
							t = cp;
							sum[0] = (double)t[0] * xdr;
							sum[1] = (double)t[1] * xdr;
							sum[2] = (double)t[2] * xdr;
							t += 4;
							sum[0] += (double)t[0] * dr;
							sum[1] += (double)t[1] * dr;
							sum[2] += (double)t[2] * dr;
							t = cp + (level << 2);
							sum[3] = (double)t[0] * xdr;
							sum[4] = (double)t[1] * xdr;
							sum[5] = (double)t[2] * xdr;
							t += 4;
							sum[3] += (double)t[0] * dr;
							sum[4] += (double)t[1] * dr;
							sum[5] += (double)t[2] * dr;
							sum[6] = sum[0] * xdg + sum[3] * dg;
							sum[7] = sum[1] * xdg + sum[4] * dg;
							sum[8] = sum[2] * xdg + sum[5] * dg;
							t = cp + (level2 << 2);
							sum[0] = (double)t[0] * xdr;
							sum[1] = (double)t[1] * xdr;
							sum[2] = (double)t[2] * xdr;
							t += 4;
							sum[0] += (double)t[0] * dr;
							sum[1] += (double)t[1] * dr;
							sum[2] += (double)t[2] * dr;
							t = cp + ((level2 + level) << 2);
							sum[3] = (double)t[0] * xdr;
							sum[4] = (double)t[1] * xdr;
							sum[5] = (double)t[2] * xdr;
							t += 4;
							sum[3] += (double)t[0] * dr;
							sum[4] += (double)t[1] * dr;
							sum[5] += (double)t[2] * dr;
							sum[0] = sum[0] * xdg + sum[3] * dg;
							sum[1] = sum[1] * xdg + sum[4] * dg;
							sum[2] = sum[2] * xdg + sum[5] * dg;
							v = (sum[6] * xdb + sum[0] * db) * p[3] / 255;
							p[0] = clamp(v, 0, 255);
							v = (sum[7] * xdb + sum[1] * db) * p[3] / 255;
							p[1] = clamp(v, 0, 255);
							v = (sum[8] * xdb + sum[2] * db) * p[3] / 255;
							p[2] = clamp(v, 0, 255);
						}
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

void surface_filter_blur(struct surface_t * s, int radius)
{
	int width = surface_get_width(s);
	int height = surface_get_height(s);

	if(radius > 0)
		expblur(surface_get_pixels(s), width, height, 0, 0, width, height, radius);
}
