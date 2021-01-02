/*
 * driver/camera/video.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
#include <jpeglib.h>
#include <jerror.h>
#include <camera/video.h>

static inline unsigned char yuv_to_red(int y, int u, int v)
{
	int r = y + (v - 128) + ((v - 128) * 103 >> 8);
	return (r < 0) ? 0 : ((r > 255) ? 255 : r);
}

static inline unsigned char yuv_to_green(int y, int u, int v)
{
	int g = y - ((u - 128) * 88 >> 8) - ((v - 128) * 183 >> 8);
	return (g < 0) ? 0 : ((g > 255) ? 255 : g);
}

static inline unsigned char yuv_to_blue(int y, int u, int v)
{
	int b = y + (u - 128) + ((u - 128) * 198 >> 8);
	return (b < 0) ? 0 : ((b > 255) ? 255 : b);
}

static inline void yuyv_to_argb(unsigned char * argb, unsigned char * yuv, int width, int height)
{
	unsigned char * q = argb;
	unsigned char * p = yuv;
	int len = width * height;
	int y0, u, y1, v;
	int i;

	for(i = 0; i < len; i += 2)
	{
		y0 = *p++;
		u = *p++;
		y1 = *p++;
		v = *p++;

		*q++ = yuv_to_blue(y0, u, v);
		*q++ = yuv_to_green(y0, u, v);
		*q++ = yuv_to_red(y0, u, v);
		*q++ = 255;

		*q++ = yuv_to_blue(y1, u, v);
		*q++ = yuv_to_green(y1, u, v);
		*q++ = yuv_to_red(y1, u, v);
		*q++ = 255;
	}
}

static inline void uyvy_to_argb(unsigned char * argb, unsigned char * yuv, int width, int height)
{
	unsigned char * q = argb;
	unsigned char * p = yuv;
	int len = width * height;
	int y0, u, y1, v;
	int i;

	for(i = 0; i < len; i += 2)
	{
		u = *p++;
		y0 = *p++;
		v = *p++;
		y1 = *p++;

		*q++ = yuv_to_blue(y0, u, v);
		*q++ = yuv_to_green(y0, u, v);
		*q++ = yuv_to_red(y0, u, v);
		*q++ = 255;

		*q++ = yuv_to_blue(y1, u, v);
		*q++ = yuv_to_green(y1, u, v);
		*q++ = yuv_to_red(y1, u, v);
		*q++ = 255;
	}
}

static inline void nv12_to_argb(unsigned char * argb, unsigned char * yuv, int width, int height)
{
	unsigned char * q = argb;
	unsigned char * py = yuv;
	unsigned char * puv = yuv + width * height;
	int y, u, v, t;
	int i, j;

	for(j = 0; j < height; j++)
	{
		for(i = 0; i < width; i++)
		{
			y = *py++;
			t = ((j >> 1) * ((width + 1) >> 1) + (i >> 1)) << 1;
			u = puv[t];
			v = puv[t + 1];

			*q++ = yuv_to_blue(y, u, v);
			*q++ = yuv_to_green(y, u, v);
			*q++ = yuv_to_red(y, u, v);
			*q++ = 255;
		}
	}
}

static inline void nv21_to_argb(unsigned char * argb, unsigned char * yuv, int width, int height)
{
	unsigned char * q = argb;
	unsigned char * py = yuv;
	unsigned char * puv = yuv + width * height;
	int y, u, v, t;
	int i, j;

	for(j = 0; j < height; j++)
	{
		for(i = 0; i < width; i++)
		{
			y = *py++;
			t = ((j >> 1) * ((width + 1) >> 1) + (i >> 1)) << 1;
			u = puv[t + 1];
			v = puv[t];

			*q++ = yuv_to_blue(y, u, v);
			*q++ = yuv_to_green(y, u, v);
			*q++ = yuv_to_red(y, u, v);
			*q++ = 255;
		}
	}
}

static inline void yu12_to_argb(unsigned char * argb, unsigned char * yuv, int width, int height)
{
	unsigned char * q = argb;
	unsigned char * py = yuv;
	unsigned char * pu = yuv + width * height;
	unsigned char * pv = yuv + width * height + ((width + 1) >> 1) * ((height + 1) >> 1);
	int y, u, v, t;
	int i, j;

	for(j = 0; j < height; j++)
	{
		for(i = 0; i < width; i++)
		{
			y = *py++;
			t = (j >> 1) * ((width + 1) >> 1) + (i >> 1);
			u = pu[t];
			v = pv[t];

			*q++ = yuv_to_blue(y, u, v);
			*q++ = yuv_to_green(y, u, v);
			*q++ = yuv_to_red(y, u, v);
			*q++ = 255;
		}
	}
}

static inline void yv12_to_argb(unsigned char * argb, unsigned char * yuv, int width, int height)
{
	unsigned char * q = argb;
	unsigned char * py = yuv;
	unsigned char * pu = yuv + width * height + ((width + 1) >> 1) * ((height + 1) >> 1);
	unsigned char * pv = yuv + width * height;
	int y, u, v, t;
	int i, j;

	for(j = 0; j < height; j++)
	{
		for(i = 0; i < width; i++)
		{
			y = *py++;
			t = (j >> 1) * ((width + 1) >> 1) + (i >> 1);
			u = pu[t];
			v = pv[t];

			*q++ = yuv_to_blue(y, u, v);
			*q++ = yuv_to_green(y, u, v);
			*q++ = yuv_to_red(y, u, v);
			*q++ = 255;
		}
	}
}

static const unsigned char dc_lumi_len[] = {
	0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
};
static const unsigned char dc_lumi_val[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};
static const unsigned char dc_chromi_len[] = {
	0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
};
static const unsigned char dc_chromi_val[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};
static const unsigned char ac_lumi_len[] = {
	0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d,
};
static const unsigned char ac_lumi_val[] = {
	0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21,
	0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71,
	0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1,
	0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72,
	0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25,
	0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
	0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
	0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83,
	0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93,
	0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3,
	0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3,
	0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
	0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3,
	0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
	0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1,
	0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa,
};
static const unsigned char ac_chromi_len[] = {
	0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77,
};
static const unsigned char ac_chromi_val[] = {
	0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31,
	0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22,
	0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1,
	0x09, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1,
	0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18,
	0x19, 0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36,
	0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47,
	0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a,
	0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a,
	0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a,
	0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa,
	0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba,
	0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca,
	0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
	0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
	0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa,
};
#define COPY_HUFFMAN_TABLE(dinfo,tbl,name) do { \
		if(dinfo->tbl == NULL) dinfo->tbl = jpeg_alloc_huff_table((j_common_ptr)dinfo); \
		memcpy(dinfo->tbl->bits, name##_len, sizeof(name##_len)); \
		memset(dinfo->tbl->huffval, 0, sizeof(dinfo->tbl->huffval)); \
		memcpy(dinfo->tbl->huffval, name##_val, sizeof(name##_val)); \
	} while(0)

static void insert_huff_tables(j_decompress_ptr dinfo)
{
	COPY_HUFFMAN_TABLE(dinfo, dc_huff_tbl_ptrs[0], dc_lumi);
	COPY_HUFFMAN_TABLE(dinfo, dc_huff_tbl_ptrs[1], dc_chromi);
	COPY_HUFFMAN_TABLE(dinfo, ac_huff_tbl_ptrs[0], ac_lumi);
	COPY_HUFFMAN_TABLE(dinfo, ac_huff_tbl_ptrs[1], ac_chromi);
}

struct x_error_mgr
{
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
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

static void mjpg_to_argb(unsigned char * argb, unsigned char * mjpg, int len, int stride)
{
	struct jpeg_decompress_struct dinfo;
	struct x_error_mgr jerr;
	JSAMPARRAY buf;
	unsigned char * p;
	int scanline, offset, i;

	dinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = x_error_exit;
	jerr.pub.emit_message = x_emit_message;
	if(setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_decompress(&dinfo);
		return;
	}
	jpeg_create_decompress(&dinfo);
	jpeg_mem_src(&dinfo, mjpg, len);
	jpeg_read_header(&dinfo, TRUE);
	if(dinfo.dc_huff_tbl_ptrs[0] == NULL)
		insert_huff_tables(&dinfo);
	dinfo.out_color_space = JCS_RGB;
	dinfo.dct_method = JDCT_IFAST;
	jpeg_start_decompress(&dinfo);
	buf = (*dinfo.mem->alloc_sarray)((j_common_ptr)&dinfo, JPOOL_IMAGE, dinfo.output_width * dinfo.output_components, 1);
	p = argb;
	while(dinfo.output_scanline < dinfo.output_height)
	{
		scanline = dinfo.output_scanline * stride;
		jpeg_read_scanlines(&dinfo, buf, 1);
		for(i = 0; i < dinfo.output_width; i++)
		{
			offset = scanline + (i * 4);
			p[offset + 3] = 0xff;
			p[offset + 2] = buf[0][(i * 3) + 0];
			p[offset + 1] = buf[0][(i * 3) + 1];
			p[offset + 0] = buf[0][(i * 3) + 2];
		}
	}
	jpeg_finish_decompress(&dinfo);
	jpeg_destroy_decompress(&dinfo);
}

void video_frame_to_argb(struct video_frame_t * frame, void * pixels)
{
	switch(frame->fmt)
	{
	case VIDEO_FORMAT_ARGB:
		memcpy(pixels, frame->buf, frame->buflen);
		break;
	case VIDEO_FORMAT_YUYV:
		yuyv_to_argb(pixels, frame->buf, frame->width, frame->height);
		break;
	case VIDEO_FORMAT_UYVY:
		uyvy_to_argb(pixels, frame->buf, frame->width, frame->height);
		break;
	case VIDEO_FORMAT_NV12:
		nv12_to_argb(pixels, frame->buf, frame->width, frame->height);
		break;
	case VIDEO_FORMAT_NV21:
		nv21_to_argb(pixels, frame->buf, frame->width, frame->height);
		break;
	case VIDEO_FORMAT_YU12:
		yu12_to_argb(pixels, frame->buf, frame->width, frame->height);
		break;
	case VIDEO_FORMAT_YV12:
		yv12_to_argb(pixels, frame->buf, frame->width, frame->height);
		break;
	case VIDEO_FORMAT_MJPG:
		mjpg_to_argb(pixels, frame->buf, frame->buflen, frame->width << 2);
		break;
	default:
		memcpy(pixels, frame->buf, frame->buflen);
		break;
	}
}
