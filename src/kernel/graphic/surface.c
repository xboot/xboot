/*
 * kernel/graphic/surface.c
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
#include <png.h>
#include <pngstruct.h>
#include <jpeglib.h>
#include <jerror.h>
#include <graphic/surface.h>

extern struct surface_operate_t surface_operate_cairo;

inline __attribute__((always_inline)) struct surface_operate_t * surface_operate_get(void)
{
	return &surface_operate_cairo;
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

	stride = width * 4;
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
	s->op = surface_operate_get();
	s->pctx = s->op->create(s);
	s->priv = priv;
	if(!s->pctx)
	{
		free(s);
		free(pixels);
		return NULL;
	}
	return s;
}

struct surface_t * surface_clone(struct surface_t * s)
{
	struct surface_t * c;
	void * pixels;

	if(!s)
		return NULL;

	c = malloc(sizeof(struct surface_t));
	if(!c)
		return NULL;

	pixels = memalign(4, s->pixlen);
	if(!pixels)
	{
		free(c);
		return NULL;
	}
	memcpy(pixels, s->pixels, s->pixlen);

	c->width = s->width;
	c->height = s->height;
	c->stride = s->stride;
	c->pixlen = s->pixlen;
	c->pixels = pixels;
	c->op = s->op;
	c->pctx = c->op->create(c);
	c->priv = NULL;
	if(!c->pctx)
	{
		free(c);
		free(pixels);
		return NULL;
	}
	return c;
}

void surface_free(struct surface_t * s)
{
	if(s)
	{
		if(s->op)
			s->op->destroy(s->pctx);
		free(s->pixels);
		free(s);
	}
}

void surface_clear(struct surface_t * s)
{
	if(s)
		memset(s->pixels, 0, s->pixlen);
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
