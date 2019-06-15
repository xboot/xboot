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
#include <graphic/surface.h>

extern struct surface_operate_t surface_operate_cairo;

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
	s->op = &surface_operate_cairo;
	s->priv = s->op->create(s);
	if(!s->priv)
	{
		free(s);
		free(pixels);
		return NULL;
	}
	return s;
}

struct surface_t * surface_alloc_from_jpeg(const char * filename)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	struct surface_t * s;
	FILE * file;
	JSAMPARRAY buf;
	unsigned char * p;
	int scanline, offset, i;

	if((file = fopen(filename, "rb")) == NULL)
		return NULL;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, file);
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
	fclose(file);

	return s;
}

struct surface_t * surface_alloc_from_jpeg_data(void * buffer, int length)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	struct surface_t * s;
	JSAMPARRAY buf;
	unsigned char * p;
	int scanline, offset, i;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, buffer, length);
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
