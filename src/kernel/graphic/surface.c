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
#include <jpeglib.h>
#include <jerror.h>
#include <graphic/surface.h>

extern struct surface_operate_t surface_operate_cairo;

inline __attribute__((always_inline)) struct surface_operate_t * surface_operate_get(void)
{
	return &surface_operate_cairo;
}

struct surface_t * surface_alloc(int width, int height)
{
	struct surface_t * s;
	void * pixels;

	if(width < 0 || height < 0)
		return NULL;

	s = malloc(sizeof(struct surface_t));
	if(!s)
		return NULL;

	pixels = memalign(4, height * (width << 2));
	if(!pixels)
	{
		free(s);
		return NULL;
	}

	s->width = width;
	s->height = height;
	s->stride = width << 2;
	s->pixels = pixels;
	s->op = surface_operate_get();
	s->priv = s->op->create(s);
	if(!s->priv)
	{
		free(s);
		free(pixels);
		return NULL;
	}
	return s;
}

void surface_free(struct surface_t * s)
{
	if(s)
	{
		s->op->destroy(s->priv);
		free(s->pixels);
		free(s);
	}
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

struct surface_t * surface_alloc_from_xfs_jpeg(struct xfs_context_t * ctx, const char * filename)
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
	s = surface_alloc(cinfo.image_width, cinfo.image_height);
	p = surface_get_pixels(s);
	while(cinfo.output_scanline < cinfo.output_height)
	{
		scanline = cinfo.output_scanline * surface_get_stride(s);
		jpeg_read_scanlines(&cinfo, buf, 1);
		for(i = 0; i < cinfo.output_width; i++)
		{
			offset = scanline + (i * 4);
			p[offset + 3] = 255;
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
